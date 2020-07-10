#!/usr/bin/env python3

import os
import multiprocessing
import shutil
import subprocess
import sys
import getopt

from concurrent.futures import ThreadPoolExecutor



def usage():
    print('options for building matchmaker:\n')
    print('    -h, --help                    print this message\n')
    print('    -b  --build_dir               build directory')
    print('                                    * default is <matchmaker root>/build')
    print('                                    * relative paths are relative to <matchmaker root>\n')
    print('    -i  --install_dir             install directory')
    print('                                    * default is <matchmaker root>/install')
    print('                                    * relative paths are relative to <build_dir>\n')
    print('    -c  --clang                   force use of clang compiler')
    print('                                    * use system compiler by default\n')
    print('    -r  --retain                  retain previous build')
    print('                                    * continuation/incremental building')
    print('                                    * implies \'-m\'\n')
    print('    -m  --retain_matchables       retain generated matchables')
    print('                                    * skips build matchable step')
    print('                                    * skips build data_reader step')
    print('                                    * skips run data_reader step\n')
    print('    -j  --jobs                    max jobs')
    print('                                    * default is cpu count [' +                                 \
            str(multiprocessing.cpu_count()) + '])')
    print('                                    * you probably want --mem... instead\n')
    print('    -a, --atomic_libs             instead of multi-libs, each prefix gets its own lib')
    print('                                    * increases startup time for programs 5x when not q only')
    print('                                    * avoidance recommended except for q\n')
    print('    -q, --q                       q only (implies -a)')
    print('                                    * low memory usage (less than 1 GB per job')
    print('                                    * adds \'_q\' suffix to build_dir')
    print('                                    * adds \'_q\' suffix to install_dir')
    print('                                    * builds quickly')
    print('                                    * starts quickly\n')
    print('    -l, --memory_usage_limit      memory usage limit in GB')
    print('                                    * usage can spike to 20GB regardless of value given')
    print('                                    * values under 20GB will be respected where possible\n')
    print('    -d, --debug                   debug build')
    print('                                    * compilation requires less memory')
    print('                                    * compiles faster\n')



def build_and_install(use_clang, retain, retain_matchables, jobs, build_dir, install_dir, atomic_libs,
                      q, memory_usage_limit, debug):
    start_dir = os.getcwd()

    matchmaker_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    os.chdir(matchmaker_root)

    suffix_q = ''
    if q:
        suffix_q = '_q'

    if not retain:
        prepare_letters_cmd = ['scripts/prepare_letters.py']
        if q:
            prepare_letters_cmd.append('-q')
        if subprocess.run(prepare_letters_cmd).returncode != 0:
            print('prepare_letters.py failed')
            exit(1)

        if not retain_matchables:

            # first build and install matchable
            build_matchable_cmd = ['matchable/scripts/build_and_install.py']
            build_matchable_cmd.append('-j' + jobs)
            build_matchable_cmd.append('-l')
            if use_clang:
                build_matchable_cmd.append('-c')
            if q:
                build_matchable_cmd.append('-b')
                build_matchable_cmd.append('build_q')
                build_matchable_cmd.append('-i')
                build_matchable_cmd.append('../install_q')

            if subprocess.run(build_matchable_cmd).returncode != 0:
                print('matchable failed to build')
                exit(1)

            # then build and install data reader
            build_data_reader_cmd = ['data_reader/scripts/build_and_install.py']
            if q:
                build_data_reader_cmd.append('-b')
                build_data_reader_cmd.append('build_q')
                build_data_reader_cmd.append('-i')
                build_data_reader_cmd.append('../install_q')
                build_data_reader_cmd.append('-m')
                build_data_reader_cmd.append('../matchable/install_q')
            if use_clang:
                build_data_reader_cmd.append('-c')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('data reader failed to build')
                exit(1)

            # run prepare_matchables with the data reader
            prepare_matchables_cmd = ['scripts/prepare_matchables.py']
            prepare_matchables_cmd.append('-l')
            prepare_matchables_cmd.append('data_reader/install' + suffix_q)
            if q:
                prepare_matchables_cmd.append('-q')
            if subprocess.run(prepare_matchables_cmd).returncode != 0:
                print('prepare_matchables_cmd.py failed')
                exit(1)


    if build_dir == '':
        build_dir = matchmaker_root + '/build'
    while build_dir[-1] == '/':
        build_dir = build_dir[:-1]
    build_dir = build_dir + suffix_q + '/'

    if install_dir == '':
        install_dir = matchmaker_root + '/install'
    while install_dir[-1] == '/':
        install_dir = install_dir[:-1]
    install_dir = install_dir + suffix_q + '/'

    if not retain:
        shutil.rmtree(build_dir, ignore_errors=True)
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    os.chdir(build_dir)

    if not retain:
        shutil.rmtree(install_dir, ignore_errors=True)
    if not os.path.exists(install_dir):
        os.makedirs(install_dir)

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + install_dir]

    cmake_cmd.append('-DJOBS=' + jobs)

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    if q:
        cmake_cmd.append('-Dq=ON')
        atomic_libs = True
    if atomic_libs:
        cmake_cmd.append('-DATOMIC_LIBS=ON')

    memory_usage_limit_is_number = len(memory_usage_limit) > 0
    for digit in memory_usage_limit:
        if digit < '0' or digit > '9':
            memory_usage_limit_is_number = False

    if memory_usage_limit_is_number:
        cmake_cmd.append('-DMEM_LIMIT=' + memory_usage_limit)

    if use_clang:
        if debug:
            cmake_cmd.append('-DMEM_MODEL_CLANG_DEBUG=ON')
        cmake_cmd.append('-DCMAKE_C_COMPILER=/usr/bin/clang')
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=/usr/bin/clang++')

    cmake_cmd.append('-Dmatchable_DIR=' + matchmaker_root + '/matchable/install' + suffix_q + \
            '/lib/matchable/cmake')
    cmake_cmd.append(matchmaker_root)

    if subprocess.run(cmake_cmd).returncode != 0:
        print('cmake failed')
        os.chdir(start_dir)
        exit(1)

    if subprocess.run(['make', '-j' + jobs, 'install']).returncode != 0:
        print('make failed')
        os.chdir(start_dir)
        exit(1)

    os.chdir(start_dir)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hb:i:crmj:aql:d',
                ['help', 'build_dir', 'install_dir', 'clang', 'retain', 'retain_matchables', 'jobs',
                'atomic_libs', 'q', 'memory_usage_limit', 'debug'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    use_clang = False
    retain = False
    retain_matchables = False
    jobs = str(multiprocessing.cpu_count())
    build_dir = ''
    install_dir = ''
    atomic_libs = False
    q = False
    memory_usage_limit = ''
    debug = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-c', '--clang'):
            use_clang = True
        elif o in ('-r', '--retain'):
            retain = True
        elif o in ('-m', '--retain_matchables'):
            retain_matchables = True
        elif o in ('-j', '--jobs'):
            jobs = a
        elif o in ('-b', '--build_dir'):
            build_dir = a
        elif o in ('-i', '--install_dir'):
            install_dir = a
        elif o in ('-a', '--atomic_libs'):
            atomic_libs = True
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-l, --memory_usage_limit'):
            memory_usage_limit = a
        elif o in ('-d, --debug'):
            debug = True
        else:
            assert False, "unhandled option"


    build_and_install(use_clang, retain, retain_matchables, jobs, build_dir, install_dir, atomic_libs,
                      q, memory_usage_limit, debug)

    exit(0)



if __name__ == "__main__":
    main()
