#pragma once

#include <vector>



struct BookWord
{
    int word{-1};
    std::vector<int> ancestors;
    int index_within_first_ancestor{-1};
};
