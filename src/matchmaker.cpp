#include <matchmaker/matchmaker.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>
#include <matchmaker/word_order.h>

#include <arrays.h>
#include <tree.h>


#include <matchmaker/parts_of_speech.h>
#include <matchmaker/ordinal_summation.h>


int mm_count()
{
    return tree::count();
}


int mm_lookup(char const * term, bool * found)
{
    return tree::lookup(term, strlen(term), found);
}


char const * mm_at(int index, int * length)
{
    static char const * empty_str = "";

    if (index < 0 || index >= mm_count())
    {
        if (nullptr != length)
            *length = 0;
        return empty_str;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto & ret_str = leaf.at(index_in_leaf);
    if (nullptr != length)
        *length = (int) ret_str.length();
    return ret_str.c_str();
}


namespace
{
    // the entry of an array that is compiled in units
    int unit_entry(std::span<int const * const> units, size_t entry)
    {
        return units[entry >> arrays::unit_shift][entry & ((size_t{1} << arrays::unit_shift) - 1)];
    }
}


int mm_as_longest(int index)
{
    if (index < 0 || index >= mm_count() || arrays::as_longest.empty())
        return -1;

    return unit_entry(arrays::as_longest, index);
}


int mm_from_longest(int length_index)
{
    if (length_index < 0 || length_index >= mm_count() || arrays::from_longest.empty())
        return -1;

    return unit_entry(arrays::from_longest, length_index);
}


void mm_lengths(int const * * lengths, int * count)
{
    *lengths = arrays::lengths.data();
    *count = (int) arrays::lengths.size();
}


bool mm_length_location(int length, int * index, int * count)
{
    auto const iter = std::lower_bound(arrays::lengths.begin(), arrays::lengths.end(), length);
    if (iter == arrays::lengths.end() || *iter != length)
        return false;

    auto const i = iter - arrays::lengths.begin();
    *index = arrays::length_firsts[i];
    *count = arrays::length_counts[i];
    return true;
}


int mm_ordinal_summation(int index)
{
    if (index < 0 || index >= mm_count())
        return 0;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.ordinal_summation(index_in_leaf);
}


void mm_from_ordinal_summation(int summation, int const * * words, int * count)
{
    if (nullptr == words || nullptr == count)
        return;

    if (summation < 0 || summation >= (int) ORDINAL_SUMMATIONS.size())
    {
        *words = nullptr;
        *count = 0;
        return;
    }

    *words = ORDINAL_SUMMATIONS[summation].data();
    *count = (int) ORDINAL_SUMMATIONS[summation].size();
}


bool mm_parts_of_speech(int index, char const * const * * pos, int8_t const * * flagged, int * count)
{
    static std::vector<char const *> const all = []()
                                                 {
                                                     std::vector<char const *> ret;
                                                     for (auto p : parts_of_speech::variants())
                                                         ret.push_back(p.as_pos_desc().as_string().c_str());
                                                             return ret;
                                                 }();
    *pos = all.data();
    *flagged = nullptr;
    *count = 0;

    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto const & f = leaf.flagged_parts_of_speech(index_in_leaf);
    if (f.size() != all.size())
    {
        std::cout << "******* mm_parts_of_speech() -> f.size() != all.size() *******"
                  << "buggy code detected!" << std::endl;
        return false;
    }

    *flagged = f.data();
    *count = (int) all.size();

    return true;
}


bool mm_is_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_name(index_in_leaf) != 0;
}


bool mm_is_male_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);


    return leaf.is_male_name(index_in_leaf) != 0;
}


bool mm_is_female_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_female_name(index_in_leaf) != 0;
}


bool mm_is_place(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_place(index_in_leaf) != 0;
}


bool mm_is_compound(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_compound(index_in_leaf) != 0;
}


bool mm_is_acronym(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_acronym(index_in_leaf) != 0;
}


bool mm_is_phrase(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_phrase(index_in_leaf) != 0;
}


bool mm_is_used_in_book(int book_index, int index)
{
    if (book_index < 0 || book_index >= mm_book_count() || index < 0 || index >= mm_count())
        return false;

    size_t const ints_per_book = ((size_t) mm_count() + 31) / 32;
    size_t const entry = book_index * ints_per_book + index / 32;
    if (entry >> arrays::unit_shift >= arrays::used_in_book.size())
        return false;

    return ((unsigned) unit_entry(arrays::used_in_book, entry) >> (index % 32) & 1u) != 0;
}


