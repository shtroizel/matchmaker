#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>



// how the stage 1 reader cuts the arrays the library compiles in, so that no compiled file holds more
// ints than a budget
namespace array_cuts
{
    // an array read an entry at a time is cut into units of 1 << unit_shift(budget) entries: the largest
    // power of two within the budget
    inline int unit_shift(int budget)
    {
        int shift{0};
        while (shift < 30 && (2 << shift) <= budget)
            ++shift;
        return shift;
    }


    // items that stay whole are cut into groups of 1 << group_shift(...) consecutive items. A group holds
    // its items' ints and group_ints more. The shift is the smallest that puts every item in one group
    // when that group is within the budget, and otherwise the largest whose largest group is, down to 0:
    // an item over the budget gets a group to itself
    inline int group_shift(std::vector<int> const & item_ints, int group_ints, int budget)
    {
        int shift{0};
        while ((size_t{1} << shift) < item_ints.size())
            ++shift;
        for (; shift > 0; --shift)
        {
            long long largest{0};
            for (size_t first = 0; first < item_ints.size(); first += size_t{1} << shift)
            {
                long long ints{group_ints};
                size_t const end = std::min(item_ints.size(), first + (size_t{1} << shift));
                for (size_t i = first; i < end; ++i)
                    ints += item_ints[i];
                largest = std::max(largest, ints);
            }
            if (largest <= budget)
                break;
        }
        return shift;
    }
}
