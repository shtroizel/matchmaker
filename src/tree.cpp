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


    int first_index(int leaf)
    {
        return offsets()[leaf];
    }


    int leaf_of(char const * term, size_t length)
    {
        int first = 0;
        int remaining = leaf_count;
        while (remaining > 0)
        {
            int const step = remaining / 2;
            int const middle = first + step;
            if (mm_word_compare(term, length, first_terms[middle], std::strlen(first_terms[middle])) < 0)
            {
                remaining = step;
            }
            else
            {
                first = middle + 1;
                remaining -= step + 1;
            }
        }
        return first == 0 ? 0 : first - 1;
    }


    int lookup(char const * term, size_t length, bool * found)
    {
        if (nullptr != found)
            *found = false;

        if (length == 0)
            return 0;

        int const leaf = leaf_of(term, length);
        ::matchable::VariantNames const & names = leaves[leaf].names();
        int const position = names.lower_bound_by(
            [&](char const * name, size_t name_length)
                { return mm_word_compare(name, name_length, term, length) < 0; });
        if (nullptr != found)
            *found = position != names.size() && names.equals(position, term, length);
        return position + offsets()[leaf];
    }
}
