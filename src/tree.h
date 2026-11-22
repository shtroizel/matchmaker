#pragma once

#include <cstdint>
#include <string>
#include <vector>



namespace tree
{
    struct Leaf
    {
        int (*count)();
        std::string const & (*at)(int);
        int (*lookup)(std::string const &, bool *);
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

    int lookup(std::string const & term, bool * found);
}



#define MM_DECLARE_LEAF(n)                                                                                 \
    int mm_count_##n();                                                                                    \
    std::string const & mm_at_##n(int);                                                                    \
    int mm_lookup_##n(std::string const &, bool *);                                                        \
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
        &mm_count_##n, &mm_at_##n, &mm_lookup_##n, &mm_as_longest_##n, &mm_ordinal_summation_##n,          \
        &mm_flagged_parts_of_speech_##n, &mm_is_name_##n, &mm_is_male_name_##n, &mm_is_female_name_##n,    \
        &mm_is_place_##n, &mm_is_compound_##n, &mm_is_acronym_##n, &mm_is_phrase_##n,                      \
        &mm_is_used_in_book_##n, &mm_synonyms_##n, &mm_antonyms_##n, &mm_embedded_##n,                     \
        &mm_definition_##n, &mm_locations_##n                                                              \
    }
