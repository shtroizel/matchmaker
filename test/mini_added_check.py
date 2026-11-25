#!/usr/bin/env python3
# mini_added_check.py <build dir> <build dir with the word> <word>
# Compares the leaf headers stage 1 wrote for two mini builds whose data differs by one added dictionary
# word. The word moves every term after it one alphabetic index up, and leaves refer to words by name, so
# the leaf that holds the word is the only header that differs. The other leaves must hold references, or
# there would be nothing to compare.

import os
import re
import sys

base, added, word = sys.argv[1].rstrip('/'), sys.argv[2].rstrip('/'), sys.argv[3]


def headers(build):
    root = os.path.join(build, 'workspace', 'generated_include', 'matchmaker', 'generated_matchables')
    found = {}
    for leaf in sorted(os.listdir(root)):
        with open(os.path.join(root, leaf, leaf + '.h'), encoding='latin-1') as f:
            found[leaf] = f.read()
    return found


before, after = headers(base), headers(added)
declares = re.compile(r'[(,] esc_' + re.escape(word) + r'[,)]')
links = re.compile(r'MATCHABLE_VARIANT_PROPERTY_VALUES\(\w+, \w+, (syn|ant|embedded|definition), ')
holders = [leaf for leaf, text in after.items() if declares.search(text)]
failures = []
if len(holders) != 1:
    failures.append('%d leaves declare %s' % (len(holders), word))
if any(declares.search(text) for text in before.values()):
    failures.append(base + ' already has ' + word)

others = 0
for leaf in sorted(set(before) | set(after)):
    if leaf in holders:
        continue
    if leaf not in before:
        failures.append('new leaf: ' + leaf)
    elif leaf not in after:
        failures.append('leaf gone: ' + leaf)
    elif before[leaf] != after[leaf]:
        failures.append('leaf changed: ' + leaf)
    elif links.search(after[leaf]):
        others += 1
if others == 0:
    failures.append('no other leaf links its words to others')

for failure in failures:
    print(failure)
print('%d leaves, %d of the unchanged ones link their words to others' % (len(after), others))
sys.exit(1 if failures else 0)
