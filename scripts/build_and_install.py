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
    print('    -c  --clang                   build with clang')
    print('                                    * use system compiler by default\n')
    print('    -r  --retain                  retain previous build')
    print('                                    * continuation/incremental building')
    print('                                    * implies \'-l\'\n')
    print('    -l  --retain_leaves           retain most of previous build')
    print('                                    * parents are rebuilt')
    print('                                    * matchmaker interface is rebuilt')
    print('                                    * leaves are left alone')
    print('                                    * skips building of matchable')
    print('                                    * skips building and running of data readers\n')
    print('    -f  --force_stage_1           force stage 1 complete rebuild even when incremental')
    print('                                    * incremental as having -r or -l options\n')
    print('    -j  --jobs                    max jobs')
    print('                                    * default is cpu count [' +                                 \
            str(multiprocessing.cpu_count()) + ']\n')
    print('    -a, --atomic_libs             instead of multi-libs, each prefix gets its own lib')
    print('                                    * increases startup time for programs 80x')
    print('                                    * adds \'_atomic\' suffix to build_dir')
    print('                                    * adds \'_atomic\' suffix to library name in install_dir')
    print('                                    * avoidance recommended\n')
    print('    --dictionary=<usage>          which dictionary sources add words')
    print('                                    * all: every source (default)')
    print('                                    * reduced: all but 3201 SINGLE and COMPOUND')
    print('                                    * none: only the selected books')
    print('                                    * every source still annotates the words it finds\n')
    print('    --books=<books>               which books are included, adding their vocabulary')
    print('                                    * all: every book (default)')
    print('                                    * or a comma separated list of: ' + ', '.join(BOOKS) + '\n')
    print('    -q, --q                       q only')
    print('                                    * same as --dictionary=none --books=Crumbs')
    print('                                    * builds much faster')
    print('                                    * adds \'_q\' suffix to build_dir')
    print('                                    * adds \'_q\' suffix to library name in install_dir\n')
    print('    -d, --debug                   debug build\n')
    print('    -e, --export                  export every symbol, for loading units of words at run time')
    print('                                    * units are compiled with MATCHABLE_UNLOADABLE against the')
    print('                                      type header installed per leaf in leaf_types/')
    print('                                    * programs start slower and the link needs more memory\n')
    print('    -s, --skip_tests              skip the tests run before the stage 0 data reader')
    print('                                    * tests are in <matchmaker root>/test, built in Release')
    print('                                    * every label but mini_build, which is a whole build\n')
    print('    --data_dir=<dir>              data both data readers read instead of their own copies')
    print('                                    * same layout as the unpacked data_reader/data')
    print('                                    * for building from small test data\n')
    print('    --linker=<linker>             linker for the matchmaker libraries')
    print('                                    * gold, lld, mold, or another name the compiler takes')
    print('                                      as -fuse-ld=<linker>')
    print('                                    * default: the compiler\'s own linker')
    print('                                    * without it, gold when installed\n')
    print('    --time_compiles=<file>        append "<seconds> <peak RSS KB> <source>" to file per compile')
    print('                                    * covers matchable, the data readers and both stages')
    print('                                    * applies to build directories configured with it')
    print('                                    * Linux and macOS\n')



BOOKS = ['Crumbs']



# Ninja limits how many leaf libraries link at once; a build directory keeps the generator it was made with
def generator_args(build_dir):
    if os.path.exists(os.path.join(build_dir, 'CMakeCache.txt')):
        return []
    if os.name == 'nt' or which('ninja') is not None:
        return ['-G', 'Ninja']
    return []



def run_tests(matchmaker_root, test_build_dir, jobs, use_clang):
    print('\n\nbuild and run the matchmaker tests:\n\n')
    cmake_cmd = ['cmake', '-S', matchmaker_root + 'test', '-B', test_build_dir,
                 '-DCMAKE_BUILD_TYPE=Release']
    cmake_cmd += generator_args(test_build_dir)
    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            return False
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)
    for cmd in [cmake_cmd, ['cmake', '--build', test_build_dir, '-j' + jobs]]:
        if subprocess.run(cmd).returncode != 0:
            print('building the tests failed')
            return False
    ctest_cmd = ['ctest', '--output-on-failure', '-j' + jobs, '-LE', 'mini_build']
    if subprocess.run(ctest_cmd, cwd=test_build_dir).returncode != 0:
        print('\ntests failed (skip them with -s)')
        return False
    return True



