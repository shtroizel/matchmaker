#!/usr/bin/env python3
# time_compile_check.py <time_compile.py> <C++ compiler> <work dir>
# Runs time_compile.py around a compile that works and one that fails, checking that each appends one
# "<seconds> <peak RSS KB> <source>" line to the log and that the compiler's exit code comes back unchanged.

import os
import re
import shutil
import subprocess
import sys

launcher, compiler, work = sys.argv[1:4]
shutil.rmtree(work, ignore_errors=True)
os.makedirs(work)
log = os.path.join(work, 'compiles.txt')
good = os.path.join(work, 'good.cpp')
bad = os.path.join(work, 'bad.cpp')
with open(good, 'w') as f:
    f.write('int main() { return 0; }\n')
with open(bad, 'w') as f:
    f.write('int main() { return undeclared; }\n')

failures = []


def compile_with_launcher(source):
    obj = source + '.o'
    direct = subprocess.run([compiler, '-c', source, '-o', obj], stderr=subprocess.DEVNULL).returncode
    timed = subprocess.run([sys.executable, launcher, log, compiler, '-c', source, '-o', obj],
                           stderr=subprocess.PIPE)
    if timed.returncode != direct:
        failures.append('%s: exit code %d through the launcher, %d without'
                        % (source, timed.returncode, direct))
    return timed


compile_with_launcher(good)
bad_result = compile_with_launcher(bad)
if b'undeclared' not in bad_result.stderr:
    failures.append('the failed compile\'s error message did not come through')

with open(log) as f:
    lines = f.read().splitlines()
if len(lines) != 2:
    failures.append('expected 2 log lines, got %d: %r' % (len(lines), lines))
for line, source in zip(lines, [good, bad]):
    m = re.fullmatch(r'(\d+\.\d\d) (\d+) (.+)', line)
    if m is None:
        failures.append('malformed line: ' + line)
    elif m.group(3) != source:
        failures.append('expected source %s in line: %s' % (source, line))
    elif int(m.group(2)) <= 0:
        failures.append('no peak memory in line: ' + line)

for failure in failures:
    print(failure)
sys.exit(1 if failures else 0)
