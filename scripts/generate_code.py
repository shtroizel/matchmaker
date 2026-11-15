#!/usr/bin/env python3

import os
import shutil
import getopt
import sys

from string import ascii_lowercase
from string import ascii_uppercase
from pathlib import Path



SYMBOLS = ["_spc_", "_bng_", "_quot_", "_hsh_", "_dol_", "_pct_", "_und_", "_sqt_", "_parl_", "_parr_",
           "_ast_", "_plus_", "_cma_", "_mns_", "_dot_", "_slsh_",
           "esc_0", "esc_1", "esc_2", "esc_3", "esc_4", "esc_5", "esc_6", "esc_7", "esc_8", "esc_9",
           "_cln_", "_scln_", "_lt_", "_eq_", "_gt_", "_qstn_", "_atsym_",
           "_sbl_", "_bslsh_", "_sbr_", "_dach_", "_", "_bqt_",
           "_cbl_", "_pip_", "_cbr_", "_tld_"] \
        + ['esc_' + ch for ch in ascii_uppercase] \
        + list(ascii_lowercase)

MAX_PREFIX_DEPTH = 6

# byte of each symbol: all non-letters, then all letters, each group in byte order (matchable::str_lt_str)
SYMBOL_BYTES = dict(zip(SYMBOLS, sorted(range(32, 127), key=lambda b: (chr(b).isalpha(), b))))



def usage():
    print(sys.argv[0] + ' [OPTION]')
    print('    -h, --help                  print this message')
    print('    -w, --workspace_root        workspace directory')
    print('    -q, --q                     q only')
    print('    -p, --parents_only          update parents only')
    print('    -t, --tree                  parent prefix file (default: scripts/parent_prefixes.txt)')
    print('    -l, --leaves                leaf list from the stage 0 reader: generate only these leaves')



def read_parent_prefixes(filename):
    parents = set()
    with open(filename) as f:
        for line_number, line in enumerate(f, 1):
            prefix = tuple(line.split())
            if len(prefix) == 0:
                continue
            where = str(filename) + ':' + str(line_number)
            if len(prefix) >= MAX_PREFIX_DEPTH:
                sys.exit(where + ': parent prefix deeper than ' + str(MAX_PREFIX_DEPTH - 1))
            for symbol in prefix:
                if symbol not in SYMBOLS:
                    sys.exit(where + ': unknown symbol "' + symbol + '"')
            parents.add(prefix)
    for prefix in parents:
        if len(prefix) > 1 and prefix[:-1] not in parents:
            sys.exit('parent prefix "' + ' '.join(prefix) + '" lacks parent "'
                     + ' '.join(prefix[:-1]) + '"')
    return parents



def write_from_template(template, template_text, prefix, destination):
    text = template_text.replace('aoeu', '/'.join(prefix)).replace('snth', '_'.join(prefix))
    with open(destination, 'w') as f:
        f.write(text)
    shutil.copymode(template, destination)



def write_if_changed(filename, text):
    if filename.exists() and filename.read_text() == text:
        return
    filename.write_text(text)



