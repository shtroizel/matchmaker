#!/usr/bin/env python3
# linker_comment_check.py <linker name> <library dir>
# Checks that the named linker linked every shared library in the directory: lld and mold record
# "Linker: <name>" in each library's .comment section, maybe with a vendor's name ("Linker: Ubuntu LLD").

import os
import re
import subprocess
import sys
from shutil import which

name = sys.argv[1]
lib_dir = sys.argv[2]
readelf = which('readelf') or which('llvm-readelf')
if readelf is None:
    sys.exit('readelf not found')

libs = sorted(f for f in os.listdir(lib_dir) if f.endswith('.so'))
if not libs:
    sys.exit('no libraries in ' + lib_dir)
failed = False
for lib in libs:
    comment = subprocess.run([readelf, '-p', '.comment', os.path.join(lib_dir, lib)],
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.decode('latin-1')
    if not re.search(r'Linker: (\S+ )*' + re.escape(name) + r'\b', comment):
        print(lib + ' was not linked by ' + name)
        failed = True
sys.exit(1 if failed else 0)
