#!/usr/bin/env python3

import os
import shutil
import fileinput
import getopt
import sys

from string import ascii_lowercase
from string import ascii_uppercase





def usage():
    print(sys.argv[0] + ' [OPTION]')
    print('    -h, --help                  print this message')
    print('    -q, --q                     q only')
    print('    -p, --parents_only          update parents only')



def prepare_letters(repo_root, q, parents_only):
    if not parents_only:
        # reset header location
        shutil.rmtree(repo_root + '/generated_include/matchmaker/generated_letters', ignore_errors=True)
        os.makedirs(repo_root + '/generated_include/matchmaker/generated_letters')

        # reset impl location
        shutil.rmtree(repo_root + '/generated_src', ignore_errors=True)
        os.makedirs(repo_root + '/generated_src')

    leaf_h = repo_root + '/templates/leaf.h'
    leaf_cpp = repo_root + '/templates/leaf.cpp'
    parent_h = repo_root + '/templates/parent.h'
    parent_cpp = repo_root + '/templates/parent.cpp'

    for first_letter in ascii_uppercase + ascii_lowercase:

        if q:
            if first_letter != "q" and first_letter != "Q":
                continue

        if not parents_only:
            os.makedirs(repo_root + '/generated_include/matchmaker/generated_letters/' + first_letter)

        letter_h = repo_root + '/generated_include/matchmaker/generated_letters/' \
                             + first_letter + '/' + first_letter + '.h'

        letter_cpp = repo_root + '/generated_src/' + first_letter + '.cpp'

        #single_leaves = ascii_uppercase
        single_leaves = ascii_uppercase + 'jkvxyz'
        if not q:
            single_leaves = single_leaves + 'q'

        if not parents_only and first_letter in single_leaves:
            shutil.copy(leaf_h, letter_h)
            shutil.copy(leaf_cpp, letter_cpp)
            for filename in [letter_h, letter_cpp]:
                for line in fileinput.input(filename, inplace=True):
                    replaced = line.replace("aoeu", first_letter)
                    replaced = replaced.replace("snth", first_letter)
                    print(replaced, end='')
            continue

        shutil.copy(parent_h, letter_h)
        shutil.copy(parent_cpp, letter_cpp)
        for filename in [letter_h, letter_cpp]:
            for line in fileinput.input(filename, inplace=True):
                replaced = line.replace("aoeu", first_letter)
                replaced = replaced.replace("snth", first_letter)
                print(replaced, end='')


        for second_letter in ascii_lowercase:
            if not parents_only:
                os.makedirs(repo_root + '/generated_include/matchmaker/generated_letters/' + first_letter \
                                      + '/' + second_letter)
            letter_letter_h = repo_root + '/generated_include/matchmaker/generated_letters/' \
                                             + first_letter + '/' + second_letter + '/'  \
                                             + first_letter + '_' + second_letter + '.h'
            letter_letter_cpp = repo_root + '/generated_src/' + first_letter + '_' + second_letter + '.cpp'

            if (q and first_letter == 'q' and second_letter == 'u') or \
               (first_letter == 'p' and second_letter == 'r') or \
               (first_letter == 'n' and second_letter == 'o') or \
               (first_letter == 'u' and second_letter == 'n'):

                # two letter parent
                shutil.copy(parent_h, letter_letter_h)
                shutil.copy(parent_cpp, letter_letter_cpp)
                for filename in [letter_letter_h, letter_letter_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", first_letter + '/' + second_letter)
                        replaced = replaced.replace("snth", first_letter + '_' + second_letter)
                        print(replaced, end='')

                if parents_only:
                    continue

                for third_letter in ascii_lowercase:
                    os.makedirs(repo_root + '/generated_include/matchmaker/generated_letters/' \
                                          + first_letter + '/' + second_letter + '/' + third_letter)
                    letter_letter_letter_h = repo_root + '/generated_include/matchmaker/' \
                            + 'generated_letters/' + first_letter + '/' + second_letter + '/' \
                            + third_letter + '/' + first_letter + '_' + second_letter + '_' \
                            + third_letter + '.h'
                    letter_letter_letter_cpp = repo_root + '/generated_src/' + first_letter + '_' \
                            + second_letter + '_' + third_letter + '.cpp'

                    # three letter leaf
                    shutil.copy(leaf_h, letter_letter_letter_h)
                    shutil.copy(leaf_cpp, letter_letter_letter_cpp)
                    for filename in [letter_letter_letter_h, letter_letter_letter_cpp]:
                        for line in fileinput.input(filename, inplace=True):
                            replaced = line.replace("aoeu", first_letter + '/' + second_letter + '/' + \
                                                    third_letter)
                            replaced = replaced.replace("snth", first_letter + '_' + second_letter + '_' + \
                                                        third_letter)
                            print(replaced, end='')

            elif not parents_only:
                shutil.copy(leaf_h, letter_letter_h)
                shutil.copy(leaf_cpp, letter_letter_cpp)
                for filename in [letter_letter_h, letter_letter_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", first_letter + '/' + second_letter)
                        replaced = replaced.replace("snth", first_letter + '_' + second_letter)
                        print(replaced, end='')


def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hqp', ['help', 'q', 'parents_only'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    q = False
    parents_only = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-p', '--parents_only'):
            parents_only = True
        else:
            assert False, "unhandled option"


    repo_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    prepare_letters(repo_root, q, parents_only)
    exit(0)



if __name__ == "__main__":
    main()
