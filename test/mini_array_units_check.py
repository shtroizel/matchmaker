#!/usr/bin/env python3
# mini_array_units_check.py <build dir> <budget> <kind>=<units> ...
# Checks how stage 1 cut the arrays of a mini build into compiled units (workspace/generated_src/arrays):
# each kind's list names exactly its unit files, numbered from 0; as_longest, from_longest and
# used_in_book come in units of the largest power of two of entries within the budget; a block of locations
# or a group of chapters is over the budget only when it holds one term or one chapter; and each kind has
# the number of units given, so the fixture stays cut the way the other tests rely on. The kinds of link
# (synonyms, antonyms, embedded, definitions) are generated only with --relation_ranks=generated: given a
# number of units they must be there, in blocks cut like those of locations, and otherwise absent.

import os
import re
import sys

build, budget = sys.argv[1].rstrip('/'), int(sys.argv[2])
expected = dict(arg.split('=') for arg in sys.argv[3:])
arrays = os.path.join(build, 'workspace', 'generated_src', 'arrays')
failures = []


def ints(name):
    with open(os.path.join(arrays, name)) as f:
        return [int(line.rstrip(',\n')) for line in f if re.fullmatch(r'-?\d+,\n', line)]


def units(kind):
    with open(os.path.join(arrays, kind + '.inc')) as f:
        listed = re.findall(r'MM_UNIT\((\w+)\)', f.read())
    names = ['%s_%d' % (kind, n) for n in range(len(listed))]
    if listed != names:
        failures.append('%s.inc lists %s' % (kind, ' '.join(listed)))
    files = sorted(n[:-4] for n in os.listdir(arrays) if re.fullmatch(kind + r'_\d+\.cpp', n))
    if files != sorted(names):
        failures.append('%s has the unit files %s' % (kind, ' '.join(files)))
    if kind in expected and len(names) != int(expected[kind]):
        failures.append('%s is cut into %d units, not %s' % (kind, len(names), expected[kind]))
    return [len(ints(name + '.cpp')) for name in names]


unit_shift = ints('unit_shift.inc')[0]
if not (1 << unit_shift) <= budget < (2 << unit_shift):
    failures.append('units of %d entries for a budget of %d' % (1 << unit_shift, budget))
for kind in ('as_longest', 'from_longest', 'used_in_book'):
    sizes = units(kind)
    if any(size != 1 << unit_shift for size in sizes[:-1]) or not 1 <= sizes[-1] <= 1 << unit_shift:
        failures.append('%s units hold %s entries' % (kind, sizes))

location_shift = ints('location_shift.inc')[0]
for block, size in enumerate(units('locations')):
    if size > budget and location_shift != 0:
        failures.append('locations_%d holds %d ints in blocks of %d terms'
                        % (block, size, 1 << location_shift))

for kind in ('synonyms', 'antonyms', 'embedded', 'definitions'):
    generated = os.path.exists(os.path.join(arrays, kind + '.inc'))
    if kind not in expected:
        if generated:
            failures.append(kind + ' was generated')
        continue
    if not generated:
        failures.append(kind + ' was not generated')
        continue
    shift = ints(kind + '_shift.inc')[0]
    for block, size in enumerate(units(kind)):
        if size > budget and shift != 0:
            failures.append('%s_%d holds %d ints in blocks of %d terms' % (kind, block, size, 1 << shift))

book_groups = ints('book_groups.inc')
sizes = units('chapters')
for book in range(len(book_groups) // 2):
    first, shift = book_groups[2 * book:2 * book + 2]
    end = book_groups[2 * book + 2] if 2 * book + 2 < len(book_groups) else len(sizes)
    for group in range(first, end):
        if sizes[group] > budget and shift != 0:
            failures.append('chapters_%d holds %d ints in groups of %d chapters'
                            % (group, sizes[group], 1 << shift))

for failure in failures:
    print(failure)
print('budget %d: units of %d entries, blocks of %d terms, %d groups of chapters'
      % (budget, 1 << unit_shift, 1 << location_shift, len(sizes)))
sys.exit(1 if failures else 0)
