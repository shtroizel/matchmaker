#include <relations.h>

#ifdef MM_GENERATED_RELATIONS

#include <arrays.h>



void relations::get(Kind kind, int index, int const * * terms, int * count)
{
    static std::span<int const * const> const * const blocks[kind_count] = {
        &arrays::synonyms, &arrays::antonyms, &arrays::embedded, &arrays::definitions
    };
    static int const * const shifts[kind_count] = {
        &arrays::synonyms_shift, &arrays::antonyms_shift, &arrays::embedded_shift,
        &arrays::definitions_shift
    };

    int const shift = *shifts[kind];
    int const * const block = (*blocks[kind])[index >> shift];
    int const * const starts = block + (index & ((1 << shift) - 1));
    *terms = block + starts[0];
    *count = starts[1] - starts[0];
}

#else

#include <array>
#include <unordered_map>
#include <vector>

#include <tree.h>



namespace
{
    // one kind's lists for every term, back to back: the list of the term at index i runs from starts[i]
    // up to starts[i + 1]
    struct Lists
    {
        std::vector<int> starts;
        std::vector<int> terms;
    };


    std::array<Lists, relations::kind_count> const & lists()
    {
        static std::array<Lists, relations::kind_count> const all =
            []()
            {
                using List = std::vector<::matchable::VariantRef> const & (*)(int);
                std::array<Lists, relations::kind_count> ret;
                for (Lists & l : ret)
                    l.starts.reserve(tree::count() + 1);

                // most references are to a word that others refer to as well
                std::unordered_map<::matchable::Variant const *, int> index_of;

                for (int leaf = 0; leaf < tree::leaf_count; ++leaf)
                {
                    tree::Leaf const & l = tree::leaves[leaf];
                    List const list[relations::kind_count] = {
                        l.synonyms, l.antonyms, l.embedded, l.definition
                    };
                    // as many terms as the tree counts here, so each list sits at its term's index
                    int const count = tree::first_index(leaf + 1) - tree::first_index(leaf);
                    for (int i = 0; i < count; ++i)
                    {
                        for (int kind = 0; kind < relations::kind_count; ++kind)
                        {
                            ret[kind].starts.push_back((int) ret[kind].terms.size());
                            for (::matchable::VariantRef word : list[kind](i))
                            {
                                auto const known = index_of.emplace(word.get(), 0);
                                if (known.second)
                                    known.first->second = tree::index_of(*word);
                                ret[kind].terms.push_back(known.first->second);
                            }
                        }
                    }
                }
                for (Lists & l : ret)
                    l.starts.push_back((int) l.terms.size());
                return ret;
            }();
        return all;
    }
}



void relations::get(Kind kind, int index, int const * * terms, int * count)
{
    Lists const & l = lists()[kind];
    *terms = l.terms.data() + l.starts[index];
    *count = l.starts[index + 1] - l.starts[index];
}

#endif
