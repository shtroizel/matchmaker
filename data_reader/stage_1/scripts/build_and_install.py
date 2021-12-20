#!/usr/bin/env python3

import os
import multiprocessing
import shutil
import subprocess
import sys
import getopt
from shutil import which


def usage():
    print('options for building data_reader_stage_1:\n')
    print('    -h, --help              print this message\n')
    print('    -b  --build_dir         build directory')
    print('                              * defaults to <data_reader_stage_1 root>/build')
    print('                              * relative paths are relative to <data_reader_stage_1 root>\n')
    print('    -i  --install_dir       install directory')
    print('                              * defaults to <data_reader_stage_1 root>/install')
    print('                              * relative paths are relative to build_dir\n')
    print('    -m  --matchable_dir     location of installed matchable cmake directory')
    print('                              * defaults to <data_reader_stage_1 root>/../../matchable/'\
                                               'install_mm/lib/matchable/cmake\n')
    print('    -s  --stage_0_dir       location of installed stage 0 matchmaker cmake directory\n')
    print('    -c  --clang             force use of clang compiler')
    print('                              * system compiler used by default\n')
    print('    -d  --debug             debug build\n')



def build_and_install(build_dir, install_dir, matchable_dir, stage_0_dir, use_clang, debug):
    start_dir = os.getcwd()

    data_reader_stage_1_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    os.chdir(data_reader_stage_1_root)

    if build_dir == '':
        build_dir = data_reader_stage_1_root + '/build'
    while build_dir[-1] == '/':
        build_dir = build_dir[:-1]

    if install_dir == '':
        install_dir = data_reader_stage_1_root + '/install'
    while install_dir[-1] == '/':
        install_dir = install_dir[:-1]

    if matchable_dir == '':
        matchable_dir = data_reader_stage_1_root + '/../../matchable/install_mm/lib/matchable/cmake'

    build_dir = build_dir + '/'
    install_dir = install_dir + '/'

    shutil.rmtree(build_dir, ignore_errors=True)
    os.makedirs(build_dir)
    os.chdir(build_dir)

    shutil.rmtree(install_dir, ignore_errors=True)
    os.makedirs(install_dir)

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + install_dir]
    cmake_cmd.append('-Dmatchable_DIR=' + matchable_dir)
    cmake_cmd.append('-Dmatchmaker_DIR=' + stage_0_dir)

    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            exit(1)
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    cmake_cmd.append(data_reader_stage_1_root)

    if subprocess.run(cmake_cmd).returncode != 0:
        print('cmake failed')
        os.chdir(data_reader_stage_1_root)
        exit(1)

    if subprocess.run(['make', '-j' + str(multiprocessing.cpu_count()), 'install']).returncode != 0:
        print('make failed')
        os.chdir(data_reader_stage_1_root)
        exit(1)

    os.chdir(start_dir)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hcb:i:m:s:d',
                ['help', 'clang', 'build_dir', 'install_dir', 'matchable_dir', 'stage_0_dir' 'debug'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    use_clang = False
    build_dir = ''
    install_dir = ''
    matchable_dir = ''
    stage_0_dir = ''
    debug = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-c', '--clang'):
            use_clang = True
        elif o in ('-b', '--build_dir'):
            build_dir = a
        elif o in ('-i', '--install_dir'):
            install_dir = a
        elif o in ('-m', '--matchable_dir'):
            matchable_dir = a
        elif o in ('-s', '--stage_0_dir'):
            stage_0_dir = a
        elif o in ('-d', '--debug'):
            debug = True
        else:
            assert False, "unhandled option"

    build_and_install(build_dir, install_dir, matchable_dir, stage_0_dir, use_clang, debug)

    exit(0)



if __name__ == "__main__":
    main()
