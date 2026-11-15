#include <matchmaker/matchmaker.h>

#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>

#include <tree.h>


#include <matchmaker/parts_of_speech.h>
#include <matchmaker/longest_words.h>
#include <matchmaker/ordinal_summation.h>


#ifndef STAGE0
#include <matchmaker/books/books_content.h>
#endif


int mm_count()
{
    return tree::count();
}


int mm_lookup(char const * term, bool * found)
{
    return tree::lookup(term, found);
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


int mm_as_longest(int index)
{
    if (index < 0 || index >= mm_count())
        return -1;

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.as_longest(index_in_leaf);
}


int mm_from_longest(int length_index)
{
    if (length_index < 0 || length_index >= mm_count())
        return -1;

    return LONGEST_WORDS[length_index];
}


void mm_lengths(int const * * lengths, int * count)
{
    static std::vector<int> const v = []()
                                      {
                                          std::vector<int> v;
                                          for (auto const & [l, o] : LONGEST_WORDS_OFFSETS)
                                              v.push_back(l);
                                          return v;
                                      }();
    *lengths = v.data();
    *count = v.size();
}


bool mm_length_location(int length, int * index, int * count)
{
    auto iter = LONGEST_WORDS_OFFSETS.find(length);
    if (iter == LONGEST_WORDS_OFFSETS.end())
        return false;

    *index = iter->second.first;
    *count = iter->second.second - iter->second.first + 1;
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

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    return leaf.is_used_in_book(book_index, index_in_leaf) != 0;
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


void mm_locations(
    int index,
    int const * * book_indexes,
    int const * * chapter_indexes,
    int const * * paragraph_indexes,
    int const * * word_indexes,
    int * count
)
{
    if (index < 0 || index >= mm_count())
    {
        *book_indexes = nullptr;
        *chapter_indexes = nullptr;
        *paragraph_indexes = nullptr;
        *word_indexes = nullptr;
        *count = 0;
        return;
    }

    int index_in_leaf;
    tree::Leaf const & leaf = tree::leaf_at(index, &index_in_leaf);

    leaf.locations(
        index_in_leaf,
        book_indexes,
        chapter_indexes,
        paragraph_indexes,
        word_indexes,
        count
    );
}


void mm_complete(char const * prefix, int * start, int * length)
{
    // shortcut empty prefix to everything
    if (0 == strlen(prefix))
    {
        *start = 0;
        *length = mm_count();
        return;
    }

    auto str_starts_with =
        [](char const * str, int str_len, char const * starts_with)
        {
            int starts_with_len = strlen(starts_with);

            if (starts_with_len > str_len)
                return false;

            for (int i = 0; i < starts_with_len; ++i)
                if (str[i] != starts_with[i])
                    return false;

            return true;
        };

    int index = mm_lookup(prefix, nullptr);

    *start = index;
    *length = 0;
    int s_len = 0;
    char const * s = nullptr;
    for (; index < mm_count(); ++index)
    {
        s = mm_at(index, &s_len);

        if (str_starts_with(s, s_len, prefix))
            *length += 1;
        else
            break;
    }
}


int mm_book_count()
{
#ifdef STAGE0
    return 0;
#else
    return BOOK_COUNT;
#endif
}


void mm_book_title(int book_index, int const * * book_title, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) book_title;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
    {
        *book_title = nullptr;
        *word_count = 0;
        return;
    }

    book_title_funcs[book_index](book_title, word_count);
#endif
}


void mm_book_author(int book_index, int const * * book_author, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) book_author;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
    {
        *book_author = nullptr;
        *word_count = 0;
        return;
    }

    book_author_funcs[book_index](book_author, word_count);
#endif
}


int mm_chapter_count(int book_index)
{
#ifdef STAGE0
    (void) book_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
        return 0;

    return chapter_count_funcs[book_index]();
#endif
}


void mm_chapter_title(int book_index, int chapter_index, int const * * chapter_title, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) chapter_title;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_title = nullptr;
        *word_count = 0;
        return;
    }

    chapter_title_funcs[book_index](chapter_index, chapter_title, word_count);
#endif
}


void mm_chapter_subtitle(int book_index, int chapter_index,
                         int const * * chapter_subtitle, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) chapter_subtitle;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_subtitle = nullptr;
        *word_count = 0;
        return;
    }

    chapter_subtitle_funcs[book_index](chapter_index, chapter_subtitle, word_count);
#endif
}


int mm_paragraph_count(int book_index, int chapter_index)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
        return 0;

    return paragraph_count_funcs[book_index](chapter_index);
#endif
}


int mm_word_count(int book_index, int chapter_index, int paragraph_index)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) paragraph_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index))
        return 0;

    return word_count_funcs[book_index](chapter_index, paragraph_index);
#endif
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
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) paragraph_index;
    (void) word_index;
    (void) ancestors;
    (void) ancestor_count;
    (void) index_within_first_ancestor;
    (void) referenced;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index) ||
            word_index < 0 || word_index >= mm_word_count(book_index, chapter_index, paragraph_index))
        return -1;

    return word_funcs[book_index](chapter_index,
                                  paragraph_index,
                                  word_index,
                                  ancestors,
                                  ancestor_count,
                                  index_within_first_ancestor,
                                  referenced);
#endif
}