def build_and_install(use_clang, retain, retain_leaves, force_stage_1, jobs,
                      build_dir, install_dir, atomic_libs, q, debug, dictionary, books, skip_tests,
                      data_dir, export, linker):

    start_dir = os.getcwd()

    matchmaker_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    os.chdir(matchmaker_root)

    suffix = ''
    if q:
        suffix = '_q'
    if atomic_libs:
        suffix = suffix + '_atomic'

    if build_dir == '':
        build_dir = matchmaker_root + 'build'
    if build_dir[0] != '/':
        build_dir = matchmaker_root + build_dir
    while build_dir[-1] == '/':
        build_dir = build_dir[:-1]
    build_dir = build_dir + suffix
    stage_0_build_dir = build_dir + '_stage_0/'
    build_dir = build_dir + '/'

    if install_dir == '':
        install_dir = matchmaker_root + 'install'
    while install_dir[-1] == '/':
        install_dir = install_dir[:-1]
    install_dir = install_dir + '/'
    abs_install_dir = install_dir
    if abs_install_dir[0] != '/':
        abs_install_dir = build_dir + install_dir
    stage_0_install_prefix = stage_0_build_dir + 'install/'

    stage_0_matchable_build_dir = stage_0_build_dir + 'matchable_build'
    stage_1_matchable_build_dir = build_dir + 'matchable_build'
    stage_0_data_reader_build_dir = stage_0_build_dir + 'data_reader_build'
    stage_0_data_reader_install_dir = stage_0_build_dir + 'data_reader_install'
    stage_1_data_reader_build_dir = build_dir + 'data_reader_build'
    stage_1_data_reader_install_dir = build_dir + 'data_reader_install'

    try:
        if not retain and not retain_leaves and os.path.exists(stage_0_build_dir):
            shutil.rmtree(stage_0_build_dir)
        if not os.path.exists(stage_0_build_dir):
            os.makedirs(stage_0_build_dir)
        if not os.path.exists(stage_0_install_prefix):
            os.makedirs(stage_0_install_prefix)
    except OSError as error:
        print("Failed to create matchmaker stage 0 build directory '%s'" % os.path.abspath(stage_0_build_dir))
        exit(1)

    try:
        if not retain and not retain_leaves and os.path.exists(build_dir):
            shutil.rmtree(build_dir)
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
    except OSError as error:
        print("Failed to create matchmaker stage 1 build directory '%s'" % os.path.abspath(build_dir))
        exit(1)

    try:
        if not os.path.exists(abs_install_dir):
            os.makedirs(abs_install_dir)
    except OSError as error:
        pass

    stage_0_workspace_dir = stage_0_build_dir + 'workspace/'
    leaf_list = stage_0_workspace_dir + 'leaf_list'

    # always update cmake files
    if os.path.exists(stage_0_workspace_dir):
        shutil.copy(matchmaker_root + 'workspace_common/CMakeLists.txt',
                    stage_0_workspace_dir + 'CMakeLists.txt')
        shutil.copy(matchmaker_root + 'workspace_common/config.cmake.in',
                    stage_0_workspace_dir + 'config.cmake.in')

    if not retain or not os.path.exists(stage_0_workspace_dir):

        # create stage 0 workspace as needed
        if not retain_leaves:
            shutil.rmtree(stage_0_workspace_dir, ignore_errors=True)
        if not os.path.exists(stage_0_workspace_dir):
            os.makedirs(stage_0_workspace_dir)
            shutil.copy(matchmaker_root + 'workspace_common/CMakeLists.txt',
                        stage_0_workspace_dir + 'CMakeLists.txt')
            shutil.copy(matchmaker_root + 'workspace_common/config.cmake.in',
                        stage_0_workspace_dir + 'config.cmake.in')

        if not retain_leaves and not skip_tests:
            if not run_tests(matchmaker_root, stage_0_build_dir + 'test_build', jobs, use_clang):
                os.chdir(start_dir)
                exit(1)

        if not retain_leaves:

            # first build and install matchable for each stage
            # matchable is included (installed with) all stages
            # for the final stage omit '-l' when building to offer a full install
            print('\n\nbuild and install matchable:\n\n')

            # build matchable for stage 0
            build_matchable_cmd = ['matchable/scripts/build_and_install.py']
            build_matchable_cmd.append('-j' + jobs)
            build_matchable_cmd.append('-l')
            if use_clang:
                build_matchable_cmd.append('-c')
            build_matchable_cmd.append('-b')
            build_matchable_cmd.append(stage_0_matchable_build_dir)
            build_matchable_cmd.append('-i')
            build_matchable_cmd.append(stage_0_install_prefix)
            if subprocess.run(build_matchable_cmd).returncode != 0:
                print('matchable failed to build for stage 0')
                exit(1)

        if not retain_leaves or force_stage_1:

            # build matchable for stage 1
            build_matchable_cmd = ['matchable/scripts/build_and_install.py']
            build_matchable_cmd.append('-j' + jobs)
            build_matchable_cmd.append('-l')
            if use_clang:
                build_matchable_cmd.append('-c')
            build_matchable_cmd.append('-b')
            build_matchable_cmd.append(stage_1_matchable_build_dir)
            build_matchable_cmd.append('-i')
            build_matchable_cmd.append(abs_install_dir)
            if subprocess.run(build_matchable_cmd).returncode != 0:
                print('matchable failed to build for stage 1')
                exit(1)

        if not retain_leaves:

            # then build and install data reader for stage 0
            build_data_reader_cmd = ['data_reader/stage_0/scripts/build_and_install.py']
            build_data_reader_cmd.append('-j' + jobs)
            build_data_reader_cmd.append('-b')
            build_data_reader_cmd.append(stage_0_data_reader_build_dir)
            build_data_reader_cmd.append('-i')
            build_data_reader_cmd.append(stage_0_data_reader_install_dir)
            build_data_reader_cmd.append('-m')
            build_data_reader_cmd.append(stage_0_install_prefix + '/lib/matchable/cmake')
            if use_clang:
                build_data_reader_cmd.append('-c')
            if debug:
                build_data_reader_cmd.append('-d')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('data reader failed to build')
                cmd_as_str = ""
                for c in build_data_reader_cmd:
                    cmd_as_str += ' '
                    cmd_as_str += c
                print('command was: ' + cmd_as_str)
                exit(1)

            # run data reader for stage 0
            print('\n\ncreating stage 0 matchables...')

            book_vocabulary_dir = stage_0_workspace_dir + "/book_vocabulary/"
            shutil.rmtree(book_vocabulary_dir, ignore_errors=True)
            os.makedirs(book_vocabulary_dir)

            reader_loc = stage_0_data_reader_install_dir
            data_reader_stage_0 = reader_loc + "/bin/data_reader_stage_0"
            data_reader_stage_0_data = reader_loc + "/share/matchmaker/data_reader_stage_0/data"
            if data_dir is not None:
                data_reader_stage_0_data = data_dir
            out_dir = stage_0_workspace_dir + "/generated_include/matchmaker/generated_matchables/"
            shutil.rmtree(out_dir, ignore_errors=True)
            os.makedirs(out_dir)

            run_data_reader_cmd = [data_reader_stage_0, data_reader_stage_0_data, book_vocabulary_dir,
                                   out_dir, matchmaker_root + 'scripts/parent_prefixes.txt', leaf_list,
                                   dictionary, books]
            if subprocess.run(run_data_reader_cmd).returncode != 0:
                print('\ndata_reader_stage_0 failed')
                build_cmd_as_str = ''
                for c in build_data_reader_cmd:
                    build_cmd_as_str += ' '
                    build_cmd_as_str += c
                print('\nbuild command was:' + build_cmd_as_str)
                cmd_as_str = ''
                for c in run_data_reader_cmd:
                    cmd_as_str += ' '
                    cmd_as_str += c
                print('  run command was:' + cmd_as_str)
                exit(1)

        print('\n\nmatchables are ready!')

        # generate code for the leaves the reader filled
        generate_code_cmd = [sys.executable, matchmaker_root + 'scripts/generate_code.py',
                             '-w', stage_0_workspace_dir, '-l', leaf_list]
        if retain_leaves:
            generate_code_cmd.append('-p')
        if subprocess.run(generate_code_cmd).returncode != 0:
            print('generate_code.py failed')
            exit(1)


        # create longest_words.h for stage 0
        stage_0_longest_word_file = stage_0_workspace_dir + 'generated_include/matchmaker/longest_words.h'
        with open(stage_0_longest_word_file, 'w') as f:
            f.write('#pragma once\n#include<vector>\n#include<map>\n')
            f.write('inline std::vector<int> const LONGEST_WORDS{0};\n')
            f.write('inline std::map<int, std::pair<int, int>> const LONGEST_WORDS_OFFSETS;\n\n')

        # create ordinal_summation.h for stage 0
        stage_0_ord_sum_file = stage_0_workspace_dir + 'generated_include/matchmaker/ordinal_summation.h'
        with open(stage_0_ord_sum_file, 'w') as f:
            f.write('#pragma once\n#include<array>\n#include<vector>\n')
            f.write('inline std::array<std::vector<int>, 1> const ORDINAL_SUMMATIONS;\n\n')



    os.chdir(stage_0_build_dir)

    print('\n\nbegin first stage matchmaker build - this will take a while...\n\n')

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + stage_0_install_prefix]
    cmake_cmd += generator_args('.')

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    cmake_cmd.append('-DMATCHMAKER_ROOT=' + matchmaker_root)

    if q:
        cmake_cmd.append('-Dq=ON')
    if atomic_libs:
        cmake_cmd.append('-DATOMIC_LIBS=ON')

    cmake_cmd.append('-DSTAGE0=ON')
    cmake_cmd.append('-DMM_LINKER=' + linker)

    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            exit(1)
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)

    cmake_cmd.append('-Dmatchable_DIR=' + stage_0_install_prefix + '/lib/matchable/cmake')
    cmake_cmd.append(stage_0_workspace_dir)

    if subprocess.run(cmake_cmd).returncode != 0:
        print('cmake failed')
        os.chdir(start_dir)
        exit(1)

    if subprocess.run(['cmake', '--build', '.', '-j', jobs, '--target', 'install']).returncode != 0:
        print('build failed')
        os.chdir(start_dir)
        exit(1)


    print('\n\n\n\n\n\n\n******* STAGE 0 COMPLETE *******\n\n\n\n\n\n')
    print('begin stage 1 ...\n\n')


    # stage 0 finished, begin stage 1...
    os.chdir(matchmaker_root)

    stage_1_workspace_dir = build_dir + 'workspace/'

    # always update cmake files
    if os.path.exists(stage_1_workspace_dir):
        shutil.copy(matchmaker_root + 'workspace_common/CMakeLists.txt',
                    stage_1_workspace_dir + 'CMakeLists.txt')
        shutil.copy(matchmaker_root + 'workspace_common/config.cmake.in',
                    stage_1_workspace_dir + 'config.cmake.in')

    if not retain or not os.path.exists(stage_1_workspace_dir) or force_stage_1:

        # recreate stage 1 workspace
        if not retain_leaves:
            shutil.rmtree(stage_1_workspace_dir, ignore_errors=True)
        if not os.path.exists(stage_1_workspace_dir):
            os.makedirs(stage_1_workspace_dir)
            shutil.copy(matchmaker_root + 'workspace_common/CMakeLists.txt',
                        stage_1_workspace_dir + 'CMakeLists.txt')
            shutil.copy(matchmaker_root + 'workspace_common/config.cmake.in',
                        stage_1_workspace_dir + 'config.cmake.in')

            shutil.copytree(stage_0_workspace_dir + 'generated_src', stage_1_workspace_dir + 'generated_src')
            if not os.path.exists(stage_1_workspace_dir + 'generated_src/books'):
                os.makedirs(stage_1_workspace_dir + 'generated_src/books')
            shutil.copy(matchmaker_root + 'workspace_common/BookWord.h',
                        stage_1_workspace_dir + 'generated_src/books/BookWord.h')
            shutil.copytree(stage_0_workspace_dir + 'book_vocabulary', stage_1_workspace_dir + 'book_vocabulary')
            os.makedirs(stage_1_workspace_dir + 'generated_include/matchmaker')
            os.makedirs(stage_1_workspace_dir + 'generated_include/matchmaker/books')
            shutil.copytree(stage_0_workspace_dir + 'generated_include/matchmaker/generated_symbols',
                            stage_1_workspace_dir + 'generated_include/matchmaker/generated_symbols')

        if not retain_leaves or not os.path.exists(
                stage_1_workspace_dir + 'generated_include/matchmaker/generated_matchables'):
            shutil.rmtree(stage_1_workspace_dir + 'generated_include/matchmaker/generated_matchables',
                          ignore_errors=True)
            shutil.copytree(stage_0_workspace_dir + 'generated_include/matchmaker/generated_matchables',
                            stage_1_workspace_dir + 'generated_include/matchmaker/generated_matchables')

        if not retain_leaves or force_stage_1:
            print('building data reader for stage 1 ...\n\n')

            # build and install stage 1 data reader
            build_data_reader_cmd = ['data_reader/stage_1/scripts/build_and_install.py']
            build_data_reader_cmd.append('-j' + jobs)
            build_data_reader_cmd.append('-b')
            build_data_reader_cmd.append(stage_1_data_reader_build_dir)
            build_data_reader_cmd.append('-i')
            build_data_reader_cmd.append(stage_1_data_reader_install_dir)
            build_data_reader_cmd.append('-m')
            build_data_reader_cmd.append(abs_install_dir + '/lib/matchable/cmake')
            build_data_reader_cmd.append('-s')
            build_data_reader_cmd.append(stage_0_install_prefix + '/lib/matchmaker' + suffix + '/cmake')
            if use_clang:
                build_data_reader_cmd.append('-c')
            if subprocess.run(build_data_reader_cmd).returncode != 0:
                print('\ndata_reader_stage_1 failed to build')
                cmd_as_str = ''
                for c in build_data_reader_cmd:
                    cmd_as_str += ' '
                    cmd_as_str += c
                print('command was:' + cmd_as_str)
                exit(1)

            print('\n\ndata reader for stage 1 ready!\n\n')
            print('\n\nreading stage 1 data ...\n\n')

            # run stage 1 data reader to create matchables for stage 1
            dr_stage_1_install_dir = stage_1_data_reader_install_dir
            dr_stage_1_binary = dr_stage_1_install_dir + '/bin/data_reader_stage_1'
            template_dir = matchmaker_root + '/templates'
            dr_stage_1_data = dr_stage_1_install_dir + '/share/matchmaker/data_reader_stage_1/data'
            if data_dir is not None:
                dr_stage_1_data = data_dir

            run_dr_stage_1_cmd = [dr_stage_1_binary, dr_stage_1_data, stage_1_workspace_dir, template_dir,
                                  jobs, books]
            if subprocess.run(run_dr_stage_1_cmd).returncode != 0:
                print('\ndata_reader_stage_1 failed')
                build_cmd_as_str = ''
                for c in build_data_reader_cmd:
                    build_cmd_as_str += ' '
                    build_cmd_as_str += c
                print('\nbuild command was:' + build_cmd_as_str)
                cmd_as_str = ''
                for c in run_dr_stage_1_cmd:
                    cmd_as_str += ' '
                    cmd_as_str += c
                print('  run command was:' + cmd_as_str)
                exit(1)

    print('\n\nMATCHMAKER!\n\n')

    shutil.copy(stage_0_workspace_dir + 'tree.cmake', stage_1_workspace_dir + 'tree.cmake')

    os.chdir(build_dir)

    cmake_cmd = ['cmake', '-DCMAKE_INSTALL_PREFIX=' + abs_install_dir]
    cmake_cmd += generator_args('.')

    if debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    cmake_cmd.append('-DMATCHMAKER_ROOT=' + matchmaker_root)

    if q:
        cmake_cmd.append('-Dq=ON')

    if atomic_libs:
        cmake_cmd.append('-DATOMIC_LIBS=ON')

    cmake_cmd.append('-DEXPORT_ALL=' + ('ON' if export else 'OFF'))
    cmake_cmd.append('-DMM_LINKER=' + linker)

    if use_clang:
        clang_c = which('clang')
        clang_cxx = which('clang++')
        if clang_c is None or clang_cxx is None:
            print('search for clang compiler failed')
            exit(1)
        cmake_cmd.append('-DCMAKE_C_COMPILER=' + clang_c)
        cmake_cmd.append('-DCMAKE_CXX_COMPILER=' + clang_cxx)

    cmake_cmd.append('-Dmatchable_DIR=' + abs_install_dir + '/lib/matchable/cmake')
    cmake_cmd.append(stage_1_workspace_dir)

    if subprocess.run(cmake_cmd).returncode != 0:
        print('cmake failed')
        os.chdir(start_dir)
        exit(1)

    if subprocess.run(['cmake', '--build', '.', '-j', jobs]).returncode != 0:
        print('build failed')
        os.chdir(start_dir)
        exit(1)

    if subprocess.run(['cmake', '--install', '.']).returncode != 0:
        print('install failed, trying sudo cmake --install ...')
        if subprocess.run(['sudo', 'cmake', '--install', '.']).returncode != 0:
            print('sudo cmake --install failed')
            os.chdir(start_dir)
            exit(1)

    os.chdir(start_dir)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hb:i:crlfj:aqdes',
                ['help', 'build_dir=', 'install_dir=', 'clang', 'retain', 'retain_leaves',
                 'force_stage_1', 'jobs=', 'atomic_libs', 'q',
                 'debug', 'export', 'dictionary=', 'books=', 'skip_tests', 'data_dir=', 'time_compiles=',
                 'linker='])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    use_clang = False
    retain = False
    retain_leaves = False
    force_stage_1 = False
    jobs = str(multiprocessing.cpu_count())
    build_dir = ''
    install_dir = ''
    atomic_libs = False
    q = False
    debug = False
    export = False
    dictionary = None
    books = None
    skip_tests = False
    data_dir = None
    time_compiles = None
    linker = ''

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-c', '--clang'):
            use_clang = True
        elif o in ('-r', '--retain'):
            retain = True
        elif o in ('-l', '--retain_leaves'):
            retain_leaves = True
        elif o in ('-f', '--force_stage_1'):
            force_stage_1 = True
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
        elif o == '--dictionary':
            dictionary = a
        elif o == '--books':
            books = a
        elif o in ('-d', '--debug'):
            debug = True
        elif o in ('-e', '--export'):
            export = True
        elif o in ('-s', '--skip_tests'):
            skip_tests = True
        elif o == '--data_dir':
            data_dir = os.path.abspath(a)
        elif o == '--time_compiles':
            time_compiles = os.path.abspath(a)
        elif o == '--linker':
            linker = a
        else:
            assert False, "unhandled option"

    if dictionary not in (None, 'all', 'reduced', 'none'):
        print('--dictionary must be one of: all, reduced, none')
        sys.exit(2)
    if q and dictionary not in (None, 'none'):
        print('-q builds with --dictionary=none and can\'t be combined with --dictionary=' + dictionary)
        sys.exit(2)
    if dictionary is None:
        dictionary = 'none' if q else 'all'

    if books is not None and books != 'all':
        selected = books.split(',')
        for book in selected:
            if book not in BOOKS:
                print('--books must be all or a comma separated list of: ' + ', '.join(BOOKS))
                sys.exit(2)
        books = ','.join([b for b in BOOKS if b in selected])
    if q and books not in (None, 'Crumbs'):
        print('-q builds with --books=Crumbs and can\'t be combined with --books=' + books)
        sys.exit(2)
    if books is None:
        books = 'Crumbs' if q else 'all'

    if time_compiles is not None:
        if os.name == 'nt':
            print('--time_compiles works on Linux and macOS')
            sys.exit(2)
        launcher = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'time_compile.py')
        os.environ['CMAKE_CXX_COMPILER_LAUNCHER'] = ';'.join([sys.executable, launcher, time_compiles])
        print('timing each compile into ' + time_compiles)

    build_and_install(use_clang, retain, retain_leaves, force_stage_1, jobs, build_dir,
                      install_dir, atomic_libs, q, debug, dictionary, books, skip_tests, data_dir, export,
                      linker)

    exit(0)



if __name__ == "__main__":
    main()
