#!/usr/bin/env python3
# mini_exports_check.py <toolchain dir> <library dir>
# Checks what the libraries of a mini build export: the mm_* functions, and from the leaf libraries the
# address of each word's instance, which other leaves refer to by name, as many as the library has terms.
# Each address is a weak symbol with protected visibility, so a library's own references stay inside it
# and a second matchmaker in the process keeps its own. Nothing else is exported, the instances' objects
# included.

import ctypes
import os
import re
import subprocess
import sys

toolchain, lib_dir = sys.argv[1:3]
readelf = os.path.join(toolchain, 'bin', 'llvm-readelf')
address = re.compile(r'_ZN9matchable8InstanceIN\d+word_\w+EE7addressE')
leaf_lib = re.compile(r'libmatchmaker_\d+\.so')

failures = []
addresses = 0
for lib in sorted(f for f in os.listdir(lib_dir) if f.endswith('.so')):
    out = subprocess.run([readelf, '--dyn-syms', '-W', os.path.join(lib_dir, lib)],
                         stdout=subprocess.PIPE, check=True).stdout.decode('latin-1')
    for line in out.splitlines():
        fields = line.split()
        if len(fields) != 8 or not fields[0].rstrip(':').isdigit() or fields[6] == 'UND':
            continue
        bind, visibility, symbol = fields[4], fields[5], fields[7].split('@')[0]
        if leaf_lib.fullmatch(lib) and address.fullmatch(symbol):
            addresses += 1
            if (bind, visibility) != ('WEAK', 'PROTECTED'):
                failures.append('%s exports %s as %s %s' % (lib, symbol, bind, visibility))
        elif not re.match(r'(_Z\d*)?mm_', symbol):
            failures.append('%s exports %s' % (lib, symbol))

mm = ctypes.CDLL(os.path.join(lib_dir, 'libmatchmaker.so'))
if addresses != mm.mm_count():
    failures.append('%d instance addresses exported for %d terms' % (addresses, mm.mm_count()))

for failure in failures[:20]:
    print(failure)
print('%d instance addresses exported' % addresses)
sys.exit(1 if failures else 0)
