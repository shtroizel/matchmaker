#!/usr/bin/env python3
# mini_export_check.py <toolchain dir> <install dir> <work dir>
# Builds test/mini_export/check.cpp and both versions of test/mini_export/unit.cpp against a matchmaker
# install made with build_and_install.py -e, using the type header it installs for the leaf that holds
# Abzz1 to Abzz3 (found in the installed leaf_list), then runs the check with the two units. The pinned
# toolchain that built the install builds them, with libc++ linked in.
import glob
import os
import subprocess
import sys

toolchain, install_dir, work_dir = sys.argv[1:4]
src_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'mini_export')
lib_dir = os.path.join(install_dir, 'lib', 'matchmaker')
os.makedirs(work_dir, exist_ok=True)


# the library's word order (include/matchmaker/word_order.h): the word with its ASCII letters lowercased,
# then the word itself, both as unsigned bytes
def term_key(word):
    return (bytes(b + 32 if 65 <= b <= 90 else b for b in word), word)


# the leaf whose range holds a word: the last whose first word is not after it
def leaf_of(word, leaves):
    return [name for name, first in leaves if term_key(first) <= term_key(word)][-1]


leaf_list = os.path.join(install_dir, 'include', 'matchmaker', 'leaf_types', 'leaf_list')
with open(leaf_list, 'rb') as f:
    leaves = [(name.decode(), first) for name, first in
              (line.split(b' ', 1) for line in f.read().split(b'\n') if line)]
leaf = leaf_of(b'Abzz1', leaves)
if any(leaf_of(w, leaves) != leaf for w in (b'Abzz2', b'Abzz3')):
    print('Abzz1 to Abzz3 fall in different leaves')
    sys.exit(1)

common = [os.path.join(toolchain, 'bin', 'clang++'), '-stdlib=libc++', '-std=c++20', '-O2', '-Wall',
          '-Werror', '-Wextra', '-DMATCHABLE_OMIT_BY_INDEX',
          '-DLEAF=word_' + leaf, '-DLEAF_TYPE_HEADER=<matchmaker/leaf_types/%s/%s.h>' % (leaf, leaf),
          '-I' + os.path.join(install_dir, 'include')]
link = ['-fuse-ld=lld', '-nostdlib++']
for name in ('c++', 'c++abi', 'unwind'):
    link += glob.glob(os.path.join(toolchain, 'lib', '**', 'lib%s.a' % name), recursive=True)[:1]
link.append('-lpthread')

unit_flags = ['-fPIC', '-shared', '-DMATCHABLE_UNLOADABLE']

units = []
for version in (1, 2):
    unit = os.path.join(work_dir, 'unit_%d.so' % version)
    cmd = common + unit_flags + ['-DUNIT_VERSION=%d' % version, os.path.join(src_dir, 'unit.cpp'),
                                 '-o', unit] + link
    if subprocess.run(cmd).returncode != 0:
        print('building unit %d failed' % version)
        sys.exit(1)
    units.append(unit)

# the check exports its symbols too, so it shares one registry with the library and the units
check = os.path.join(work_dir, 'mini_export_check')
cmd = common + [os.path.join(src_dir, 'check.cpp'), '-o', check, '-rdynamic', '-L' + lib_dir,
                '-lmatchmaker', '-Wl,-rpath,' + lib_dir, '-ldl'] + link
if subprocess.run(cmd).returncode != 0:
    print('building mini_export_check failed')
    sys.exit(1)

sys.exit(subprocess.run([check] + units).returncode)
