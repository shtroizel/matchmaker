#!/usr/bin/env python3

import os
import multiprocessing
import shutil
import subprocess
import sys
import getopt

from concurrent.futures import ThreadPoolExecutor
from shutil import which


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
            str(multiprocessing.cpu_count()) + '])\n')
    print('    -a, --atomic_libs             instead of multi-libs, each prefix gets its own lib')
    print('                                    * increases startup time for programs 80x')
    print('                                    * adds \'_atomic\' suffix to build_dir')
    print('                                    * adds \'_atomic\' suffix to install_dir')
    print('                                    * avoidance recommended\n')
    print('    -q, --q                       q only')
    print('                                    * adds \'_q\' suffix to build_dir')
    print('                                    * adds \'_q\' suffix to install_dir\n')
    print('    -d, --debug                   debug build\n')



def build_and_install(use_clang, retain, retain_matchables, jobs, build_dir, install_dir, atomic_libs,
                      q, debug):
    print('\n\n************** matchmaker **************\n\n')
    print('starting stage 0 ...\n\n')

    start_dir = os.getcwd()

    matchmaker_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    os.chdir(matchmaker_root)

    suffix = ''
    if q:
        suffix = '_q'
    elif atomic_libs:
        suffix = '_atomic'

    if not retain:
        print('generate matchmaker code...\n')
        prepare_letters_cmd = ['scripts/prepare_letters.py']
        if q:
            prepare_letters_cmd.append('-q')
        if atomic_libs:
            prepare_letters_cmd.append('-a')
        if subprocess.run(prepare_letters_cmd).returncode != 0:
            print('prepare_letters.py failed')
            exit(1)
        print('\n\nmatchmaker code ready!')

        if not retain_matchables:

            # first build and install matchable
            print('\n\nbuild and install matchable:\n\n')
            build_matchable_cmd = ['matchable/scripts/build_and_install.py']
            build_matchable_cmd.append('-j' + jobs)
            build_matchable_cmd.append('-l')
            if use_clang:
                build_matchable_cmd.append('-c')

            build_matchable_cmd.append('-b')
            build_matchable_cmd.append('build' + suffix)
            build_matchable_cmd.append('-i')
            build_matchable_cmd.append('../install' + suffix)

            if subprocess.run(build_matchable_cmd).returncode != 0:
                print('matchable failed to build')
                exit(1)
            print('\n\nmatchable ready!')

            # then build and install data reader for stage 0
            print('\n\nbuild and install data reader for stage 0:\n\n')
            build_data_reader_cmd = ['data_reader/stage_0/scripts/build_and_install.py']
            build_data_reader_cmd.append('-b')
            build_data_reader_cmd.append('build' + suffix)
            build_data_reader_cmd.append('-i')
            build_data_reader_cmd.append('../install' + suffix)
            build_data_reader_cmd.append('-m')
            build_data_reader_cmd.append('../../matchable/install' + suffix)
            if use_clang:
                build_data_reader_cmd.append('-c')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('data reader failed to build')
                exit(1)
            print('\n\nstage 0 data reader ready!')

            # prepare_matchables for stage 0
            print('\n\ncreate stage 0 matchables...')
            prepare_matchables_cmd = ['scripts/prepare_matchables.py']
            prepare_matchables_cmd.append('-l')
            prepare_matchables_cmd.append('data_reader/stage_0/install' + suffix)
            if q:
                prepare_matchables_cmd.append('-q')
            if atomic_libs:
                prepare_matchables_cmd.append('-a')
            if subprocess.run(prepare_matchables_cmd).returncode != 0:
                print('failed to prepare matchables for stage 0')
                exit(1)
            print('\n\nstage 0 matchables ready!')


    # note: stage 0 builds and installs within the build dir

    if build_dir == '':
        build_dir = matchmaker_root + '/build'
    while build_dir[-1] == '/':
        build_dir = build_dir[:-1]
    build_dir = build_dir + suffix + '/'
    stage_0_build_dir = build_dir + 'build_stage_0/'

    if install_dir == '':
        install_dir = matchmaker_root + '/install'
    while install_dir[-1] == '/':
        install_dir = install_dir[:-1]
    install_dir = install_dir + suffix + '/'
    stage_0_install_dir = build_dir + 'install_stage_0/'

    if not retain:
        shutil.rmtree(install_dir, ignore_errors=True)
        shutil.rmtree(build_dir, ignore_errors=True)
    if not os.path.exists(install_dir):
        os.makedirs(install_dir)
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    if not os.path.exists(stage_0_build_dir):
        os.makedirs(stage_0_build_dir)
    if not os.path.exists(stage_0_install_dir):
        os.makedirs(stage_0_install_dir)

    os.chdir(stage_0_build_dir)

    print('\n\nMATCHMAKER!\n\n')

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + stage_0_install_dir]

    cmake_cmd.append('-Dstage=0')

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    if q:
        cmake_cmd.append('-Dq=ON')
    if atomic_libs:
        cmake_cmd.append('-DATOMIC_LIBS=ON')

    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            exit(1)
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)

    cmake_cmd.append('-Dmatchable_DIR=' + matchmaker_root + '/matchable/install' + suffix + \
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


    print('\n\n\n\n\n\n\n******* STAGE 0 COMPLETE *******\n\n\n\n\n\n')
    print('begin stage 1 ...\n\n')


    # stage 0 finished, begin stage 1...
    os.chdir(matchmaker_root)

    stage_1_matchables_dir = matchmaker_root + 'generated_include' + suffix                                \
            + '/matchmaker/generated_matchables_stage_1'

    if not retain or not os.path.exists(stage_1_matchables_dir):

        if not retain_matchables or not os.path.exists(stage_1_matchables_dir):
            shutil.rmtree(stage_1_matchables_dir, ignore_errors=True)

            print('building data reader for stage 1 ...\n\n')

            # build and install stage 1 data reader
            build_data_reader_cmd = ['data_reader/stage_1/scripts/build_and_install.py']
            build_data_reader_cmd.append('-b')
            build_data_reader_cmd.append('build' + suffix)
            build_data_reader_cmd.append('-i')
            build_data_reader_cmd.append('../install' + suffix)
            build_data_reader_cmd.append('-m')
            build_data_reader_cmd.append('../../matchable/install' + suffix)
            build_data_reader_cmd.append('-s')
            build_data_reader_cmd.append(stage_0_install_dir)
            if use_clang:
                build_data_reader_cmd.append('-c')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('data_reader_stage_1 failed to build')
                exit(1)

            print('\n\ndata reader for stage 1 ready!\n\n')
            print('\n\nreading stage 1 data ...\n\n')

            # run stage 1 data reader to create matchables for stage 1
            dr_stage_1_install_dir = 'data_reader/stage_1/install' + suffix
            dr_stage_1_binary = dr_stage_1_install_dir + '/bin/data_reader_stage_1'
            dr_stage_1_data = dr_stage_1_install_dir + '/share/matchmaker/data_reader_stage_1/data'
            stage_0_matchables_dir = matchmaker_root + 'generated_include' + suffix                        \
                    + '/matchmaker/generated_matchables_stage_0/'

            run_dr_stage_1_cmd = [dr_stage_1_binary, dr_stage_1_data, stage_0_matchables_dir]
            if subprocess.run(run_dr_stage_1_cmd).returncode != 0:
                print('data_reader_stage_1 failed')
                exit(1)

    print('\n\nMATCHMAKER!\n\n')

    os.chdir(build_dir)

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + install_dir]

    cmake_cmd.append('-Dstage=1')

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    if q:
        cmake_cmd.append('-Dq=ON')
    if atomic_libs:
        cmake_cmd.append('-DATOMIC_LIBS=ON')

    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            exit(1)
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)

    cmake_cmd.append('-Dmatchable_DIR=' + matchmaker_root + '/matchable/install' + suffix + \
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
        elif o in ('-d, --debug'):
            debug = True
        else:
            assert False, "unhandled option"

    build_and_install(use_clang, retain, retain_matchables, jobs, build_dir, install_dir, atomic_libs,
                      q, debug)

    exit(0)



if __name__ == "__main__":
    main()
