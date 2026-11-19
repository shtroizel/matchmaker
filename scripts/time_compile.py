#!/usr/bin/env python3
# time_compile.py <log> <compiler command>...
# Compiler launcher behind build_and_install.py --time_compiles: runs the command, then appends
# "<seconds> <peak RSS KB> <source>" to <log>. Peak RSS covers the compiler and the processes it waits for.

import os
import resource
import subprocess
import sys
import time

log = sys.argv[1]
command = sys.argv[2:]

source = command[-1]
for flag, value in zip(command, command[1:]):
    if flag == '-c':
        source = value

start = time.monotonic()
returncode = subprocess.call(command)
seconds = time.monotonic() - start

peak = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
if sys.platform == 'darwin':
    peak //= 1024

# one write per line with O_APPEND, so parallel compiles never interleave within a line
fd = os.open(log, os.O_WRONLY | os.O_APPEND | os.O_CREAT, 0o644)
os.write(fd, ('%.2f %d %s\n' % (seconds, peak, source)).encode())
os.close(fd)

sys.exit(returncode if returncode >= 0 else 128 - returncode)
