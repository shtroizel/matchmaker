#!/usr/bin/env python3
# mini_export_check.py <c++ compiler> <compiler id> <install dir> <work dir>
# Builds test/mini_export/check.cpp and both versions of test/mini_export/unit.cpp against a matchmaker
# install made with build_and_install.py -e, using the type header it installs for leaf esc_A_b, then runs
# the check with the two units.
import os
import subprocess
import sys

cxx, cxx_id, install_dir, work_dir = sys.argv[1:5]
src_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'mini_export')
lib_dir = os.path.join(install_dir, 'lib', 'matchmaker')
os.makedirs(work_dir, exist_ok=True)

common = [cxx, '-std=c++20', '-O2', '-Wall', '-Werror', '-Wextra', '-DMATCHABLE_OMIT_BY_INDEX',
          '-DLEAF=word_esc_A_b', '-DLEAF_TYPE_HEADER=<matchmaker/leaf_types/esc_A/b/esc_A_b.h>',
          '-I' + os.path.join(install_dir, 'include')]
if cxx_id == 'GNU':
    common.append('-Wno-dangling-reference')

# gcc marks inline statics unique, which keeps a unit mapped after dlclose
unit_flags = ['-fPIC', '-shared', '-DMATCHABLE_UNLOADABLE']
if cxx_id == 'GNU':
    unit_flags.append('-fno-gnu-unique')

units = []
for version in (1, 2):
    unit = os.path.join(work_dir, 'unit_%d.so' % version)
    cmd = common + unit_flags + ['-DUNIT_VERSION=%d' % version, os.path.join(src_dir, 'unit.cpp'),
                                 '-o', unit]
    if subprocess.run(cmd).returncode != 0:
        print('building unit %d failed' % version)
        sys.exit(1)
    units.append(unit)

# the check exports its symbols too, so it shares one registry with the library and the units
check = os.path.join(work_dir, 'mini_export_check')
cmd = common + [os.path.join(src_dir, 'check.cpp'), '-o', check, '-rdynamic', '-L' + lib_dir,
                '-lmatchmaker', '-Wl,-rpath,' + lib_dir, '-ldl']
if subprocess.run(cmd).returncode != 0:
    print('building mini_export_check failed')
    sys.exit(1)

sys.exit(subprocess.run([check] + units).returncode)
