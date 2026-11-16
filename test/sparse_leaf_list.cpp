// sparse_leaf_list <parent prefix file>
// Prints a leaf list, as the stage 0 reader writes one, naming some of the leaves: every third leaf, except
// none starting with 'q', so every leaf under parent "q" is missing
#include <iostream>

#include <routing.h>

#include "tree_leaves.h"



int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <parent prefix file>" << std::endl;
        return 2;
    }

    auto const leaves = all_leaves(read_parents(argv[1]));
    for (size_t i = 0; i < leaves.size(); i += 3)
        if (leaves[i][0] != 'q')
            std::cout << routing::symbols(leaves[i], ' ') << '\n';
    return 0;
}
