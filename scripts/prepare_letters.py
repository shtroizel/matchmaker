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
    print('    -a, --atomic_libs           atomic_libs')
    print('    -p, --parents_only          update parents only')



def prepare_letters(repo_root, q, atomic_libs, parents_only):
    suffix = ''
    if q:
        suffix = '_q'
    elif atomic_libs:
        suffix = '_atomic'

    generated_include = repo_root + '/generated_include' + suffix + '/'
    generated_src = repo_root + '/generated_src' + suffix + '/'

    if not parents_only:
        # reset header location
        shutil.rmtree(generated_include + '/matchmaker/generated_letters', ignore_errors=True)
        os.makedirs(generated_include + '/matchmaker/generated_letters')

        # reset impl location
        shutil.rmtree(generated_src, ignore_errors=True)
        os.makedirs(generated_src)

    leaf_h = repo_root + '/templates/leaf.h'
    leaf_cpp = repo_root + '/templates/leaf.cpp'
    parent_h = repo_root + '/templates/parent.h'
    parent_cpp = repo_root + '/templates/parent.cpp'

    for first_letter in ascii_uppercase + ascii_lowercase:

        if q:
            if first_letter != "q" and first_letter != "Q":
                continue

        if not parents_only:
            os.makedirs(generated_include + '/matchmaker/generated_letters/' + first_letter)

        letter_h = generated_include + '/matchmaker/generated_letters/' + first_letter + '/' +             \
                                                                          first_letter + '.h'

        letter_cpp = generated_src + first_letter + '.cpp'

        # single letter parent
        shutil.copy(parent_h, letter_h)
        shutil.copy(parent_cpp, letter_cpp)
        for filename in [letter_h, letter_cpp]:
            for line in fileinput.input(filename, inplace=True):
                replaced = line.replace("aoeu", first_letter)
                replaced = replaced.replace("snth", first_letter)
                print(replaced, end='')


        for second_letter in ascii_lowercase:
            if not parents_only:
                os.makedirs(generated_include + '/matchmaker/generated_letters/' + first_letter + '/'      \
                                                                                 + second_letter)

            letter_letter_h = generated_include + '/matchmaker/generated_letters/' + first_letter + '/'    \
                    + second_letter + '/' + first_letter + '_' + second_letter + '.h'

            letter_letter_cpp = generated_src + first_letter + '_' + second_letter + '.cpp'

            if (q and first_letter == 'q' and second_letter == 'u') or                                     \
                    (first_letter == 'p' and second_letter == 'r') or                                      \
                    (first_letter == 'n' and second_letter == 'o') or                                      \
                    (first_letter == 'u' and second_letter == 'n') or                                      \
                    (first_letter == 'c' and second_letter == 'o') or                                      \
                    (first_letter == 'i' and second_letter == 'n') or                                      \
                    (first_letter == 'c' and second_letter == 'a') or                                      \
                    (first_letter == 's' and second_letter == 'u') or                                      \
                    (first_letter == 'd' and second_letter == 'i') or                                      \
                    (first_letter == 'd' and second_letter == 'e') or                                      \
                    (first_letter == 'p' and second_letter == 'a') or                                      \
                    (first_letter == 's' and second_letter == 't') or                                      \
                    (first_letter == 't' and second_letter == 'r') or                                      \
                    (first_letter == 'r' and second_letter == 'e') or                                      \
                    (first_letter == 'a' and second_letter == 'n') or                                      \
                    (first_letter == 'c' and second_letter == 'h') or                                      \
                    (first_letter == 'm' and second_letter == 'a') or                                      \
                    (first_letter == 's' and second_letter == 'e') or                                      \
                    (first_letter == 'o' and second_letter == 'v') or                                      \
                    (first_letter == 'm' and second_letter == 'i') or                                      \
                    (first_letter == 'p' and second_letter == 'e') or                                      \
                    (first_letter == 'p' and second_letter == 'o') or                                      \
                    (first_letter == 'm' and second_letter == 'e') or                                      \
                    (first_letter == 'b' and second_letter == 'a') or                                      \
                    (first_letter == 'b' and second_letter == 'e') or                                      \
                    (first_letter == 'h' and second_letter == 'e') or                                      \
                    (first_letter == 'h' and second_letter == 'a') or                                      \
                    (first_letter == 'p' and second_letter == 'h') or                                      \
                    (first_letter == 'm' and second_letter == 'o') or                                      \
                    (first_letter == 't' and second_letter == 'h') or                                      \
                    (first_letter == 'l' and second_letter == 'a') or                                      \
                    (first_letter == 'e' and second_letter == 'x') or                                      \
                    (first_letter == 's' and second_letter == 'p') or                                      \
                    (first_letter == 'c' and second_letter == 'r') or                                      \
                    (first_letter == 'h' and second_letter == 'o') or                                      \
                    (first_letter == 'f' and second_letter == 'o') or                                      \
                    (first_letter == 's' and second_letter == 'c') or                                      \
                    (first_letter == 'b' and second_letter == 'r') or                                      \
                    (first_letter == 'e' and second_letter == 'n') or                                      \
                    (first_letter == 't' and second_letter == 'e') or                                      \
                    (first_letter == 't' and second_letter == 'a') or                                      \
                    (first_letter == 's' and second_letter == 'a') or                                      \
                    (first_letter == 'h' and second_letter == 'y') or                                      \
                    (first_letter == 'c' and second_letter == 'l') or                                      \
                    (first_letter == 'a' and second_letter == 'r') or                                      \
                    (first_letter == 'a' and second_letter == 'l') or                                      \
                    (first_letter == 'l' and second_letter == 'i') or                                      \
                    (first_letter == 'g' and second_letter == 'a') or                                      \
                    (first_letter == 's' and second_letter == 'o') or                                      \
                    (first_letter == 'r' and second_letter == 'a') or                                      \
                    (first_letter == 'g' and second_letter == 'r') or                                      \
                    (first_letter == 'b' and second_letter == 'i') or                                      \
                    (first_letter == 'p' and second_letter == 'l') or                                      \
                    (first_letter == 'a' and second_letter == 'c') or                                      \
                    (first_letter == 't' and second_letter == 'o') or                                      \
                    (first_letter == 'p' and second_letter == 'i') or                                      \
                    (first_letter == 'o' and second_letter == 'u') or                                      \
                    (first_letter == 'f' and second_letter == 'l') or                                      \
                    (first_letter == 's' and second_letter == 'i') or                                      \
                    (first_letter == 'b' and second_letter == 'u') or                                      \
                    (first_letter == 'm' and second_letter == 'u') or                                      \
                    (first_letter == 's' and second_letter == 'h') or                                      \
                    (first_letter == 'd' and second_letter == 'o') or                                      \
                    (first_letter == 'b' and second_letter == 'o') or                                      \
                    (first_letter == 'n' and second_letter == 'e') or                                      \
                    (first_letter == 'i' and second_letter == 'm') or                                      \
                    (first_letter == 'l' and second_letter == 'e') or                                      \
                    (first_letter == 'f' and second_letter == 'a') or                                      \
                    (first_letter == 'f' and second_letter == 'i') or                                      \
                    (first_letter == 'w' and second_letter == 'a') or                                      \
                    (first_letter == 'a' and second_letter == 'm') or                                      \
                    (first_letter == 'v' and second_letter == 'i') or                                      \
                    (first_letter == 'v' and second_letter == 'e') or                                      \
                    (first_letter == 'l' and second_letter == 'o') or                                      \
                    (first_letter == 'f' and second_letter == 'r') or                                      \
                    (first_letter == 'r' and second_letter == 'o') or                                      \
                    (first_letter == 'b' and second_letter == 'l') or                                      \
                    (first_letter == 'p' and second_letter == 'u') or                                      \
                    (first_letter == 'a' and second_letter == 'p') or                                      \
                    (first_letter == 'c' and second_letter == 'u') or                                      \
                    (first_letter == 'a' and second_letter == 's') or                                      \
                    (first_letter == 'c' and second_letter == 'e') or                                      \
                    (first_letter == 'p' and second_letter == 's') or                                      \
                    (first_letter == 'g' and second_letter == 'e') or                                      \
                    (first_letter == 'f' and second_letter == 'e'):

                # two letter parent
                shutil.copy(parent_h, letter_letter_h)
                shutil.copy(parent_cpp, letter_letter_cpp)
                for filename in [letter_letter_h, letter_letter_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", first_letter + '/' + second_letter)
                        replaced = replaced.replace("snth", first_letter + '_' + second_letter)
                        print(replaced, end='')


                for third_letter in ascii_lowercase:
                    if not parents_only:
                        os.makedirs(generated_include + '/matchmaker/generated_letters/'                   \
                                + first_letter + '/' + second_letter + '/' + third_letter)

                    letter_letter_letter_h = generated_include + '/matchmaker/generated_letters/'          \
                            + first_letter + '/' + second_letter + '/' + third_letter + '/'                \
                            + first_letter + '_' + second_letter + '_' + third_letter + '.h'

                    letter_letter_letter_cpp = generated_src + first_letter + '_'                          \
                            + second_letter + '_' + third_letter + '.cpp'

                    if (first_letter == 'p' and second_letter == 'r' and third_letter == 'e') or           \
                            (first_letter == 'd' and second_letter == 'i' and third_letter == 's') or      \
                            (first_letter == 'c' and second_letter == 'o' and third_letter == 'n') or      \
                            (first_letter == 'n' and second_letter == 'o' and third_letter == 'n') or      \
                            (first_letter == 'u' and second_letter == 'n' and third_letter == 'd') or      \
                            (first_letter == 'p' and second_letter == 'r' and third_letter == 'o') or      \
                            (first_letter == 'u' and second_letter == 'n' and third_letter == 's') or      \
                            (first_letter == 's' and second_letter == 'u' and third_letter == 'b') or      \
                            (first_letter == 'i' and second_letter == 'n' and third_letter == 't') or      \
                            (first_letter == 's' and second_letter == 'u' and third_letter == 'p') or      \
                            (first_letter == 'u' and second_letter == 'n' and third_letter == 'c') or      \
                            (first_letter == 'p' and second_letter == 'a' and third_letter == 'r') or      \
                            (first_letter == 't' and second_letter == 'r' and third_letter == 'a') or      \
                            (first_letter == 'u' and second_letter == 'n' and third_letter == 'p') or      \
                            (first_letter == 't' and second_letter == 'r' and third_letter == 'i') or      \
                            (first_letter == 'c' and second_letter == 'o' and third_letter == 'm') or      \
                            (first_letter == 'c' and second_letter == 'a' and third_letter == 'r') or      \
                            (first_letter == 'r' and second_letter == 'e' and third_letter == 'c'):

                        # three letter parent
                        shutil.copy(parent_h, letter_letter_letter_h)
                        shutil.copy(parent_cpp, letter_letter_letter_cpp)
                        for filename in [letter_letter_letter_h, letter_letter_letter_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", first_letter + '/' + second_letter + '/'   \
                                        + third_letter)
                                replaced = replaced.replace("snth", first_letter + '_' + second_letter     \
                                        + '_' + third_letter)
                                print(replaced, end='')

                        if parents_only:
                            continue

                        for fourth_letter in ascii_lowercase:
                            os.makedirs(generated_include + '/matchmaker/generated_letters/'               \
                                        + first_letter + '/' + second_letter + '/' + third_letter + '/'    \
                                        + fourth_letter)

                            letter_letter_letter_letter_h = generated_include                              \
                                    + '/matchmaker/generated_letters/' + first_letter + '/'                \
                                    + second_letter + '/' + third_letter + '/' + fourth_letter + '/'       \
                                    + first_letter + '_' + second_letter + '_' + third_letter + '_'        \
                                    + fourth_letter + '.h'

                            letter_letter_letter_letter_cpp = generated_src + first_letter + '_'           \
                                    + second_letter + '_' + third_letter + '_' + fourth_letter + '.cpp'

                            # four letter leaf
                            shutil.copy(leaf_h, letter_letter_letter_letter_h)
                            shutil.copy(leaf_cpp, letter_letter_letter_letter_cpp)
                            for filename in [letter_letter_letter_letter_h,
                                             letter_letter_letter_letter_cpp]:
                                for line in fileinput.input(filename, inplace=True):
                                    replaced = line.replace("aoeu", first_letter + '/' + second_letter     \
                                            + '/' + third_letter + '/' + fourth_letter)
                                    replaced = replaced.replace("snth", first_letter + '_' + second_letter \
                                            + '_' + third_letter + '_' + fourth_letter)
                                    print(replaced, end='')

                    elif not parents_only:

                        # three letter leaf
                        shutil.copy(leaf_h, letter_letter_letter_h)
                        shutil.copy(leaf_cpp, letter_letter_letter_cpp)
                        for filename in [letter_letter_letter_h, letter_letter_letter_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", first_letter + '/' + second_letter + '/'   \
                                        + third_letter)
                                replaced = replaced.replace("snth", first_letter + '_' + second_letter     \
                                        + '_' + third_letter)
                                print(replaced, end='')

            elif not parents_only:

                # two letter leaf
                shutil.copy(leaf_h, letter_letter_h)
                shutil.copy(leaf_cpp, letter_letter_cpp)
                for filename in [letter_letter_h, letter_letter_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", first_letter + '/' + second_letter)
                        replaced = replaced.replace("snth", first_letter + '_' + second_letter)
                        print(replaced, end='')


def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hqap', ['help', 'q', 'atomic_libs', 'parents_only'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    q = False
    atomic_libs = False
    parents_only = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-a', '--atomic_libs'):
            atomic_libs = True
        elif o in ('-p', '--parents_only'):
            parents_only = True
        else:
            assert False, "unhandled option"


    repo_root = os.path.dirname(os.path.realpath(__file__)) + '/../'
    prepare_letters(repo_root, q, atomic_libs, parents_only)
    exit(0)



if __name__ == "__main__":
    main()
