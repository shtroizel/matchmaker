#pragma once

// force-included ahead of a generated tree_data.cpp: each leaf holds one term, so tree::lookup() returns
// the index of the leaf a term reaches

#include <tree.h>



namespace stub
{
    inline int count() { return 1; }

    inline int lookup(std::string const &, bool * found)
    {
        if (nullptr != found)
            *found = true;
        return 0;
    }
}

#undef MM_DECLARE_LEAF
#define MM_DECLARE_LEAF(n)

#undef MM_LEAF
#define MM_LEAF(n)                                                                                         \
    tree::Leaf{                                                                                            \
        &stub::count, nullptr, &stub::lookup, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,        \
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr           \
    }
