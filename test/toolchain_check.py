#!/usr/bin/env python3
# toolchain_check.py <toolchain dir> <library dir>
# Checks that the pinned toolchain built every shared library in the directory: its Clang compiled it and
# its lld linked it (both named in the library's .comment section), the library needs no C++ runtime
# beside it, and it exports nothing of libc++.

import os
import re
import subprocess
import sys


def output(cmd):
    return subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.decode('latin-1')


toolchain, lib_dir = sys.argv[1:3]
tool = os.path.join(toolchain, 'bin', '')
version = re.search(r'clang version (\S+)', output([tool + 'clang++', '--version']))
if version is None:
    sys.exit('no clang++ version from ' + toolchain)
version = version.group(1)

libs = sorted(f for f in os.listdir(lib_dir) if f.endswith('.so'))
if not libs:
    sys.exit('no libraries in ' + lib_dir)
failures = []
for lib in libs:
    path = os.path.join(lib_dir, lib)
    comment = output([tool + 'llvm-readelf', '-p', '.comment', path])
    if 'clang version ' + version not in comment:
        failures.append(lib + ' was not compiled by clang ' + version)
    if 'Linker: LLD ' + version not in comment:
        failures.append(lib + ' was not linked by lld ' + version)
    needed = re.findall(r'\(NEEDED\).*\[(.+)\]', output([tool + 'llvm-readelf', '-d', path]))
    for runtime in [n for n in needed if re.match(r'lib(std)?c\+\+|libunwind', n)]:
        failures.append(lib + ' needs ' + runtime)
    exported = output([tool + 'llvm-nm', '-D', '--defined-only', path])
    if '_ZNSt3__1' in exported or '_ZNKSt3__1' in exported:
        failures.append(lib + ' exports libc++ symbols')
print('\n'.join(failures))
sys.exit(1 if failures else 0)
