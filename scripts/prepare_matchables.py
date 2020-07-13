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



def prepare_generated_include(out_dir, reader_loc, q):
    shutil.rmtree(out_dir, ignore_errors=True)
    os.makedirs(out_dir)

    start_dir = os.getcwd()
    os.chdir(reader_loc)

    data_reader_stage_0 = reader_loc + '/bin/data_reader_stage_0'
    data_reader_stage_0_data = reader_loc + '/share/matchmaker/data_reader_stage_0/data'

    out_dir += '/'
    cmds = []

    for first_letter in ascii_uppercase + ascii_lowercase:
        if q:
            if first_letter != "q" and first_letter != "Q":
                continue

        os.makedirs(out_dir + first_letter)

        single_leaves = ascii_uppercase + 'jkxyz'
        if not q:
            single_leaves = single_leaves + 'q'

        if first_letter in single_leaves:
            cmds.append([
                    data_reader_stage_0,
                    data_reader_stage_0_data,
                    out_dir + first_letter,
                    first_letter,
                    'nil',
                    'nil',
                    'nil'
                ])
            continue

        for second_letter in ascii_lowercase:
            os.makedirs(out_dir + first_letter + '/' + second_letter)

            if (q and first_letter == 'q' and second_letter == 'u') or \
               (first_letter == 'p' and second_letter == 'r') or \
               (first_letter == 'n' and second_letter == 'o') or \
               (first_letter == 'c' and second_letter == 'o') or \
               (first_letter == 'r' and second_letter == 'e') or \
               (first_letter == 'i' and second_letter == 'n') or \
               (first_letter == 'c' and second_letter == 'a') or \
               (first_letter == 's' and second_letter == 'u') or \
               (first_letter == 'd' and second_letter == 'i') or \
               (first_letter == 'd' and second_letter == 'e') or \
               (first_letter == 'p' and second_letter == 'a') or \
               (first_letter == 's' and second_letter == 't') or \
               (first_letter == 't' and second_letter == 'r') or \
               (first_letter == 'u' and second_letter == 'n'):

                for third_letter in ascii_lowercase:
                    os.makedirs(out_dir + first_letter + '/' + second_letter + '/' + third_letter)
                    cmds.append([
                        data_reader_stage_0,
                        data_reader_stage_0_data,
                        out_dir + first_letter + '/' + second_letter + '/' + third_letter,
                        first_letter,
                        second_letter,
                        third_letter,
                        'nil'
                    ])

            else:
                cmds.append([
                    data_reader_stage_0,
                    data_reader_stage_0_data,
                    out_dir + first_letter + '/' + second_letter,
                    first_letter,
                    second_letter,
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
        opts, args = getopt.getopt(sys.argv[1:], 'hl:q', ['help', 'data_reader_loc', 'q'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    reader_loc = ''
    q = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-l', '--data_reader_loc'):
            reader_loc = a
        elif o in ('-q', '--q'):
            q = True
        else:
            assert False, "unhandled option"

    if reader_loc == '':
        usage()
        sys.exit(2)

    repo_root = os.path.dirname(os.path.realpath(__file__)) + '/../'

    if reader_loc[0] != '/':
        reader_loc = repo_root + reader_loc

    out_dir = repo_root + '/generated_include'
    if q:
        out_dir = out_dir + '_q'
    out_dir = out_dir + '/'

    prepare_generated_include(out_dir + '/matchmaker/generated_matchables/', reader_loc, q)
    exit(0)



if __name__ == "__main__":
    main()