void mm_synonyms(int index, int const * * synonyms, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *synonyms = nullptr;
        *count = 0;
        return;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto & s = leaf.synonyms(index_in_leaf);
    *synonyms = s.data();
    *count = (int) s.size();
}


void mm_antonyms(int index, int const * * antonyms, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *antonyms = nullptr;
        *count = 0;
        return;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto & a = leaf.antonyms(index_in_leaf);
    *antonyms = a.data();
    *count = (int) a.size();
}


void mm_embedded(int index, int const * * embedded, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *embedded = nullptr;
        *count = 0;
        return;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto & e = leaf.embedded(index_in_leaf);
    *embedded = e.data();
    *count = (int) e.size();
}


void mm_definition(int index, int const * * definition, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *definition = nullptr;
        *count = 0;
        return;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    auto & d = leaf.definition(index_in_leaf);
    *definition = d.data();
    *count = (int) d.size();
}


namespace
{
    // how many ranges an array of starts holds: entry i is where range i begins and entry i + 1 where it
    // ends
    int range_count(std::span<int const> starts)
    {
        return starts.empty() ? 0 : (int) starts.size() - 1;
    }


    void heading(std::span<int const> starts, int index, int const * * terms, int * count)
    {
        if (index < 0 || index >= range_count(starts))
        {
            *terms = nullptr;
            *count = 0;
            return;
        }

        *terms = arrays::headings.data() + starts[index];
        *count = starts[index + 1] - starts[index];
    }


