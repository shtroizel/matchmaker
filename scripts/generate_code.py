#!/usr/bin/env python3

import os
import shutil
import fileinput
import getopt
import sys

from string import ascii_lowercase
from string import ascii_uppercase
from pathlib import Path



def usage():
    print(sys.argv[0] + ' [OPTION]')
    print('    -h, --help                  print this message')
    print('    -w, --workspace_root        workspace directory')
    print('    -q, --q                     q only')
    print('    -p, --parents_only          update parents only')



def prepare_letters(workspace_root, q, parents_only):

    generated_include = workspace_root / 'generated_include'
    generated_src = workspace_root / 'generated_src'
    generated_symbols = generated_include / 'matchmaker' / 'generated_symbols'
    generated_matchables = generated_include / 'matchmaker' / 'generated_matchables'

    if not parents_only:
        # reset header location
        shutil.rmtree(generated_symbols, ignore_errors=True)
        os.makedirs(generated_symbols)

        # reset impl location
        shutil.rmtree(generated_src, ignore_errors=True)
        os.makedirs(generated_src)

        # reset matchables location
        shutil.rmtree(generated_matchables, ignore_errors=True)
        os.makedirs(generated_matchables)

        # reset prefix file
        prefix_filename = workspace_root / 'prefix_list'
        prefix_file = open(prefix_filename, "w")

    matchmaker_root = Path(os.path.dirname(os.path.realpath(__file__))).parent

    leaf_h = matchmaker_root / 'templates' / 'internal_interface.h'
    leaf_cpp = matchmaker_root / 'templates' / 'leaf.cpp'
    parent_h = matchmaker_root / 'templates' / 'internal_interface.h'
    parent_cpp = matchmaker_root / 'templates' / 'parent.cpp'

    progress = 'generating matchmaker code that is now finally ready!';
    progress_index = 0

    prefixes = []
    for ch in ascii_uppercase:
        prefixes.append('esc_' + ch)
    for ch in ascii_lowercase:
        prefixes.append(ch)

    for l0 in prefixes:

        # update progress
        print(progress[progress_index], end='')
        progress_index += 1
        sys.stdout.flush()

        if not parents_only:
            os.makedirs(generated_symbols / l0)
            os.makedirs(generated_matchables / l0)

        letter_x1_h = generated_symbols / l0 / (l0 + '.h')
        letter_x1_cpp = generated_src / (l0 + '.cpp')

        # single letter parent
        shutil.copy(parent_h, letter_x1_h)
        shutil.copy(parent_cpp, letter_x1_cpp)
        for filename in [letter_x1_h, letter_x1_cpp]:
            for line in fileinput.input(filename, inplace=True):
                replaced = line.replace("aoeu", l0)
                replaced = replaced.replace("snth", l0)
                print(replaced, end='')


        for l1 in prefixes:
            if not parents_only:
                os.makedirs(generated_symbols / l0 / l1)
                os.makedirs(generated_matchables / l0 / l1)

            letter_x2_h = generated_symbols / l0 / l1 / (l0 + '_' + l1 + '.h')
            letter_x2_cpp = generated_src / (l0 + '_' + l1 + '.cpp')

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
                    (l0 == 'w' and l1 == 'o') or                                                           \
                    (l0 == 'esc_C' and l1 == 'a') or                                                       \
                    (l0 == 'h' and l1 == 'i') or                                                           \
                    (l0 == 'esc_C' and l1 == 'h') or                                                       \
                    (l0 == 'esc_C' and l1 == 'o') or                                                       \
                    (l0 == 's' and l1 == 'w') or                                                           \
                    (l0 == 't' and l1 == 'w') or                                                           \
                    (l0 == 'r' and l1 == 'u') or                                                           \
                    (l0 == 'g' and l1 == 'i') or                                                           \
                    (l0 == 'esc_S' and l1 == 'a') or                                                       \
                    (l0 == 'esc_B' and l1 == 'a') or                                                       \
                    (l0 == 'a' and l1 == 't') or                                                           \
                    (l0 == 'esc_P' and l1 == 'a') or                                                       \
                    (l0 == 'd' and l1 == 'u') or                                                           \
                    (l0 == 'esc_L' and l1 == 'a') or                                                       \
                    (l0 == 'n' and l1 == 'i') or                                                           \
                    (l0 == 'esc_A' and l1 == 'n') or                                                       \
                    (l0 == 's' and l1 == 'n') or                                                           \
                    (l0 == 'esc_B' and l1 == 'e') or                                                       \
                    (l0 == 'esc_H' and l1 == 'a') or                                                       \
                    (l0 == 'c' and l1 == 'y') or                                                           \
                    (l0 == 's' and l1 == 'm') or                                                           \
                    (l0 == 'j' and l1 == 'u') or                                                           \
                    (l0 == 'l' and l1 == 'u') or                                                           \
                    (l0 == 'esc_R' and l1 == 'o') or                                                       \
                    (l0 == 'o' and l1 == 'p') or                                                           \
                    (l0 == 'a' and l1 == 'g') or                                                           \
                    (l0 == 'k' and l1 == 'i') or                                                           \
                    (l0 == 'esc_M' and l1 == 'i') or                                                       \
                    (l0 == 'esc_P' and l1 == 'e') or                                                       \
                    (l0 == 'w' and l1 == 'e') or                                                           \
                    (l0 == 'esc_M' and l1 == 'o') or                                                       \
                    (l0 == 'h' and l1 == 'u') or                                                           \
                    (l0 == 'o' and l1 == 'n') or                                                           \
                    (l0 == 'esc_B' and l1 == 'r') or                                                       \
                    (l0 == 'esc_A' and l1 == 'r') or                                                       \
                    (l0 == 'esc_G' and l1 == 'r') or                                                       \
                    (l0 == 'esc_P' and l1 == 'r') or                                                       \
                    (l0 == 'esc_A' and l1 == 'l') or                                                       \
                    (l0 == 's' and l1 == 'k') or                                                           \
                    (l0 == 'esc_H' and l1 == 'e') or                                                       \
                    (l0 == 'esc_S' and l1 == 't') or                                                       \
                    (l0 == 'esc_L' and l1 == 'e') or                                                       \
                    (l0 == 'esc_D' and l1 == 'e') or                                                       \
                    (l0 == 'i' and l1 == 's') or                                                           \
                    (l0 == 'v' and l1 == 'o') or                                                           \
                    (l0 == 'k' and l1 == 'e') or                                                           \
                    (l0 == 'esc_M' and l1 == 'e') or                                                       \
                    (l0 == 'j' and l1 == 'a') or                                                           \
                    (l0 == 'u' and l1 == 'p') or                                                           \
                    (l0 == 'esc_B' and l1 == 'o') or                                                       \
                    (l0 == 'o' and l1 == 'b') or                                                           \
                    (l0 == 'm' and l1 == 'y') or                                                           \
                    (l0 == 'esc_T' and l1 == 'h') or                                                       \
                    (l0 == 'esc_W' and l1 == 'h') or                                                       \
                    (l0 == 'r' and l1 == 'i'):

                # two letter parent
                shutil.copy(parent_h, letter_x2_h)
                shutil.copy(parent_cpp, letter_x2_cpp)
                for filename in [letter_x2_h, letter_x2_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", l0 + '/' + l1)
                        replaced = replaced.replace("snth", l0 + '_' + l1)
                        print(replaced, end='')


                for l2 in prefixes:
                    if not parents_only:
                        os.makedirs(generated_symbols / l0 / l1 / l2)
                        os.makedirs(generated_matchables / l0 / l1 / l2)

                    letter_x3_h = generated_symbols / l0 / l1 / l2 / (l0 + '_' + l1 + '_' + l2 + '.h')
                    letter_x3_cpp = generated_src / (l0 + '_' + l1 + '_' + l2 + '.cpp')

                    if (l0 == 'p' and l1 == 'r' and l2 == 'e') or                                          \
                            (l0 == 'q' and l1 == 'u' and l2 == 'a') or                                     \
                            (l0 == 'q' and l1 == 'u' and l2 == 'i') or                                     \
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
                            (l0 == 's' and l1 == 'e' and l2 == 'l') or                                     \
                            (l0 == 'g' and l1 == 'r' and l2 == 'a') or                                     \
                            (l0 == 's' and l1 == 't' and l2 == 'e') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'l') or                                     \
                            (l0 == 'p' and l1 == 's' and l2 == 'e') or                                     \
                            (l0 == 's' and l1 == 't' and l2 == 'o') or                                     \
                            (l0 == 'm' and l1 == 'a' and l2 == 'n') or                                     \
                            (l0 == 's' and l1 == 'h' and l2 == 'o') or                                     \
                            (l0 == 'h' and l1 == 'e' and l2 == 'a') or                                     \
                            (l0 == 'b' and l1 == 'a' and l2 == 'r') or                                     \
                            (l0 == 'h' and l1 == 'a' and l2 == 'l') or                                     \
                            (l0 == 'a' and l1 == 'l' and l2 == 'l') or                                     \
                            (l0 == 'm' and l1 == 'a' and l2 == 'n') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 'd') or                                     \
                            (l0 == 't' and l1 == 'h' and l2 == 'e') or                                     \
                            (l0 == 'w' and l1 == 'i' and l2 == 'n') or                                     \
                            (l0 == 'm' and l1 == 'a' and l2 == 'r') or                                     \
                            (l0 == 'c' and l1 == 'a' and l2 == 't') or                                     \
                            (l0 == 'b' and l1 == 'l' and l2 == 'a') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 'p') or                                     \
                            (l0 == 'u' and l1 == 'n' and l2 == 't') or                                     \
                            (l0 == 'h' and l1 == 'a' and l2 == 'v') or                                     \
                            (l0 == 'w' and l1 == 'h' and l2 == 'i') or                                     \
                            (l0 == 'c' and l1 == 'a' and l2 == 'n') or                                     \
                            (l0 == 's' and l1 == 'p' and l2 == 'i') or                                     \
                            (l0 == 's' and l1 == 'h' and l2 == 'a') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 't') or                                     \
                            (l0 == 'm' and l1 == 'i' and l2 == 'c') or                                     \
                            (l0 == 'w' and l1 == 'o' and l2 == 'r') or                                     \
                            (l0 == 'p' and l1 == 'o' and l2 == 's') or                                     \
                            (l0 == 't' and l1 == 'h' and l2 == 'r') or                                     \
                            (l0 == 'c' and l1 == 'h' and l2 == 'e') or                                     \
                            (l0 == 'f' and l1 == 'l' and l2 == 'a') or                                     \
                            (l0 == 'p' and l1 == 'h' and l2 == 'o') or                                     \
                            (l0 == 'i' and l1 == 'n' and l2 == 's') or                                     \
                            (l0 == 's' and l1 == 'c' and l2 == 'a') or                                     \
                            (l0 == 'd' and l1 == 'i' and l2 == 'a') or                                     \
                            (l0 == 'm' and l1 == 'a' and l2 == 'k') or                                     \
                            (l0 == 's' and l1 == 'a' and l2 == 'l') or                                     \
                            (l0 == 'i' and l1 == 'n' and l2 == 'd') or                                     \
                            (l0 == 'e' and l1 == 'l' and l2 == 'e') or                                     \
                            (l0 == 'c' and l1 == 'a' and l2 == 'l') or                                     \
                            (l0 == 't' and l1 == 'w' and l2 == 'i') or                                     \
                            (l0 == 'e' and l1 == 'p' and l2 == 'i') or                                     \
                            (l0 == 'b' and l1 == 'r' and l2 == 'i') or                                     \
                            (l0 == 'b' and l1 == 'a' and l2 == 'c') or                                     \
                            (l0 == 'b' and l1 == 'r' and l2 == 'a') or                                     \
                            (l0 == 'b' and l1 == 'r' and l2 == 'o') or                                     \
                            (l0 == 'n' and l1 == 'o' and l2 == 't') or                                     \
                            (l0 == 't' and l1 == 'h' and l2 == 'i') or                                     \
                            (l0 == 'h' and l1 == 'a' and l2 == 'r') or                                     \
                            (l0 == 'esc_D' and l1 == 'e' and l2 == 'c') or                                 \
                            (l0 == 'b' and l1 == 'e' and l2 == 'a') or                                     \
                            (l0 == 'p' and l1 == 'a' and l2 == 'n') or                                     \
                            (l0 == 'c' and l1 == 'h' and l2 == 'i') or                                     \
                            (l0 == 's' and l1 == 'p' and l2 == 'e') or                                     \
                            (l0 == 'r' and l1 == 'e' and l2 == 'a') or                                     \
                            (l0 == 'p' and l1 == 'r' and l2 == 'i') or                                     \
                            (l0 == 's' and l1 == 'e' and l2 == 'a') or                                     \
                            (l0 == 'c' and l1 == 'l' and l2 == 'o') or                                     \
                            (l0 == 'c' and l1 == 'r' and l2 == 'o') or                                     \
                            (l0 == 'p' and l1 == 'a' and l2 == 'l') or                                     \
                            (l0 == 'a' and l1 == 'u' and l2 == 't') or                                     \
                            (l0 == 'c' and l1 == 'h' and l2 == 'o') or                                     \
                            (l0 == 'w' and l1 == 'e' and l2 == 'l') or                                     \
                            (l0 == 's' and l1 == 'c' and l2 == 'r') or                                     \
                            (l0 == 'c' and l1 == 'l' and l2 == 'a') or                                     \
                            (l0 == 'c' and l1 == 'o' and l2 == 'u'):

                        # three letter parent
                        shutil.copy(parent_h, letter_x3_h)
                        shutil.copy(parent_cpp, letter_x3_cpp)
                        for filename in [letter_x3_h, letter_x3_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2)
                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2)
                                print(replaced, end='')


                        for l3 in prefixes:
                            if not parents_only:
                                os.makedirs(generated_symbols / l0 / l1 / l2 / l3)
                                os.makedirs(generated_matchables / l0 / l1 / l2 / l3)

                            letter_x4_h = generated_symbols / l0 / l1 / l2 / l3 /                          \
                                    (l0 + '_' + l1 + '_' + l2 + '_' + l3 + '.h')

                            letter_x4_cpp = generated_src / (l0 + '_' + l1 + '_' + l2 + '_' + l3 + '.cpp')

                            if (l0 == 'o' and l1 == 'v' and l2 == 'e' and l3 == 'r') or                    \
                                    (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e') or               \
                                    (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e') or               \
                                    (l0 == 'a' and l1 == 'n' and l2 == 't' and l3 == 'i') or               \
                                    (l0 == 'u' and l1 == 'n' and l2 == 'c' and l3 == 'o') or               \
                                    (l0 == 's' and l1 == 'e' and l2 == 'm' and l3 == 'i') or               \
                                    (l0 == 'q' and l1 == 'u' and l2 == 'a' and l3 == 's') or               \
                                    (l0 == 'c' and l1 == 'o' and l2 == 'm' and l3 == 'p') or               \
                                    (l0 == 'c' and l1 == 'o' and l2 == 'n' and l3 == 't') or               \
                                    (l0 == 'p' and l1 == 'a' and l2 == 'r' and l3 == 'a') or               \
                                    (l0 == 'p' and l1 == 'r' and l2 == 'e' and l3 == 's') or               \
                                    (l0 == 'h' and l1 == 'a' and l2 == 'l' and l3 == 'f') or               \
                                    (l0 == 'c' and l1 == 'o' and l2 == 'u' and l3 == 'n') or               \
                                    (l0 == 'p' and l1 == 'o' and l2 == 'l' and l3 == 'y') or               \
                                    (l0 == 's' and l1 == 'e' and l2 == 'l' and l3 == 'f') or               \
                                    (l0 == 't' and l1 == 'r' and l2 == 'a' and l3 == 'n') or               \
                                    (l0 == 'f' and l1 == 'o' and l2 == 'r' and l3 == 'e') or               \
                                    (l0 == 'p' and l1 == 's' and l2 == 'e' and l3 == 'u') or               \
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

                                for l4 in prefixes:
                                    if not parents_only:
                                        os.makedirs(generated_symbols / l0 / l1 / l2 / l3 / l4)
                                        os.makedirs(generated_matchables / l0 / l1 / l2 / l3 / l4)

                                    letter_x5_h = generated_symbols / l0 / l1 / l2 / l3 / l4 /             \
                                            (l0 + '_' + l1 + '_' + l2 + '_' + l3 + '_' + l4 + '.h')

                                    letter_x5_cpp = generated_src / (l0 + '_' + l1 + '_' + l2 + '_' + l3   \
                                            + '_' + l4 + '.cpp')

                                    if (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e' and            \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e' and       \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 'q' and l1 == 'u' and l2 == 'a' and l3 == 's' and       \
                                                    l4 == 'i') or                                          \
                                                                                                           \
                                            (l0 == 'c' and l1 == 'o' and l2 == 'u' and l3 == 'n' and       \
                                                    l4 == 't') or                                          \
                                                                                                           \
                                            (l0 == 't' and l1 == 'r' and l2 == 'a' and l3 == 'n' and       \
                                                    l4 == 's') or                                          \
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

                                        for l5 in prefixes:
                                            os.makedirs(generated_symbols / l0 / l1 / l2 / l3 / l4 / l5)
                                            os.makedirs(generated_matchables / l0 / l1 / l2 / l3 / l4 / l5)

                                            letter_x6_h = generated_symbols / l0 / l1 / l2 / l3 / l4 / l5 /\
                                                    (l0 + '_' + l1 + '_' + l2 + '_' + l3 + '_' + l4 + '_'  \
                                                    + l5 + '.h')

                                            letter_x6_cpp = generated_src / (l0 + '_' + l1 + '_' + l2      \
                                                    + '_' + l3 + '_' + l4 + '_' + l5 + '.cpp')











                                            # six letter leaf
                                            prefix_file.write(l0 + " " + l1 + " " + l2 + " " +
                                                              l3 + " " + l4 + " " + l5 + "\n")
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
                                        prefix_file.write(l0 + " " + l1 + " " + l2 + " " +
                                                          l3 + " " + l4 + " nil\n")
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
                                prefix_file.write(l0 + " " + l1 + " " + l2 + " " + l3 + " nil nil\n")
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
                        prefix_file.write(l0 + " " + l1 + " " + l2 + " nil nil nil\n")
                        shutil.copy(leaf_h, letter_x3_h)
                        shutil.copy(leaf_cpp, letter_x3_cpp)
                        for filename in [letter_x3_h, letter_x3_cpp]:
                            for line in fileinput.input(filename, inplace=True):
                                replaced = line.replace("aoeu", l0 + '/' + l1 + '/' + l2)
                                replaced = replaced.replace("snth", l0 + '_' + l1 + '_' + l2)
                                print(replaced, end='')

            elif not parents_only:

                # two letter leaf
                prefix_file.write(l0 + " " + l1 + " nil nil nil nil\n")
                shutil.copy(leaf_h, letter_x2_h)
                shutil.copy(leaf_cpp, letter_x2_cpp)
                for filename in [letter_x2_h, letter_x2_cpp]:
                    for line in fileinput.input(filename, inplace=True):
                        replaced = line.replace("aoeu", l0 + '/' + l1)
                        replaced = replaced.replace("snth", l0 + '_' + l1)
                        print(replaced, end='')


    if not parents_only:
        syms = [("0", "esc_0"),
                ("1", "esc_1"),
                ("2", "esc_2"),
                ("3", "esc_3"),
                ("4", "esc_4"),
                ("5", "esc_5"),
                ("6", "esc_6"),
                ("7", "esc_7"),
                ("8", "esc_8"),
                ("9", "esc_9"),
                ("-", "_mns_"),
                (".", "_dot_"),
                ("/", "_slsh_"),
                (":", "_cln_"),
                ("\"", "_quot_"),
                ("#", "_hsh_"),
                ("$", "_dol_"),
                ("'", "_sqt_"),
                ("(", "_parl_"),
                ("+", "_plus_"),
                (">", "_gt_"),
                ("]", "_sbr_"),
                ("_", "_"),
                ("~", "_tld_")]

        for sym in syms:
            os.makedirs(generated_symbols / sym[1])
            os.makedirs(generated_matchables / sym[1])
            prefix_file.write(sym[0] + " nil nil nil nil nil\n")
            sym_h = generated_symbols / sym[1] / (sym[1] + '.h')
            sym_cpp = generated_src / (sym[1] + '.cpp')
            shutil.copy(leaf_h, sym_h)
            shutil.copy(leaf_cpp, sym_cpp)
            for filename in [sym_h, sym_cpp]:
                for line in fileinput.input(filename, inplace=True):
                    replaced = line.replace("aoeu", sym[1])
                    replaced = replaced.replace("snth", sym[1])
                    print(replaced, end='')

        prefix_file.close()
        print(progress[progress_index], end='')
        progress_index += 1
        sys.stdout.flush()



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hw:qp', ['help', 'workspace_root', 'q', 'parents_only'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    workspace_root = ''
    q = False
    atomic_libs = False
    parents_only = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-w', '--workspace_root'):
            workspace_root = a
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-p', '--parents_only'):
            parents_only = True
        else:
            assert False, "unhandled option"

    if workspace_root == '':
        print("workspace_root unset, use -w to set a workspace directory")
        usage()

    workspace_root = Path(workspace_root)

    prepare_letters(workspace_root, q, parents_only)
    exit(0)



if __name__ == "__main__":
    main()
