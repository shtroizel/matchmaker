#!/usr/bin/env python3
# expect_output.py <expected file> <command>...
# Runs the command and compares its standard output with the expected file, printing the differences.
# With UPDATE_EXPECTED=1 it writes the output to the expected file instead (review the change before
# committing it).

import difflib
import os
import subprocess
import sys

expected_path = sys.argv[1]
result = subprocess.run(sys.argv[2:], stdout=subprocess.PIPE)
if result.returncode != 0:
    sys.exit(sys.argv[2] + ' exited with ' + str(result.returncode))
actual = result.stdout.decode('latin-1')

if os.environ.get('UPDATE_EXPECTED') == '1':
    with open(expected_path, 'w', encoding='latin-1', newline='') as f:
        f.write(actual)
    print('wrote ' + expected_path)
    sys.exit(0)

with open(expected_path, encoding='latin-1', newline='') as f:
    expected = f.read()
if actual != expected:
    sys.stdout.writelines(difflib.unified_diff(expected.splitlines(True), actual.splitlines(True),
                                               expected_path, 'actual'))
    sys.exit(1)
