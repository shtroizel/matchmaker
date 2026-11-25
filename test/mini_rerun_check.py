#!/usr/bin/env python3
# mini_rerun_check.py <log> <build dir> [moved_text | recut | generated]
# Checks the --time_compiles log of a build that ran stage 1 again (-r -f) on mini_build's build: the stage
# 1 reader was compiled and timed into this log, and no leaf was compiled again. With the same data stage
# 1 writes the same files, so nothing the library embeds is compiled again either. With moved_text the
# data has the text of two posts at other locations: their groups of chapters and the blocks holding
# their terms' locations are compiled again, and not the first post's group, the blocks of other terms,
# the arrays the text leaves alone or the tables. With recut the budget differs, so the units and the
# tables are compiled again. With generated the build changed to --relation_ranks=generated on the same
# data: the blocks of each kind of link and the tables are compiled, and no unit of the other arrays.

import os
import re
import sys

log, build = sys.argv[1:3]
mode = sys.argv[3] if len(sys.argv) > 3 else 'same'
build = build.rstrip('/')
with open(log) as f:
    sources = [re.fullmatch(r'\S+ \S+ (.+)', line).group(1) for line in f.read().splitlines()]

failures = []
if not any('data_reader/stage_1/' in s for s in sources):
    failures.append('no stage 1 reader compile timed into ' + log)
units = [s for s in sources if '/workspace/generated_src/arrays/' in s]
tables = [s for s in sources if s.endswith('/src/arrays.cpp')]
for s in sources:
    if '/workspace/' in s and s not in units:
        failures.append('compiled again: ' + s)


def compiled(kind):
    return [s for s in units if re.search('/' + kind + r'_\d+\.cpp$', s)]


if mode == 'same':
    for s in units + tables:
        failures.append('compiled again: ' + s)
if mode == 'moved_text':
    for kind in ('chapters', 'locations'):
        if not compiled(kind):
            failures.append('no unit of ' + kind + ' was compiled again')
    for s in compiled('as_longest') + compiled('from_longest') + compiled('used_in_book') + tables:
        failures.append('compiled again: ' + s)
    for s in units:
        if s.endswith('/chapters_0.cpp'):
            failures.append('compiled again: ' + s)
    blocks = [n for n in os.listdir(os.path.join(build, 'workspace', 'generated_src', 'arrays'))
              if re.fullmatch(r'locations_\d+\.cpp', n)]
    if len(blocks) > 1 and len(compiled('locations')) == len(blocks):
        failures.append('every block of locations was compiled again')
if mode == 'recut':
    if not units:
        failures.append('no unit was compiled again')
    if not tables:
        failures.append('the tables were not compiled again')
if mode == 'generated':
    relations = ('synonyms', 'antonyms', 'embedded', 'definitions')
    for kind in relations:
        if not compiled(kind):
            failures.append('no block of ' + kind + ' was compiled')
    if not tables:
        failures.append('the tables were not compiled again')
    for s in units:
        if not any(s in compiled(kind) for kind in relations):
            failures.append('compiled again: ' + s)

for failure in failures:
    print(failure)
print('%d compiles timed' % len(sources))
sys.exit(1 if failures else 0)
