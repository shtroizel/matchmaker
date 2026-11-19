#!/usr/bin/env python3
# mini_build_compiles_check.py <log> <build dir>
# Checks the log that build_and_install.py --time_compiles wrote during mini_build: every line well formed,
# and compiles timed from matchable, both data readers and both stage workspaces.

import re
import sys

log, build = sys.argv[1:3]
build = build.rstrip('/')
with open(log) as f:
    lines = f.read().splitlines()

sources = []
failures = []
for line in lines:
    m = re.fullmatch(r'\d+\.\d\d (\d+) (.+)', line)
    if m is None or int(m.group(1)) <= 0:
        failures.append('malformed line: ' + line)
    else:
        sources.append(m.group(2))

expected = {
    'matchable': 'src/MatchableMaker.cpp',
    'stage 0 reader': 'data_reader/stage_0/',
    'stage 1 reader': 'data_reader/stage_1/',
    'stage 0 workspace': build + '_stage_0/workspace/',
    'stage 1 workspace': build + '/workspace/',
}
for part, fragment in expected.items():
    if not any(fragment in s for s in sources):
        failures.append('no compile timed from ' + part + ' (' + fragment + ')')

for failure in failures:
    print(failure)
print('%d compiles timed' % len(sources))
sys.exit(1 if failures else 0)