    // the group of chapters that holds a book's chapter, with the chapter's place in it, or nullptr
    int const * chapter_group(int book_index, int chapter_index, int * chapter)
    {
        if (chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
            return nullptr;

        int const shift = arrays::book_groups[2 * book_index + 1];
        *chapter = chapter_index & ((1 << shift) - 1);
        return arrays::chapters[arrays::book_groups[2 * book_index] + (chapter_index >> shift)];
    }


    int const * part(int const * group, int which)
    {
        return group + group[which];
    }


    // the place of range's entry at index within starts, or -1 when the range has no such entry
    int entry_at(int const * starts, int range, int index)
    {
        if (index < 0 || index >= starts[range + 1] - starts[range])
            return -1;

        return starts[range] + index;
    }


    // the place of a chapter's paragraph among its group's paragraphs, or -1
    int paragraph_at(int const * group, int chapter, int paragraph_index)
    {
        return entry_at(part(group, arrays::chapter_part::paragraph_starts), chapter, paragraph_index);
    }


    // a chapter's title (which 0) or subtitle (which 1)
    void chapter_heading(int book_index, int chapter_index, int which, int const * * terms, int * count)
    {
        int chapter;
        int const * const group = chapter_group(book_index, chapter_index, &chapter);
        if (nullptr == group)
        {
            *terms = nullptr;
            *count = 0;
            return;
        }

        int const * const starts = part(group, arrays::chapter_part::heading_starts) + 2 * chapter + which;
        *terms = part(group, arrays::chapter_part::headings) + starts[0];
        *count = starts[1] - starts[0];
    }
}


void mm_locations(
    int index,
    int const * * book_indexes,
    int const * * chapter_indexes,
    int const * * paragraph_indexes,
    int const * * word_indexes,
    int * count
)
{
    if (index < 0 || index >= mm_count() || arrays::locations.empty())
    {
        *book_indexes = nullptr;
        *chapter_indexes = nullptr;
        *paragraph_indexes = nullptr;
        *word_indexes = nullptr;
        *count = 0;
        return;
    }

    int const * const block = arrays::locations[index >> arrays::location_shift];
    int const * const starts = block + (index & ((1 << arrays::location_shift) - 1));
    *count = (starts[1] - starts[0]) / 4;
    *book_indexes = block + starts[0];
    *chapter_indexes = *book_indexes + *count;
    *paragraph_indexes = *chapter_indexes + *count;
    *word_indexes = *paragraph_indexes + *count;
}


void mm_lookup_any_case(char const * term, int * start, int * length)
{
    // terms are in key order (word_order.h), so a term's case forms sit together
    size_t const term_length = strlen(term);
    *start = tree::first_not_before(
        [&](char const * t, size_t t_length)
            { return mm_word_key_compare(t, t_length, term, term_length) < 0; });
    *length = tree::first_not_before(
        [&](char const * t, size_t t_length)
            { return mm_word_key_compare(t, t_length, term, term_length) <= 0; }) - *start;
}


void mm_complete(char const * prefix, int * start, int * length)
{
    // shortcut empty prefix to everything
    size_t const prefix_length = strlen(prefix);
    if (0 == prefix_length)
    {
        *start = 0;
        *length = mm_count();
        return;
    }

    // terms are in key order (word_order.h), so the terms whose key starts with the prefix's key sit
    // together: from the first term whose key is not before the prefix's, to the first term whose key,
    // cut to the prefix's length, is after it
    *start = tree::first_not_before(
        [&](char const * t, size_t t_length)
            { return mm_word_key_compare(t, t_length, prefix, prefix_length) < 0; });
    *length = tree::first_not_before(
        [&](char const * t, size_t t_length)
        {
            return mm_word_key_compare(t, std::min(t_length, prefix_length), prefix, prefix_length) <= 0;
        }) - *start;
}


int mm_book_count()
{
    return range_count(arrays::book_chapter_starts);
}


void mm_book_title(int book_index, int const * * book_title, int * word_count)
{
    heading(arrays::book_title_starts, book_index, book_title, word_count);
}


void mm_book_author(int book_index, int const * * book_author, int * word_count)
{
    heading(arrays::book_author_starts, book_index, book_author, word_count);
}


int mm_chapter_count(int book_index)
{
    if (book_index < 0 || book_index >= mm_book_count())
        return 0;

    return arrays::book_chapter_starts[book_index + 1] - arrays::book_chapter_starts[book_index];
}


void mm_chapter_title(int book_index, int chapter_index, int const * * chapter_title, int * word_count)
{
    chapter_heading(book_index, chapter_index, 0, chapter_title, word_count);
}


void mm_chapter_subtitle(int book_index, int chapter_index,
                         int const * * chapter_subtitle, int * word_count)
{
    chapter_heading(book_index, chapter_index, 1, chapter_subtitle, word_count);
}


int mm_paragraph_count(int book_index, int chapter_index)
{
    int chapter;
    int const * const group = chapter_group(book_index, chapter_index, &chapter);
    if (nullptr == group)
        return 0;

    int const * const starts = part(group, arrays::chapter_part::paragraph_starts);
    return starts[chapter + 1] - starts[chapter];
}


int mm_word_count(int book_index, int chapter_index, int paragraph_index)
{
    int chapter;
    int const * const group = chapter_group(book_index, chapter_index, &chapter);
    if (nullptr == group)
        return 0;

    int const paragraph = paragraph_at(group, chapter, paragraph_index);
    if (paragraph == -1)
        return 0;

    int const * const starts = part(group, arrays::chapter_part::word_starts);
    return starts[paragraph + 1] - starts[paragraph];
}


int mm_word(
    int book_index,
    int chapter_index,
    int paragraph_index,
    int word_index,
    int const * * ancestors,
    int * ancestor_count,
    int * index_within_first_ancestor,
    bool * referenced
)
{
    int chapter;
    int const * const group = chapter_group(book_index, chapter_index, &chapter);
    if (nullptr == group)
        return -1;

    int const paragraph = paragraph_at(group, chapter, paragraph_index);
    if (paragraph == -1)
        return -1;

    int const word = entry_at(part(group, arrays::chapter_part::word_starts), paragraph, word_index);
    if (word == -1)
        return -1;

    if (nullptr != ancestors && nullptr != ancestor_count)
    {
        int const * const starts = part(group, arrays::chapter_part::ancestor_starts) + word;
        *ancestors = part(group, arrays::chapter_part::ancestors) + starts[0];
        *ancestor_count = starts[1] - starts[0];
    }

    if (nullptr != index_within_first_ancestor)
        *index_within_first_ancestor = part(group, arrays::chapter_part::within_first_ancestor)[word];

    if (nullptr != referenced)
        *referenced = part(group, arrays::chapter_part::referenced)[word] != 0;

    return part(group, arrays::chapter_part::words)[word];
}
