#!/usr/bin/env python3
# reader0_full.py <reader0_fixture> <data dir> <parent prefix file> <work dir>
# Runs the stage 0 reader over the full data and checks how its results relate, then prints a summary
# (counts per mode) for expect_output.py to compare:
# - each mode saves exactly the words it keeps (reader0_fixture checks this)
# - every book vocabulary word is a word, and the vocabulary is the same in every mode
# - none is the books (vocabulary and keys); reduced adds every dictionary but 3201 SINGLE and COMPOUND,
#   and the words saving adds (in none those only annotate, like any source that is not a book); all adds
#   SINGLE and COMPOUND
# - a word kept in more than one mode has the same leaf, parts of speech, ordinal summation and flags

import collections
import os
import subprocess
import sys

fixture, data, parents, work = sys.argv[1:5]

READ_3201 = ['reading 3201 (' + n + ')' for n in
             ['SINGLE', 'COMPOUND', 'COMMON', 'NAMES', 'NAMES-F', 'NAMES-M', 'PLACES', 'CROSSWD', 'CRSWD D',
              'ACRONYMS']]
DICTIONARIES = READ_3201 + ['reading 3202', 'reading 3203']
BOOKS = ['reading Crumbs', 'reading Keys']
SAVED_BY_READER = {'shtroizel', 'Shtroizel'}

runs = {
    'none': ('none', DICTIONARIES + BOOKS + ['saving matchables']),
    'reduced': ('reduced', DICTIONARIES + BOOKS + ['saving matchables']),
    'all': ('all', DICTIONARIES + BOOKS + ['saving matchables']),
    'keys': ('all', ['reading Keys']),
    'reduced_adds': ('all', [d for d in DICTIONARIES if d not in READ_3201[:2]]),
    'all_adds': ('all', READ_3201[:2]),
}

os.makedirs(work, exist_ok=True)
processes = {}
for name, (mode, tasks) in runs.items():
    output = os.path.join(work, name)
    with open(output + '.txt', 'wb') as out, open(output + '.err', 'wb') as err:
        processes[name] = subprocess.Popen([fixture, data, parents, mode, output] + tasks,
                                           stdout=out, stderr=err)

problems = []
for name, p in processes.items():
    if p.wait() != 0:
        problems.append(name + ': reader0_fixture exited with ' + str(p.returncode) + ', see ' +
                        os.path.join(work, name + '.err'))
if problems:
    sys.exit('\n'.join(problems))


def load(name):
    words = {}
    vocabulary = []
    saved = ''
    in_vocabulary = False
    with open(os.path.join(work, name + '.txt'), encoding='latin-1') as f:
        for line in f:
            line = line.rstrip('\n')
            if line.startswith('saved '):
                saved = line
            elif line.startswith('vocabulary '):
                in_vocabulary = True
            elif in_vocabulary:
                vocabulary.append(line[5:-1])
            else:
                bar = line.rfind(' | ')
                leaf_start = line.rfind('] ', 0, bar)
                pos, os_, *flags = line[bar + 3:].split(' ')
                words[line[1:leaf_start]] = (line[leaf_start + 2:bar], pos, os_, frozenset(flags))
    return words, vocabulary, saved


results = {name: load(name) for name in runs}
modes = ['none', 'reduced', 'all']
words = {m: set(results[m][0]) for m in modes}


def check(ok, message):
    if not ok:
        problems.append(message)


def sample(s):
    return sorted(s)[:5]


vocabulary = results['all'][1]
for m in modes:
    check(results[m][1] == vocabulary, m + ': vocabulary differs from all')
    missing = set(results[m][1]) - words[m]
    check(not missing, m + ': vocabulary words not kept: ' + str(len(missing)) + ' ' + str(sample(missing)))

expected = {
    'none': set(vocabulary) | set(results['keys'][0]),
}
expected['reduced'] = expected['none'] | set(results['reduced_adds'][0]) | SAVED_BY_READER
expected['all'] = expected['reduced'] | set(results['all_adds'][0])
for m in modes:
    extra = words[m] - expected[m]
    missing = expected[m] - words[m]
    check(not extra and not missing, m + ': ' + str(len(extra)) + ' unexpected words ' +
          str(sample(extra)) + ', ' + str(len(missing)) + ' missing ' + str(sample(missing)))

for a, b in [('none', 'reduced'), ('reduced', 'all')]:
    differ = [w for w in words[a] & words[b] if results[a][0][w] != results[b][0][w]]
    check(not differ, a + ' and ' + b + ': ' + str(len(differ)) + ' words differ ' + str(sample(differ)))

if problems:
    sys.exit('\n'.join(problems))

for m in modes:
    kept = results[m][0]
    flags = collections.Counter(f for (_, _, _, fs) in kept.values() for f in fs)
    pos = collections.Counter(p for (_, ps, _, _) in kept.values() for p in ps[len('pos='):])
    print(m + ':')
    print('    ' + results[m][2])
    print('    vocabulary ' + str(len(vocabulary)) + ', words with parts of speech ' +
          str(sum(1 for (_, ps, _, _) in kept.values() if ps != 'pos=')))
    print('    flags ' + ' '.join(f + '=' + str(n) for f, n in sorted(flags.items())))
    print('    pos ' + ' '.join(p + '=' + str(n) for p, n in sorted(pos.items())))
