#!/usr/bin/env python3
# mini_rerun_check.py <log> <build dir> [moved_text]
# Checks the --time_compiles log of a build that ran stage 1 again (-r -f) on mini_build's build: the stage
# 1 reader was compiled and timed into this log, and no workspace source was compiled again. With the same
# data stage 1 writes the same files, so the arrays the library embeds aren't compiled again either. With
# moved_text the data has book text at other locations: the arrays are compiled again, and still no leaf.

import re
import sys

log, build = sys.argv[1:3]
moved_text = sys.argv[3:] == ['moved_text']
build = build.rstrip('/')
with open(log) as f:
    sources = [re.fullmatch(r'\S+ \S+ (.+)', line).group(1) for line in f.read().splitlines()]

failures = []
if not any('data_reader/stage_1/' in s for s in sources):
    failures.append('no stage 1 reader compile timed into ' + log)
for s in sources:
    if '/workspace/' in s:
        failures.append('compiled again: ' + s)
arrays = [s for s in sources if s.endswith('/src/arrays.cpp')]
if moved_text and not arrays:
    failures.append('the arrays were not compiled again')
if not moved_text and arrays:
    failures.append('compiled again: ' + arrays[0])

for failure in failures:
    print(failure)
print('%d compiles timed' % len(sources))
sys.exit(1 if failures else 0)
