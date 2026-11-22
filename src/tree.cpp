#include <tree.h>

#include <algorithm>
#include <cstring>

#include <matchmaker/word_order.h>



namespace tree
{
    namespace
    {
        // offsets[i] is the index of leaf i's first term, offsets[leaf_count] is the term count
        std::vector<int> const & offsets()
        {
            static std::vector<int> const o =
                []()
                {
                    std::vector<int> ret;
                    ret.reserve(leaf_count + 1);
                    int sum{0};
                    for (int i = 0; i < leaf_count; ++i)
                    {
                        ret.push_back(sum);
                        sum += leaves[i].count();
                    }
                    ret.push_back(sum);
                    return ret;
                }();
            return o;
        }
    }


    int count()
    {
        return offsets().back();
    }


    Leaf const & leaf_at(int index, int * index_in_leaf)
    {
        auto const & o = offsets();
        int const i = (int) (std::upper_bound(o.begin(), o.end() - 1, index) - o.begin()) - 1;
        *index_in_leaf = index - o[i];
        return leaves[i];
    }


    int lookup(std::string const & term, bool * found)
    {
        if (nullptr != found)
            *found = false;

        if (term.empty())
            return 0;

        // the last leaf whose first term is not after term, or the first leaf for terms before every leaf
        int first = 0;
        int count = leaf_count;
        while (count > 0)
        {
            int const step = count / 2;
            int const middle = first + step;
            if (mm_word_less(term, first_terms[middle]))
            {
                count = step;
            }
            else
            {
                first = middle + 1;
                count -= step + 1;
            }
        }
        int const leaf = first == 0 ? 0 : first - 1;
        return leaves[leaf].lookup(term, found) + offsets()[leaf];
    }
}
