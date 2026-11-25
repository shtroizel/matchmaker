#pragma once

#include <span>



// data the stage 1 reader writes as arrays of int, compiled into the library; every array is empty in a
// stage 0 library. The reader cuts the large ones, so that no compiled file holds more ints than its
// budget
namespace arrays
{
    // the lengths terms have, ascending, with the length index of the first term of each length and how
    // many terms share it
    extern std::span<int const> const lengths;
    extern std::span<int const> const length_firsts;
    extern std::span<int const> const length_counts;

    // arrays read an entry at a time are compiled in units of 1 << unit_shift entries: the term at each
    // length index as an alphabetic index, the length index of the term at each alphabetic index, and
    // for each book one bit per term, in ints of 32, set when the book uses the term (a book's ints
    // follow the book before it, and the term at alphabetic index i is bit i % 32 of the book's int
    // i / 32)
    extern int const unit_shift;
    extern std::span<int const * const> const from_longest;
    extern std::span<int const * const> const as_longest;
    extern std::span<int const * const> const used_in_book;

    // each term's locations in the books, compiled in blocks of 1 << location_shift terms. A block begins
    // with where in it each of its terms' locations begin, and one entry more for where the last term's
    // end; a term's locations are its books, then its chapters, its paragraphs and its words
    extern int const location_shift;
    extern std::span<int const * const> const locations;

    // with MM_GENERATED_RELATIONS, each term's synonyms, antonyms, embedded terms and definition as
    // alphabetic indexes, each kind compiled in blocks of 1 << its shift terms. A block begins with where
    // in it each of its terms' list begins, and one entry more for where the last term's ends
    extern int const synonyms_shift;
    extern int const antonyms_shift;
    extern int const embedded_shift;
    extern int const definitions_shift;
    extern std::span<int const * const> const synonyms;
    extern std::span<int const * const> const antonyms;
    extern std::span<int const * const> const embedded;
    extern std::span<int const * const> const definitions;

    // chapters are numbered through all books: a book's run from book_chapter_starts[b] up to
    // book_chapter_starts[b + 1]
    extern std::span<int const> const book_chapter_starts;

    // a book's chapters are compiled in groups of 1 << shift chapters, the shift its own: book b's first
    // group is chapters[book_groups[2 * b]] and its shift is book_groups[2 * b + 1]
    extern std::span<int const> const book_groups;
    extern std::span<int const * const> const chapters;

    // a group of chapters begins with where in it each of these parts begins. Paragraphs, words,
    // ancestors and headings are numbered from the group's first, each level saying where the next
    // level's begin, with one entry more for where the last one's end: a chapter's paragraphs
    // (paragraph_starts), a paragraph's words (word_starts) and a word's ancestors (ancestor_starts, the
    // phrases around the word from the closest out). Each word has its term as an alphabetic index, its
    // place within its closest ancestor, and 1 when it is outside the main body of text. heading_starts
    // has two entries per chapter: where its title begins among the headings, then its subtitle
    namespace chapter_part
    {
        enum
        {
            paragraph_starts,
            word_starts,
            words,
            ancestor_starts,
            ancestors,
            within_first_ancestor,
            referenced,
            heading_starts,
            headings
        };
    }

    // the terms of every book's title and author, with where each book's begin
    extern std::span<int const> const headings;
    extern std::span<int const> const book_title_starts;
    extern std::span<int const> const book_author_starts;
}