def prepare_letters(workspace_root, q, parents_only, tree, leaf_list):

    generated_include = workspace_root / 'generated_include'
    generated_src = workspace_root / 'generated_src'
    generated_symbols = generated_include / 'matchmaker' / 'generated_symbols'

    if not parents_only:
        for location in [generated_symbols, generated_src]:
            shutil.rmtree(location, ignore_errors=True)
            os.makedirs(location)

    matchmaker_root = Path(os.path.dirname(os.path.realpath(__file__))).parent
    parents = read_parent_prefixes(tree or matchmaker_root / 'scripts' / 'parent_prefixes.txt')
    wanted = None
    if leaf_list is not None:
        wanted = set(tuple(line.split()) for line in Path(leaf_list).read_text().splitlines())

    templates = {}
    for name in ['internal_interface.h', 'leaf.cpp']:
        template = matchmaker_root / 'templates' / name
        templates[name] = (template, template.read_text())

    leaves = []
    parent_slots = []

    def add_node(prefix):
        if prefix in parents:
            first_leaf = len(leaves)
            slots = [first_leaf << 2 | 3] * 256
            parent_slots.append(slots)
            index = len(parent_slots) - 1
            for symbol in SYMBOLS:
                slots[SYMBOL_BYTES[symbol]] = add_node(prefix + (symbol,))
            if len(leaves) == first_leaf:
                parent_slots.pop()
                return first_leaf << 2 | 3
            return index << 2 | 1

        if wanted is not None and prefix not in wanted:
            return len(leaves) << 2 | 3

        name = '_'.join(prefix)
        leaves.append(name)
        if not parents_only:
            os.makedirs(generated_symbols.joinpath(*prefix))
            h = generated_symbols.joinpath(*prefix) / (name + '.h')
            write_from_template(*templates['internal_interface.h'], prefix, h)
            write_from_template(*templates['leaf.cpp'], prefix, generated_src / (name + '.cpp'))
        return (len(leaves) - 1) << 2 | 2

  #            '         1         2         3         4         5         6         7         0'
  #            '12345678901234567890123456789012345678901234567890123456789012345678901234567890'
    progress = "Buckle up, buttercups - it's about to get bumpy!";
    progress_index = 0

    root = [3] * 256
    parent_slots.append(root)
    for symbol in SYMBOLS:
        if progress_index % 2 == 0:
            print(progress[progress_index // 2], end='')
        progress_index += 1
        sys.stdout.flush()
        root[SYMBOL_BYTES[symbol]] = add_node((symbol,))
    print('')

    if wanted is not None and len(leaves) != len(wanted):
        sys.exit(leaf_list + ': ' + str(len(wanted) - len(leaves)) + ' leaves are not leaves of the tree')

    tree_data = '// generated by scripts/generate_code.py from scripts/parent_prefixes.txt\n\n'
    tree_data += '#include <tree.h>\n\n'
    tree_data += ''.join('MM_DECLARE_LEAF(' + leaf + ')\n' for leaf in leaves)
    tree_data += '\nnamespace tree\n{\n    extern Leaf const leaves[] = {\n'
    tree_data += ''.join('        MM_LEAF(' + leaf + '),\n' for leaf in leaves)
    tree_data += '    };\n\n    extern int const leaf_count = ' + str(len(leaves)) + ';\n\n'
    tree_data += '    extern int32_t const parents[][256] = {\n'
    for slots in parent_slots:
        tree_data += '        {' + ','.join(str(s) for s in slots) + '},\n'
    tree_data += '    };\n}\n'
    write_if_changed(generated_src / 'tree_data.cpp', tree_data)

    tree_cmake = '# generated by scripts/generate_code.py from scripts/parent_prefixes.txt\n\n'
    tree_cmake += 'set(matchmaker_srcs generated_src/tree_data.cpp)\n\n'
    tree_cmake += 'set(\n    prefix_list\n' + ''.join('    ' + leaf + '\n' for leaf in leaves) + ')\n'
    write_if_changed(workspace_root / 'tree.cmake', tree_cmake)



def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hw:qpt:l:',
                                   ['help', 'workspace_root=', 'q', 'parents_only', 'tree=', 'leaves='])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    workspace_root = ''
    q = False
    parents_only = False
    tree = None
    leaf_list = None

    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-w', '--workspace_root'):
            workspace_root = a
        elif o in ('-q', '--q'):
            q = True
        elif o in ('-p', '--parents_only'):
            parents_only = True
        elif o in ('-t', '--tree'):
            tree = a
        elif o in ('-l', '--leaves'):
            leaf_list = a
        else:
            assert False, "unhandled option"

    if workspace_root == '':
        print("workspace_root unset, use -w to set a workspace directory")
        usage()

    workspace_root = Path(workspace_root)

    prepare_letters(workspace_root, q, parents_only, tree, leaf_list)
    exit(0)



if __name__ == "__main__":
    main()
