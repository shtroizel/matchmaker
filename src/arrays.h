#pragma once

#include <span>



// data the stage 1 reader writes as plain arrays of int, one file each, embedded in the library; every
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
}
