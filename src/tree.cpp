#include <tree.h>

#include <algorithm>



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

        int node{0};
        for (size_t depth = 0; ; ++depth)
        {
            // terms that end before reaching a leaf sort first, with space
            unsigned char const byte = depth < term.size() ? (unsigned char) term[depth] : ' ';
            int32_t const slot = parents[node][byte];
            int const index = slot >> 2;
            switch (slot & 3)
            {
                case parent:
                    node = index;
                    break;

                case leaf:
                    return leaves[index].lookup(term, found) + offsets()[index];

                case missing:
                    return offsets()[index];

                default:
                    return 0;
            }
        }
    }
}
