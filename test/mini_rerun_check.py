#!/usr/bin/env python3
# mini_rerun_check.py <log> <build dir>
# Checks the --time_compiles log of mini_rerun, which ran stage 1 again (-r -f) on mini_build's build with
# the same data: the stage 1 reader was compiled and timed into this log, and no workspace source was
# compiled again, since stage 1 wrote the same files.

import re
import sys

log, build = sys.argv[1:3]
build = build.rstrip('/')
with open(log) as f:
    sources = [re.fullmatch(r'\S+ \S+ (.+)', line).group(1) for line in f.read().splitlines()]

failures = []
if not any('data_reader/stage_1/' in s for s in sources):
    failures.append('no stage 1 reader compile timed into ' + log)
for s in sources:
    if '/workspace/' in s:
        failures.append('compiled again: ' + s)

for failure in failures:
    print(failure)
print('%d compiles timed' % len(sources))
sys.exit(1 if failures else 0)
