// array_cuts_check: the stage 1 reader's rules for cutting the generated arrays into compiled files: units
// of entries within a budget, and groups of whole items checked against the rule's definition over
// generated item sizes
#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

#include <array_cuts.h>



namespace
{
    int failures{0};

    void expect(bool ok, std::string const & what)
    {
        if (!ok && ++failures <= 20)
            std::cout << "FAIL: " << what << std::endl;
    }


    long long largest_group(std::vector<int> const & item_ints, int group_ints, int shift)
    {
        long long largest{0};
        for (size_t first = 0; first < item_ints.size(); first += size_t{1} << shift)
        {
            long long ints{group_ints};
            for (size_t i = first; i < std::min(item_ints.size(), first + (size_t{1} << shift)); ++i)
                ints += item_ints[i];
            largest = std::max(largest, ints);
        }
        return largest;
    }


    // checks a shift against the rule's definition: no shift is larger than one group of every item
    // needs, the largest group is within the budget unless each item has a group to itself, and one shift
    // more would hold every item already or go over the budget
    void check_groups(std::vector<int> const & item_ints, int group_ints, int budget)
    {
        int const shift = array_cuts::group_shift(item_ints, group_ints, budget);
        std::string const where = std::to_string(item_ints.size()) + " items, " +
                                  std::to_string(group_ints) + " per group, budget " +
                                  std::to_string(budget) + ": shift " + std::to_string(shift) + " ";
        expect(shift >= 0, where + "is not negative");
        if (shift < 0)
            return;
        expect(shift == 0 || (size_t{1} << (shift - 1)) < item_ints.size(),
               where + "is no larger than needed");
        expect(shift == 0 || largest_group(item_ints, group_ints, shift) <= budget,
               where + "fits the budget");
        expect((size_t{1} << shift) >= item_ints.size() ||
                   largest_group(item_ints, group_ints, shift + 1) > budget,
               where + "is the largest that fits");
    }
}



int main()
{
    using array_cuts::group_shift;
    using array_cuts::unit_shift;

    expect(unit_shift(1) == 0, "a budget of 1 gives units of 1");
    expect(unit_shift(2) == 1, "a budget of 2 gives units of 2");
    expect(unit_shift(3) == 1, "a budget of 3 gives units of 2");
    expect(unit_shift(4) == 2, "a budget of 4 gives units of 4");
    expect(unit_shift(100) == 6, "a budget of 100 gives units of 64");
    expect(unit_shift(1000000) == 19, "a budget of 1,000,000 gives units of 524,288");
    expect(unit_shift(1048575) == 19, "a budget of 1,048,575 gives units of 524,288");
    expect(unit_shift(1048576) == 20, "a budget of 1,048,576 gives units of 1,048,576");
    expect(unit_shift(INT_MAX) == 30, "the largest budget gives units of 1 << 30");

    expect(group_shift({}, 5, 100) == 0, "no items: shift 0");
    expect(group_shift({10}, 5, 100) == 0, "one item: shift 0");
    expect(group_shift({10, 10, 10}, 5, 100) == 2, "three items that fit together: one group of 4");
    expect(group_shift({10, 10, 10, 10, 10}, 5, 100) == 3, "five items that fit together: one group of 8");
    expect(group_shift({40, 40, 40, 40}, 5, 100) == 1, "four items of 40 in a budget of 100: groups of 2");
    expect(group_shift({40, 40, 40, 40}, 25, 100) == 0, "the group's own ints count: groups of 1");
    expect(group_shift({90, 1, 1, 1, 1, 1, 1, 1}, 5, 100) == 2,
           "a large item with small ones: groups of 4");
    expect(group_shift({1, 1, 90, 90, 1, 1, 1, 1}, 5, 100) == 0,
           "two large items side by side: groups of 1");
    expect(group_shift({500, 1}, 5, 100) == 0, "an item over the budget gets a group to itself");
    expect(group_shift({45, 50}, 5, 100) == 1, "a group of exactly the budget fits");
    expect(group_shift({45, 51}, 5, 100) == 0, "a group of one int over the budget is cut");

    unsigned state{12345};
    auto const next = [&]() { state = state * 1103515245u + 12345u; return (state >> 16) & 0x7fff; };
    for (int round = 0; round < 400; ++round)
    {
        std::vector<int> item_ints(next() % 70);
        for (int & ints : item_ints)
            ints = next() % 5 == 0 ? (int) (next() % 400) : (int) (next() % 30);
        check_groups(item_ints, (int) (next() % 20), 1 + (int) (next() % 600));
    }

    std::cout << (failures == 0 ? "ok" : std::to_string(failures) + " failures") << std::endl;
    return failures == 0 ? 0 : 1;
}
