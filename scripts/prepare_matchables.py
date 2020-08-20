#!/usr/bin/env python3

import os
import multiprocessing
import shutil
import subprocess
import sys
import getopt
from concurrent.futures import ThreadPoolExecutor

from string import ascii_lowercase
from string import ascii_uppercase



def usage():
    print(sys.argv[0] + ' [OPTION]')
    print('    -h, --help                       print this message')
    print('    -l, --data_reader_loc            data_reader installation prefix')
    print('    -q, --q                          q only')
    print('    -a, --atomic_libs                atomic_libs')



def prepare_generated_include(out_dir, reader_loc, q):
    shutil.rmtree(out_dir, ignore_errors=True)
    os.makedirs(out_dir)

    start_dir = os.getcwd()
    os.chdir(reader_loc)

    data_reader_stage_0 = reader_loc + '/bin/data_reader_stage_0'
    data_reader_stage_0_data = reader_loc + '/share/matchmaker/data_reader_stage_0/data'

    out_dir += '/'
    cmds = []

    for l0 in ascii_uppercase + ascii_lowercase:
        if q:
            if l0 != "q" and l0 != "Q":
                continue

        os.makedirs(out_dir + l0)

        for l1 in ascii_lowercase:
            os.makedirs(out_dir + l0 + '/' + l1)

            if (l0 == 'q' and l1 == 'u') or                                                                \
                    (l0 == 'p' and l1 == 'r') or                                                           \
                    (l0 == 'n' and l1 == 'o') or                                                           \
                    (l0 == 'u' and l1 == 'n') or                                                           \
                    (l0 == 'c' and l1 == 'o') or                                                           \
                    (l0 == 'i' and l1 == 'n') or                                                           \
                    (l0 == 'c' and l1 == 'a') or                                                           \
                    (l0 == 's' and l1 == 'u') or                                                           \
                    (l0 == 'd' and l1 == 'i') or                                                           \
                    (l0 == 'd' and l1 == 'e') or                                                           \
                    (l0 == 'p' and l1 == 'a') or                                                           \
                    (l0 == 's' and l1 == 't') or                                                           \
                    (l0 == 't' and l1 == 'r') or                                                           \
                    (l0 == 'r' and l1 == 'e') or                                                           \
                    (l0 == 'a' and l1 == 'n') or                                                           \
                    (l0 == 'c' and l1 == 'h') or                                                           \
                    (l0 == 'm' and l1 == 'a') or                                                           \
                    (l0 == 's' and l1 == 'e') or                                                           \
                    (l0 == 'o' and l1 == 'v') or                                                           \
                    (l0 == 'm' and l1 == 'i') or                                                           \
                    (l0 == 'p' and l1 == 'e') or                                                           \
                    (l0 == 'p' and l1 == 'o') or                                                           \
                    (l0 == 'm' and l1 == 'e') or                                                           \
                    (l0 == 'b' and l1 == 'a') or                                                           \
                    (l0 == 'b' and l1 == 'e') or                                                           \
                    (l0 == 'h' and l1 == 'e') or                                                           \
                    (l0 == 'h' and l1 == 'a') or                                                           \
                    (l0 == 'p' and l1 == 'h') or                                                           \
                    (l0 == 'm' and l1 == 'o') or                                                           \
                    (l0 == 't' and l1 == 'h') or                                                           \
                    (l0 == 'l' and l1 == 'a') or                                                           \
                    (l0 == 'e' and l1 == 'x') or                                                           \
                    (l0 == 's' and l1 == 'p') or                                                           \
                    (l0 == 'c' and l1 == 'r') or                                                           \
                    (l0 == 'h' and l1 == 'o') or                                                           \
                    (l0 == 'f' and l1 == 'o') or                                                           \
                    (l0 == 's' and l1 == 'c') or                                                           \
                    (l0 == 'b' and l1 == 'r') or                                                           \
                    (l0 == 'e' and l1 == 'n') or                                                           \
                    (l0 == 't' and l1 == 'e') or                                                           \
                    (l0 == 't' and l1 == 'a') or                                                           \
                    (l0 == 's' and l1 == 'a') or                                                           \
                    (l0 == 'h' and l1 == 'y') or                                                           \
                    (l0 == 'c' and l1 == 'l') or                                                           \
                    (l0 == 'a' and l1 == 'r') or                                                           \
                    (l0 == 'a' and l1 == 'l') or                                                           \
                    (l0 == 'l' and l1 == 'i') or                                                           \
                    (l0 == 'g' and l1 == 'a') or                                                           \
                    (l0 == 's' and l1 == 'o') or                                                           \
                    (l0 == 'r' and l1 == 'a') or                                                           \
                    (l0 == 'g' and l1 == 'r') or                                                           \
                    (l0 == 'b' and l1 == 'i') or                                                           \
                    (l0 == 'p' and l1 == 'l') or                                                           \
                    (l0 == 'a' and l1 == 'c') or                                                           \
                    (l0 == 't' and l1 == 'o') or                                                           \
                    (l0 == 'p' and l1 == 'i') or                                                           \
                    (l0 == 'o' and l1 == 'u') or                                                           \
                    (l0 == 'f' and l1 == 'l') or                                                           \
                    (l0 == 's' and l1 == 'i') or                                                           \
                    (l0 == 'b' and l1 == 'u') or                                                           \
                    (l0 == 'm' and l1 == 'u') or                                                           \
                    (l0 == 's' and l1 == 'h') or                                                           \
                    (l0 == 'd' and l1 == 'o') or                                                           \
                    (l0 == 'b' and l1 == 'o') or                                                           \
                    (l0 == 'n' and l1 == 'e') or                                                           \
                    (l0 == 'i' and l1 == 'm') or                                                           \
                    (l0 == 'l' and l1 == 'e') or                                                           \
                    (l0 == 'f' and l1 == 'a') or                                                           \
                    (l0 == 'f' and l1 == 'i') or                                                           \
                    (l0 == 'w' and l1 == 'a') or                                                           \
                    (l0 == 'a' and l1 == 'm') or                                                           \
                    (l0 == 'v' and l1 == 'i') or                                                           \
                    (l0 == 'v' and l1 == 'e') or                                                           \
                    (l0 == 'l' and l1 == 'o') or                                                           \
                    (l0 == 'f' and l1 == 'r') or                                                           \
                    (l0 == 'r' and l1 == 'o') or                                                           \
                    (l0 == 'b' and l1 == 'l') or                                                           \
                    (l0 == 'p' and l1 == 'u') or                                                           \
                    (l0 == 'a' and l1 == 'p') or                                                           \
                    (l0 == 'c' and l1 == 'u') or                                                           \
                    (l0 == 'a' and l1 == 's') or                                                           \
                    (l0 == 'c' and l1 == 'e') or                                                           \
                    (l0 == 'p' and l1 == 's') or                                                           \
                    (l0 == 'g' and l1 == 'e') or                                                           \
                    (l0 == 'f' and l1 == 'e') or                                                           \
                    (l0 == 'M' and l1 == 'a') or                                                           \
                    (l0 == 'a' and l1 == 'u') or                                                           \
                    (l0 == 'a' and l1 == 'd') or                                                           \
                    (l0 == 'd' and l1 == 'a') or                                                           \
                    (l0 == 'w' and l1 == 'i') or                                                           \
                    (l0 == 's' and l1 == 'y') or                                                           \
                    (l0 == 'g' and l1 == 'l') or                                                           \
                    (l0 == 'o' and l1 == 'r') or                                                           \
                    (l0 == 'v' and l1 == 'a') or                                                           \
                    (l0 == 'g' and l1 == 'o') or                                                           \
                    (l0 == 't' and l1 == 'i') or                                                           \
                    (l0 == 't' and l1 == 'u') or                                                           \
                    (l0 == 'e' and l1 == 'l') or                                                           \
                    (l0 == 'd' and l1 == 'r') or                                                           \
                    (l0 == 'a' and l1 == 'b') or                                                           \
                    (l0 == 'e' and l1 == 'p') or                                                           \
                    (l0 == 's' and l1 == 'l') or                                                           \
                    (l0 == 'c' and l1 == 'i') or                                                           \
                    (l0 == 'e' and l1 == 'm') or                                                           \
                    (l0 == 'w' and l1 == 'h') or                                                           \
                    (l0 == 'n' and l1 == 'a') or                                                           \
                    (l0 == 'f' and l1 == 'u') or                                                           \
                    (l0 == 'g' and l1 == 'u') or                                                           \
                    (l0 == 'r' and l1 == 'i'):

                for l2 in ascii_lowercase:
                    os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2)

                    if (l0 == 'p' and l1 == 'r' and l2 == 'e') or                                          \
                            (l0 == 'd' and l1 == 'i' and l2 == 's') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'n') or                                     \
                            (l0 == 'n' and l1 == 'o' and l2 == 'n') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'd') or                                     \
                            (l0 == 'p' and l1 == 'r' and l2 == 'o') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 's') or                                     \
                            (l0 == 's' and l1 == 'u' and l2 == 'b') or                                     \
                            (l0 == 'i' and l1 == 'n' and l2 == 't') or                                     \
                            (l0 == 's' and l1 == 'u' and l2 == 'p') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'c') or                                     \
                            (l0 == 'p' and l1 == 'a' and l2 == 'r') or                                     \
                            (l0 == 't' and l1 == 'r' and l2 == 'a') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'p') or                                     \
                            (l0 == 't' and l1 == 'r' and l2 == 'i') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'm') or                                     \
                            (l0 == 'c' and l1 == 'a' and l2 == 'r') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 'c') or                                     \
                            (l0 == 'a' and l1 == 'n' and l2 == 't') or                                     \
                            (l0 == 'o' and l1 == 'u' and l2 == 't') or                                     \
                            (l0 == 'p' and l1 == 'e' and l2 == 'r') or                                     \
                            (l0 == 'm' and l1 == 'i' and l2 == 's') or                                     \
                            (l0 == 'h' and l1 == 'y' and l2 == 'p') or                                     \
                            (l0 == 's' and l1 == 'e' and l2 == 'm') or                                     \
                            (l0 == 'f' and l1 == 'o' and l2 == 'r') or                                     \
                            (l0 == 'c' and l1 == 'h' and l2 == 'a') or                                     \
                            (l0 == 'p' and l1 == 'o' and l2 == 'l') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'a') or                                     \
                            (l0 == 'o' and l1 == 'v' and l2 == 'e') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 's') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'r') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'i') or                                     \
                            (l0 == 'm' and l1 == 'o' and l2 == 'n') or                                     \
                            (l0 == 'i' and l1 == 'm' and l2 == 'p') or                                     \
                            (l0 == 's' and l1 == 't' and l2 == 'r') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'r') or                                     \
                            (l0 == 's' and l1 == 't' and l2 == 'a') or                                     \
                            (l0 == 'i' and l1 == 'n' and l2 == 'c') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'e') or                                     \
                            (l0 == 'p' and l1 == 'l' and l2 == 'a') or                                     \
                            (l0 == 'm' and l1 == 'e' and l2 == 't') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 'f') or                                     \
                            (l0 == 'd' and l1 == 'e' and l2 == 'c') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'u'):

                        for l3 in ascii_lowercase:
                            os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3)

                            if (l0 == 'o' and l1 == 'v' and l2 == 'e' and l3 == 'r') or                    \
                                    (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e') or               \
                                    (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e') or               \
                                    (l0 == 'a' and l1 == 'n' and l2 == 't' and l3 == 'i') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'c' and l3 == 'o') or               \
                                    (l0 == 's' and l1 == 'e' and l2 == 'm' and l3 == 'i') or               \
                                    (l0 == 'n' and l1 == 'o' and l2 == 'n' and l3 == 'c'):

                                for l4 in ascii_lowercase:
                                    os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4)

                                    if (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e' and            \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e' and       \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e' and       \
                                                    l4 == 'r'):

                                        for l5 in ascii_lowercase:
                                            os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3      \
                                                    + '/' + l4 + '/' + l5)

                                            # six letter prefix
                                            cmds.append([
                                                data_reader_stage_0,
                                                data_reader_stage_0_data,
                                                out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4   \
                                                        + '/' + l5,
                                                l0,
                                                l1,
                                                l2,
                                                l3,
                                                l4,
                                                l5
                                            ])
                                    else:

                                        # five letter prefix
                                        cmds.append([
                                            data_reader_stage_0,
                                            data_reader_stage_0_data,
                                            out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4,
                                            l0,
                                            l1,
                                            l2,
                                            l3,
                                            l4,
                                            'nil'
                                        ])
                            else:

                                # four letter prefix
                                cmds.append([
                                    data_reader_stage_0,
                                    data_reader_stage_0_data,
                                    out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3,
                                    l0,
                                    l1,
                                    l2,
                                    l3,
                                    'nil',
                                    'nil'
                                ])
                    else:

                        # three letter prefix
                        cmds.append([
                            data_reader_stage_0,
                            data_reader_stage_0_data,
                            out_dir + l0 + '/' + l1 + '/' + l2,
                            l0,
                            l1,
                            l2,
                            'nil',
                            'nil',
                            'nil'
                        ])

            else:

                # two letter prefix
                cmds.append([
                    data_reader_stage_0,
                    data_reader_stage_0_data,
                    out_dir + l0 + '/' + l1,
                    l0,
                    l1,
                    'nil',
                    'nil',
                    'nil',
                    'nil'
                ])

    def prepare(cmd):
        subprocess.run(cmd)

    with ThreadPoolExecutor(multiprocessing.cpu_count()) as executor:
        executor.map(prepare, cmds)

    os.chdir(start_dir)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hl:qa', ['help', 'data_reader_loc', 'q', 'atomic_libs'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    reader_loc = ''
    q = False
    atomic_libs = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-l', '--data_reader_loc'):
            reader_loc = a
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-a', '--atomic_libs'):
            atomic_libs = True
        else:
            assert False, "unhandled option"

    if reader_loc == '':
        usage()
        sys.exit(2)

    repo_root = os.path.dirname(os.path.realpath(__file__)) + '/../'

    if reader_loc[0] != '/':
        reader_loc = repo_root + reader_loc

    out_dir = repo_root + '/generated_include'
    suffix = ''
    if q:
        suffix = '_q'
    elif atomic_libs:
        suffix = '_atomic'
    out_dir += suffix + '/'

    prepare_generated_include(out_dir + '/matchmaker/generated_matchables_stage_0/', reader_loc, q)
    exit(0)



if __name__ == "__main__":
    main()
