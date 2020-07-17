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

    for l0 in ascii_uppercase + ascii_lowercase:

        if q:
            if l0 != "q" and l0 != "Q":
                continue

        if not parents_only:
            os.makedirs(generated_include + '/matchmaker/generated_letters/' + l0)

        letter_x1_h = generated_include + '/matchmaker/generated_letters/' + l0 + '/' + l0 + '.h'

        letter_x1_cpp = generated_src + l0 + '.cpp'

        # single letter parent
        shutil.copy(parent_h, letter_x1_h)
        shutil.copy(parent_cpp, letter_x1_cpp)
        for filename in [letter_x1_h, letter_x1_cpp]:
            for line in fileinput.input(filename, inplace=True):
                replaced = line.replace("aoeu", l0)
                replaced = replaced.replace("snth", l0)
                print(replaced, end='')


        for l1 in ascii_lowercase:
            if not parents_only:
                os.makedirs(generated_include + '/matchmaker/generated_letters/' + l0 + '/' + l1)

            letter_x2_h = generated_include + '/matchmaker/generated_letters/'                             \
                    + l0 + '/' + l1 + '/' + l0 + '_' + l1 + '.h'

            letter_x2_cpp = generated_src + l0 + '_' + l1 + '.cpp'

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

                # two letter parent
                shutil.copy(parent_h, letter_x2_h)
                shutil.copy(parent_cpp, letter_x2_cpp)
                for filename in [letter_x2_h, letter_x2_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", l0 + '/' + l1)
                        replaced = replaced.replace("snth", l0 + '_' + l1)
                        print(replaced, end='')


                for l2 in ascii_lowercase:
                    if not parents_only:
                        os.makedirs(generated_include + '/matchmaker/generated_letters/'                   \
                                + l0 + '/' + l1 + '/' + l2)

                    letter_x3_h = generated_include + '/matchmaker/generated_letters/'                     \
                            + l0 + '/' + l1 + '/' + l2 + '/' + l0 + '_' + l1 + '_' + l2 + '.h'

                    letter_x3_cpp = generated_src + l0 + '_' + l1 + '_' + l2 + '.cpp'

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

                        # three letter parent
                        shutil.copy(parent_h, letter_x3_h)
                        shutil.copy(parent_cpp, letter_x3_cpp)
                        for filename in [letter_x3_h, letter_x3_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2)
                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2)
                                print(replaced, end='')


                        for l3 in ascii_lowercase:
                            if not parents_only:
                                os.makedirs(generated_include + '/matchmaker/generated_letters/'           \
                                        + l0 + '/' + l1 + '/' + l2 + '/' + l3)

                            letter_x4_h = generated_include + '/matchmaker/generated_letters/'             \
                                    + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/'                            \
                                    + l0 + '_' + l1 + '_' + l2 + '_' + l3 + '.h'

                            letter_x4_cpp = generated_src + l0 + '_' + l1 + '_' + l2 + '_' + l3 + '.cpp'

                            if (l0 == 'o' and l1 == 'v' and l2 == 'e' and l3 == 'r') or                    \
                                    (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e') or               \
                                    (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e') or               \
                                    (l0 == 'a' and l1 == 'n' and l2 == 't' and l3 == 'i') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'c' and l3 == 'o') or               \
                                    (l0 == 's' and l1 == 'e' and l2 == 'm' and l3 == 'i') or               \
                                    (l0 == 'n' and l1 == 'o' and l2 == 'n' and l3 == 'c'):

                                # four letter parent
                                shutil.copy(parent_h, letter_x4_h)
                                shutil.copy(parent_cpp, letter_x4_cpp)
                                for filename in [letter_x4_h, letter_x4_cpp]:
                                    for line in fileinput.input(filename, inplace=True):
                                        replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2 + '/' + l3)
                                        replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2 + '_' \
                                                + l3)
                                        print(replaced, end='')

                                for l4 in ascii_lowercase:
                                    if not parents_only:
                                        os.makedirs(generated_include + '/matchmaker/generated_letters/'   \
                                                + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4)

                                    letter_x5_h = generated_include + '/matchmaker/generated_letters/'     \
                                            + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4 + '/'         \
                                            + l0 + '_' + l1 + '_' + l2 + '_' + l3 + '_' + l4 + '.h'

                                    letter_x5_cpp = generated_src + l0 + '_' + l1 + '_' + l2 + '_' + l3    \
                                            + '_' + l4 + '.cpp'

                                    if (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e' and            \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e' and       \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e' and       \
                                                    l4 == 'r'):

                                        # five letter parent
                                        shutil.copy(parent_h, letter_x5_h)
                                        shutil.copy(parent_cpp, letter_x5_cpp)
                                        for filename in [letter_x5_h, letter_x5_cpp]:
                                            for line in fileinput.input(filename, inplace=True):
                                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2   \
                                                        + '/' + l3 + '/' + l4)
                                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_'    \
                                                        + l2 + '_' + l3 + '_' + l4)
                                                print(replaced, end='')

                                        if parents_only:
                                            continue

                                        for l5 in ascii_lowercase:
                                            os.makedirs(generated_include                                  \
                                                    + '/matchmaker/generated_letters/' + l0 + '/' + l1     \
                                                    + '/' + l2 + '/' + l3 + '/' + l4 + '/' + l5)

                                            letter_x6_h = generated_include                                \
                                                    + '/matchmaker/generated_letters/' + l0 + '/' + l1     \
                                                    + '/' + l2 + '/' + l3 + '/' + l4 + '/' + l5 + '/'      \
                                                    + l0 + '_' + l1 + '_' + l2 + '_' + l3 + '_' + l4 + '_' \
                                                    + l5 + '.h'

                                            letter_x6_cpp = generated_src + l0 + '_' + l1 + '_' + l2 + '_' \
                                                    + l3 + '_' + l4 + '_' + l5 + '.cpp'

                                            # six letter leaf
                                            shutil.copy(leaf_h, letter_x6_h)
                                            shutil.copy(leaf_cpp, letter_x6_cpp)
                                            for filename in [letter_x6_h, letter_x6_cpp]:
                                                for line in fileinput.input(filename, inplace=True):
                                                    replaced = line.replace("aoeu", l0 + '/' + l1 + '/'    \
                                                            + l2 + '/' + l3 + '/' + l4 + '/' + l5)
                                                    replaced = replaced.replace("snth", l0 + '_' + l1      \
                                                            + '_' + l2 + '_' + l3 + '_' + l4 + '_' + l5)
                                                    print(replaced, end='')

                                    elif not parents_only:

                                        # five letter leaf
                                        shutil.copy(leaf_h, letter_x5_h)
                                        shutil.copy(leaf_cpp, letter_x5_cpp)
                                        for filename in [letter_x5_h, letter_x5_cpp]:
                                            for line in fileinput.input(filename, inplace=True):
                                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2   \
                                                        + '/' + l3 + '/' + l4)
                                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_'    \
                                                        + l2 + '_' + l3 + '_' + l4)
                                                print(replaced, end='')

                            elif not parents_only:

                                # four letter leaf
                                shutil.copy(leaf_h, letter_x4_h)
                                shutil.copy(leaf_cpp, letter_x4_cpp)
                                for filename in [letter_x4_h, letter_x4_cpp]:
                                    for line in fileinput.input(filename, inplace=True):
                                        replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2 + '/' + l3)
                                        replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2 + '_' \
                                                + l3)
                                        print(replaced, end='')

                    elif not parents_only:

                        # three letter leaf
                        shutil.copy(leaf_h, letter_x3_h)
                        shutil.copy(leaf_cpp, letter_x3_cpp)
                        for filename in [letter_x3_h, letter_x3_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2)
                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2)
                                print(replaced, end='')

            elif not parents_only:

                # two letter leaf
                shutil.copy(leaf_h, letter_x2_h)
                shutil.copy(leaf_cpp, letter_x2_cpp)
                for filename in [letter_x2_h, letter_x2_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", l0 + '/' + l1)
                        replaced = replaced.replace("snth", l0 + '_' + l1)
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
