#pragma once

// force-included ahead of a generated tree_data.cpp, so routing_check links without the leaves

#include <tree.h>



namespace stub
{
    inline int count() { return 1; }
}

#undef MM_DECLARE_LEAF
#define MM_DECLARE_LEAF(n)

#undef MM_LEAF
#define MM_LEAF(n)                                                                                         \
    tree::Leaf{                                                                                            \
        &stub::count, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,                       \
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr                    \
    }
