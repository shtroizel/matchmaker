#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <matchable/matchable.h>



namespace tree
{
    struct Leaf
    {
        int (*count)();
        std::string const & (*at)(int);
        // the terms in order, packed: searched where they are
        ::matchable::VariantNames const & (*names)();
        int (*as_longest)(int);
        int (*ordinal_summation)(int);
        std::vector<int8_t> const & (*flagged_parts_of_speech)(int);
        bool (*is_name)(int);
        bool (*is_male_name)(int);
        bool (*is_female_name)(int);
        bool (*is_place)(int);
        bool (*is_compound)(int);
        bool (*is_acronym)(int);
        bool (*is_phrase)(int);
        bool (*is_used_in_book)(int, int);
        std::vector<int> const & (*synonyms)(int);
        std::vector<int> const & (*antonyms)(int);
        std::vector<int> const & (*embedded)(int);
        std::vector<int> const & (*definition)(int);
        void (*locations)(int, int const * *, int const * *, int const * *, int const * *, int *);
    };

    // generated (tree_data.cpp): the leaves in word order (word_order.h), and each leaf's first term
    extern Leaf const leaves[];
    extern char const * const first_terms[];
    extern int const leaf_count;

    int count();

    // index must be within [0, count())
    Leaf const & leaf_at(int index, int * index_in_leaf);

    // the index of a leaf's first term; first_index(leaf_count) is count()
    int first_index(int leaf);

    // the last leaf whose first term is not after term, or the first leaf for terms before every leaf
    int leaf_of(char const * term, size_t length);

    int lookup(char const * term, size_t length, bool * found);

    // the first index whose term is not before, or count(). before(term, length) must be true for the
    // terms up to some index and false from there on. It is called O(log N) times: on leaves' first
    // terms, then on the names of one leaf
    template<typename Before>
    int first_not_before(Before const & before)
    {
        // the first leaf whose first term is not before
        int first = 0;
        int remaining = leaf_count;
        while (remaining > 0)
        {
            int const step = remaining / 2;
            int const middle = first + step;
            if (before(first_terms[middle], std::strlen(first_terms[middle])))
            {
                first = middle + 1;
                remaining -= step + 1;
            }
            else
            {
                remaining = step;
            }
        }
        if (first == 0)
            return 0;

        // the leaf before it starts with a term that is before, so the answer is in it or is its end
        return first_index(first - 1) + leaves[first - 1].names().lower_bound_by(before);
    }
}



#define MM_DECLARE_LEAF(n)                                                                                 \
    int mm_count_##n();                                                                                    \
    std::string const & mm_at_##n(int);                                                                    \
    ::matchable::VariantNames const & mm_names_##n();                                                      \
    int mm_as_longest_##n(int);                                                                            \
    int mm_ordinal_summation_##n(int);                                                                     \
    std::vector<int8_t> const & mm_flagged_parts_of_speech_##n(int);                                       \
    bool mm_is_name_##n(int);                                                                              \
    bool mm_is_male_name_##n(int);                                                                         \
    bool mm_is_female_name_##n(int);                                                                       \
    bool mm_is_place_##n(int);                                                                             \
    bool mm_is_compound_##n(int);                                                                          \
    bool mm_is_acronym_##n(int);                                                                           \
    bool mm_is_phrase_##n(int);                                                                            \
    bool mm_is_used_in_book_##n(int, int);                                                                 \
    std::vector<int> const & mm_synonyms_##n(int);                                                         \
    std::vector<int> const & mm_antonyms_##n(int);                                                         \
    std::vector<int> const & mm_embedded_##n(int);                                                         \
    std::vector<int> const & mm_definition_##n(int);                                                       \
    void mm_locations_##n(int, int const * *, int const * *, int const * *, int const * *, int *);

#define MM_LEAF(n)                                                                                         \
    tree::Leaf{                                                                                            \
        &mm_count_##n, &mm_at_##n, &mm_names_##n,  &mm_as_longest_##n, &mm_ordinal_summation_##n,          \
        &mm_flagged_parts_of_speech_##n, &mm_is_name_##n, &mm_is_male_name_##n, &mm_is_female_name_##n,    \
        &mm_is_place_##n, &mm_is_compound_##n, &mm_is_acronym_##n, &mm_is_phrase_##n,                      \
        &mm_is_used_in_book_##n, &mm_synonyms_##n, &mm_antonyms_##n, &mm_embedded_##n,                     \
        &mm_definition_##n, &mm_locations_##n                                                              \
    }
