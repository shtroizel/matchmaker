#!/usr/bin/env python3

import os
import multiprocessing
import shutil
import subprocess
import sys
import getopt

from concurrent.futures import ThreadPoolExecutor



def usage():
    print('')
    print('    -h, --help                    print this message\n')
    print('    -b  --build_dir               build directory (default is <repo_root>/build)\n')
    print('    -i  --install_dir             install directory (default is <repo_root>/install)')
    print('                                  note that relative paths are relative to build_dir\n')
    print('    -c  --clang                   force use of clang compiler (default is system compiler)')
    print('                                    * experimental!')
    print('                                    * avoidance recommended\n')
    print('    -r  --retain                  retain previous build (continuation/incremental building)\n')
    print('    -m  --retain_matchables       retain generated matchables (implied when -r is used)')
    print('                                    * will fail if \'-q\'ness changes\n')
    print('    -j  --jobs                    max jobs (default is cpu count [' + \
            str(multiprocessing.cpu_count()) + '])')
    print('                                    * you probably want --mem... instead')
    print('                                    * avoidance recommended\n')
    print('    -a, --atomic_libs             instead of multi-libs, each prefix gets its own lib')
    print('                                    * good for stats')
    print('                                    * increases startup time for programs 5x when not q only')
    print('                                    * avoidance recommended except for q\n')
    print('    -q, --q                       q only (implies -a)')
    print('                                    * low RAM usage (less than 1G per job')
    print('                                    * builds quickly')
    print('                                    * starts quickly\n\n')
    print('    WHEN BUILDING EVERYTHING')
    print('    * memory consumption can rise high when -j is large')
    print('    * min req [15G RAM, 30G SWAP] applies to -j1 too')
    print('    * for memory limiting avoid -j and use --mem... instead')
    print('    * memory models are based on gcc, clang uses less\n')
    print('        --mem_127                 limit RAM consumption to 127G RAM\n')
    print('        --mem_63                  limit RAM consumption to 63G RAM\n')
    print('        --mem_31_14               limit RAM consumption to 31G where possible')
    print('                                  usage can spike so 14G SWAP is available\n')
    print('        --mem_15_30               limit memory consumption to 15G where possible')
    print('                                  usage can spike so 30G SWAP is available\n')



def build_and_install(use_clang, retain, retain_matchables, jobs, low_memory, repo_root, build_dir,
                      install_dir, a, q, mem_127, mem_63, mem_31_14, mem_15_30):
    start_dir = os.getcwd()

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
            if subprocess.run(build_matchable_cmd).returncode != 0:
                print('matchable failed to build')
                exit(1)

            # then build and install data reader
            build_data_reader_cmd = ['data_reader/scripts/build_and_install.py']
            if use_clang:
                build_data_reader_cmd.append('-c')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('data reader failed to build')
                exit(1)

            # run prepare_matchables with the data reader
            prepare_matchables_cmd = ['scripts/prepare_matchables.py']
            prepare_matchables_cmd.append('-l')
            prepare_matchables_cmd.append('data_reader/install')
            if q:
                prepare_matchables_cmd.append('-q')
            if subprocess.run(prepare_matchables_cmd).returncode != 0:
                print('prepare_matchables_cmd.py failed')
                exit(1)


    if build_dir == '':
        build_dir = repo_root + '/build/'

    if install_dir == '':
        install_dir = repo_root + '/install/'

    if not retain:
        shutil.rmtree(build_dir, ignore_errors=True)
        shutil.rmtree(install_dir, ignore_errors=True)

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
        os.makedirs(install_dir)

    os.chdir(build_dir)

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + install_dir]

    if q:
        cmake_cmd.append('-Dq=ON')
        a = True
    if a:
        cmake_cmd.append('-DATOMIC_LIBS=ON')
    if mem_15_30:
        cmake_cmd.append('-DMEM_15_30=ON')
    elif mem_31_14:
        cmake_cmd.append('-DMEM_31_14=ON')
    elif mem_63:
        cmake_cmd.append('-DMEM_63=ON')
    elif mem_127:
        cmake_cmd.append('-DMEM_127=ON')

    if use_clang:
        cmake_cmd.append('-DCMAKE_C_COMPILER=/usr/bin/clang')
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=/usr/bin/clang++')

    cmake_cmd.append('-Dmatchable_DIR=' + repo_root + '/matchable/install/lib/matchable/cmake')
    cmake_cmd.append(repo_root)

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
        opts, args = getopt.getopt(sys.argv[1:], 'hb:i:crmj:aq',
                ['help', 'build_dir', 'install_dir', 'clang', 'retain', 'retain_matchables', 'jobs',
                'atomic_libs', 'q', 'mem_127',  'mem_63', 'mem_31_14', 'mem_15_30'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    use_clang = False
    retain = False
    retain_matchables = False
    low_memory = False
    jobs = str(multiprocessing.cpu_count())
    build_dir = ''
    install_dir = ''
    a = False
    q = False
    mem_127 = False
    mem_63 = False
    mem_31_14 = False
    mem_15_30 = False

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
        elif o in ('-l', '--low_memory'):
            low_memory = True
        elif o in ('-j', '--jobs'):
            jobs = a
        elif o in ('-r', '--repo_root'):
            repo_root = a
        elif o in ('-b', '--build_dir'):
            build_dir = a
        elif o in ('-i', '--install_dir'):
            install_dir = a
        elif o in ('-a', '--atomic_libs'):
            a = True
        elif o in ('-q', '--q'):
            q = True
        elif o in ('--mem_127'):
            mem_127 = True
        elif o in ('--mem_63'):
            mem_63 = True
        elif o in ('--mem_31_14'):
            mem_31_14 = True
        elif o in ('--mem_15_30'):
            mem_15_30 = True
        else:
            assert False, "unhandled option"

    repo_root = os.path.dirname(os.path.realpath(__file__)) + '/../'

    build_and_install(use_clang, retain, retain_matchables, jobs, low_memory, repo_root, build_dir,
                      install_dir, a, q, mem_127, mem_63, mem_31_14, mem_15_30)

    exit(0)



if __name__ == "__main__":
    main()
