#!/usr/bin/env python3
# order.py sort <rule> <file>     prints the file's lines sorted by the rule
# order.py check <rule> <file>    checks the file's lines are in strictly increasing order by the rule
#
# Rules, each written independently of matchable and matchmaker, so their order can be checked:
#     today   matchable's str_lt_str: at the first differing byte, a non-letter sorts before an ASCII
#             letter, otherwise bytes compare as signed char (x86)
#     bytes   unsigned bytes, std::string's order
#     folded  the Unicode case-folded spelling as unsigned bytes, then the spelling itself
# Lines are bytes; a final newline ends the last line.

import sys


def is_letter(b):
    return 65 <= b <= 90 or 97 <= b <= 122


def signed(b):
    return b - 256 if b > 127 else b


def today(word):
    return [(is_letter(b), signed(b)) for b in word]


def raw(word):
    return word


def folded(word):
    return (word.decode('utf-8', 'surrogateescape').casefold().encode('utf-8', 'surrogateescape'), word)


RULES = {'today': today, 'bytes': raw, 'folded': folded}


def read_lines(path):
    with open(path, 'rb') as f:
        return f.read().split(b'\n')[:-1]


def main():
    if len(sys.argv) != 4 or sys.argv[1] not in ('sort', 'check') or sys.argv[2] not in RULES:
        sys.exit('usage: order.py sort|check today|bytes|folded <file>')
    key = RULES[sys.argv[2]]
    lines = read_lines(sys.argv[3])
    if sys.argv[1] == 'sort':
        sys.stdout.buffer.write(b''.join(line + b'\n' for line in sorted(lines, key=key)))
        return
    breaks = 0
    for i in range(1, len(lines)):
        if not key(lines[i - 1]) < key(lines[i]):
            breaks += 1
            if breaks <= 5:
                print('order break at line ' + str(i + 1) + ': ' + repr(lines[i - 1]) + ' then '
                      + repr(lines[i]))
    print(str(len(lines)) + ' lines, ' + str(breaks) + ' breaks')
    sys.exit(1 if breaks else 0)


main()
