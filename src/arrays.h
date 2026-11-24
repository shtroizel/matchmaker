#pragma once

#include <span>



// data the stage 1 reader writes as plain arrays of int, one file each, compiled into the library; every
// array is empty in a stage 0 library
namespace arrays
{
    // the term at each length index, as an alphabetic index
    extern std::span<int const> const from_longest;

    // the length index of the term at each alphabetic index
    extern std::span<int const> const as_longest;

    // the lengths terms have, ascending, with the length index of the first term of each length and how
    // many terms share it
    extern std::span<int const> const lengths;
    extern std::span<int const> const length_firsts;
    extern std::span<int const> const length_counts;

    // each term's locations in the books: the term at alphabetic index i has those from location_starts[i]
    // up to location_starts[i + 1] in the four arrays below
    extern std::span<int const> const location_starts;
    extern std::span<int const> const location_books;
    extern std::span<int const> const location_chapters;
    extern std::span<int const> const location_paragraphs;
    extern std::span<int const> const location_words;

    // for each book one bit per term, in ints of 32, set when the book uses the term: a book's ints follow
    // the book before it, and the term at alphabetic index i is bit i % 32 of the book's int i / 32
    extern std::span<int const> const used_in_book;

    // chapters, paragraphs and words are numbered through all books: a book's chapters run from
    // book_chapter_starts[b] up to book_chapter_starts[b + 1], a chapter's paragraphs and a paragraph's
    // words likewise
    extern std::span<int const> const book_chapter_starts;
    extern std::span<int const> const chapter_paragraph_starts;
    extern std::span<int const> const paragraph_word_starts;

    // each word of the books' text as an alphabetic index, with the phrases around it from the closest
    // out (text_ancestors, from text_ancestor_starts[w] up to text_ancestor_starts[w + 1]), its place
    // within the closest, and 1 when it is outside the main body of text
    extern std::span<int const> const text_words;
    extern std::span<int const> const text_ancestor_starts;
    extern std::span<int const> const text_ancestors;
    extern std::span<int const> const text_within_first_ancestor;
    extern std::span<int const> const text_referenced;

    // the terms of every title, author and subtitle, with where each book's and each chapter's begin
    extern std::span<int const> const headings;
    extern std::span<int const> const book_title_starts;
    extern std::span<int const> const book_author_starts;
    extern std::span<int const> const chapter_title_starts;
    extern std::span<int const> const chapter_subtitle_starts;
}
