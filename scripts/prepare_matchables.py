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
    print('    -w, --workspace_dir              workspace directory')
    print('    -d, --data_reader_loc            data_reader installation prefix')
    print('    -q, --q                          q only')



def prepare_matchables(workspace_dir, reader_loc, q):

    out_dir = workspace_dir + '/generated_include/matchmaker/generated_matchables/'

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

        for l1 in ascii_uppercase + ascii_lowercase:
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
                    (l0 == 'w' and l1 == 'o') or                                                           \
                    (l0 == 'C' and l1 == 'a') or                                                           \
                    (l0 == 'h' and l1 == 'i') or                                                           \
                    (l0 == 'C' and l1 == 'h') or                                                           \
                    (l0 == 'C' and l1 == 'o') or                                                           \
                    (l0 == 's' and l1 == 'w') or                                                           \
                    (l0 == 't' and l1 == 'w') or                                                           \
                    (l0 == 'r' and l1 == 'u') or                                                           \
                    (l0 == 'g' and l1 == 'i') or                                                           \
                    (l0 == 'S' and l1 == 'a') or                                                           \
                    (l0 == 'B' and l1 == 'a') or                                                           \
                    (l0 == 'a' and l1 == 't') or                                                           \
                    (l0 == 'P' and l1 == 'a') or                                                           \
                    (l0 == 'd' and l1 == 'u') or                                                           \
                    (l0 == 'L' and l1 == 'a') or                                                           \
                    (l0 == 'n' and l1 == 'i') or                                                           \
                    (l0 == 'A' and l1 == 'n') or                                                           \
                    (l0 == 's' and l1 == 'n') or                                                           \
                    (l0 == 'B' and l1 == 'e') or                                                           \
                    (l0 == 'H' and l1 == 'a') or                                                           \
                    (l0 == 'c' and l1 == 'y') or                                                           \
                    (l0 == 's' and l1 == 'm') or                                                           \
                    (l0 == 'j' and l1 == 'u') or                                                           \
                    (l0 == 'l' and l1 == 'u') or                                                           \
                    (l0 == 'R' and l1 == 'o') or                                                           \
                    (l0 == 'o' and l1 == 'p') or                                                           \
                    (l0 == 'a' and l1 == 'g') or                                                           \
                    (l0 == 'k' and l1 == 'i') or                                                           \
                    (l0 == 'M' and l1 == 'i') or                                                           \
                    (l0 == 'P' and l1 == 'e') or                                                           \
                    (l0 == 'w' and l1 == 'e') or                                                           \
                    (l0 == 'M' and l1 == 'o') or                                                           \
                    (l0 == 'h' and l1 == 'u') or                                                           \
                    (l0 == 'o' and l1 == 'n') or                                                           \
                    (l0 == 'B' and l1 == 'r') or                                                           \
                    (l0 == 'A' and l1 == 'r') or                                                           \
                    (l0 == 'G' and l1 == 'r') or                                                           \
                    (l0 == 'P' and l1 == 'r') or                                                           \
                    (l0 == 'A' and l1 == 'l') or                                                           \
                    (l0 == 's' and l1 == 'k') or                                                           \
                    (l0 == 'H' and l1 == 'e') or                                                           \
                    (l0 == 'S' and l1 == 't') or                                                           \
                    (l0 == 'L' and l1 == 'e') or                                                           \
                    (l0 == 'D' and l1 == 'e') or                                                           \
                    (l0 == 'i' and l1 == 's') or                                                           \
                    (l0 == 'v' and l1 == 'o') or                                                           \
                    (l0 == 'k' and l1 == 'e') or                                                           \
                    (l0 == 'M' and l1 == 'e') or                                                           \
                    (l0 == 'j' and l1 == 'a') or                                                           \
                    (l0 == 'u' and l1 == 'p') or                                                           \
                    (l0 == 'B' and l1 == 'o') or                                                           \
                    (l0 == 'r' and l1 == 'i'):

                for l2 in ascii_uppercase + ascii_lowercase:
                    os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2)

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
                            (l0 == 'D' and l1 == 'e' and l2 == 'c') or                                     \
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
                            (l0 == 'c' and l1 == 'o' and l2 == 'u'):

                        for l3 in ascii_uppercase + ascii_lowercase:
                            os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3)

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
                                    (l0 == 'n' and l1 == 'o' and l2 == 'n' and l3 == 'c'):

                                for l4 in ascii_uppercase + ascii_lowercase:
                                    os.makedirs(out_dir + l0 + '/' + l1 + '/' + l2 + '/' + l3 + '/' + l4)

                                    if (l0 == 'i' and l1 == 'n' and l2 == 't' and l3 == 'e' and            \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 's' and l1 == 'u' and l2 == 'p' and l3 == 'e' and       \
                                                    l4 == 'r') or                                          \
                                                                                                           \
                                            (l0 == 'q' and l1 == 'u' and l2 == 'a' and l3 == 's' and       \
                                                    l4 == 'i') or                                          \
                                                                                                           \
                                            (l0 == 'u' and l1 == 'n' and l2 == 'd' and l3 == 'e' and       \
                                                    l4 == 'r'):

                                        for l5 in ascii_uppercase + ascii_lowercase:
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

    error_filename = '/tmp/prepare_matchables_failed'
    def prepare(cmd):
        if subprocess.run(cmd).returncode != 0:
            with open(error_filename, 'w') as f:
                pass

    if os.path.exists(error_filename):
        os.remove(error_filename)

    with ThreadPoolExecutor(multiprocessing.cpu_count()) as executor:
        executor.map(prepare, cmds)

    os.chdir(start_dir)

    if os.path.exists(error_filename):
        os.remove(error_filename)
        exit(1)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hw:d:q', ['help', 'data_reader_loc', 'q'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    workspace_dir = ''
    reader_loc = ''
    q = False

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-w', '--workspace_dir'):
            workspace_dir = a
        elif o in ('-d', '--data_reader_loc'):
            reader_loc = a
        elif o in ('-q', '--q'):
            q = True
        else:
            assert False, "unhandled option"

    if workspace_dir == '':
        usage()
        sys.exit(2)

    if reader_loc == '':
        usage()
        sys.exit(2)

    matchmaker_root = os.path.dirname(os.path.realpath(__file__)) + '/../'

    if workspace_dir[0] != '/':
        workspace_dir = matchmaker_root + workspace_dir

    if reader_loc[0] != '/':
        reader_loc = matchmaker_root + reader_loc

    prepare_matchables(workspace_dir, reader_loc, q)
    exit(0)



if __name__ == "__main__":
    main()
