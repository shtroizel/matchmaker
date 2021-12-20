#pragma once


/*
Copyright (c) 2020-2021, shtroizel
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <stdbool.h>
#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif



/****************************** note to users of this library **********************************************

any attempts to manage memory by deleting anything provided by any of these functions will result in
self-inflicted pain and suffering...

all of matchmaker's functions adhere to the following:
    * matchmaker always owns everything
    * dynamic memory allocation is avoided completely
    * all arrays or strings retrieved always exist because they are static!

matchmaker works with built-in types only

this is the only header you need for using the matchmaker library.

thats it, have fun!

***********************************************************************************************************/



/**
 * retrieve the dictionary's term count.
 * the complexity for this fuction is O(1).
 * @returns
 *     the number of terms in the dictionary
 */
int mm_count();

/**
 * retrieve a dictionary index from a string.
 * the complexity for this function is O(log N).
 * @param[in] term
 *     string to look up in the dictionary
 * @param[out] found
 *     true if term is found, false otherwise
 *     ignored when nullptr
 * @returns
 *     the alphabetic index of the given term if it exists,
 *     or the index that the given term would have if it did exist.
 *     note that this could be mm_count() if *found == false
 */
int mm_lookup(char const * term, bool * found);

/**
 * retrieve the null-terminated byte string for a given dictionary index.
 * the length of the string can optionally also be retrieved, saving a call to strlen().
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary. valid indexes fall in the range of [0..mm_count()-1]
 * @param[out] length
 *     the length of the given term (ignored if NULL)
 * @returns
 *     the term at the given index, or empty string when index is out of range
 */
char const * mm_at(int index, int * length);

/**
 * matchmaker also provides its dictionary in an alternative sorting order (longest terms first).
 * use this function to retrieve a "length index" from an "alphabetic index".
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     a "length index", which is the index the given term would have in a dictionary
 *     sorted from longest to shortest, with 0 for the longest term and mm_count() - 1 for
 *     the shortest.
 * @see mm_from_longest()
 */
int mm_as_longest(int index);

/**
 * matchmaker also provides its dictionary in an alternative sorting order (longest terms first).
 * note that alphabetic indexes are "normal" indexes and are used by all non-length related functions.
 * use this function to retrieve an "alphabetic index" from a "length index".
 * the complexity for this function is O(1).
 * @param[in] length_index
 *     the index a term would have in a dictionary sorted from longest to shortest,
 *     with 0 for the longest term and mm_count() - 1 for the shortest.
 * @returns
 *     an alphabetic index for the term of the given length index
 * @see mm_as_longest()
 */
int mm_from_longest(int length_index);

/**
 * retrieve all possible lengths that a matchmaker term could have in ascending order.
 * the complexity for this function is O(1).
 * @param[out] lengths
 *     array of all possible lengths that a matchmaker term could have in ascending order
 * @param[out] count
 *     number of elements in the array
 * @see
 *     mm_length_location()
 */
void mm_lengths(int const * * lengths, int * count);

/**
 * retrieve the "length index" of the first term (in descending length order) having the given length,
 * as well as the number of terms that share this length.
 * the complexity for this function is O(1).
 * @param[in] length
 *     length of a term
 * @param[out] length_index
 *     terms of given length begin at this "length index"
 * @param[out] count
 *     number of terms with the given length
 * @returns
 *     true if any term has the given length,
 *     false otherwise (in which case length_index and count remain unchanged)
 * @see mm_lengths()
 */
bool mm_length_location(int length, int * length_index, int * count);

/**
 * retrieve the ordinal summation of a dictionary term.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     the sum of each of the term's letter's ordinal values, such that upper and lower case share
 *     the same value and that only letters are evaluated (no spaces, hyphens, etc)
 */
int mm_ordinal_summation(int index);

/**
 * retrieve an array of all terms having a given ordinal summation.
 * the complexity for this function is O(1).
 * @param[in] summation
 *     an ordinal summation for which pertaining terms are to be retrieved
 * @param[out] terms
 *     alphabetic indexes for each term with the given summation
 * @param[out] count
 *     number of terms found with the given ordinal sum
 */
void mm_from_ordinal_summation(int summation, int const * * terms, int * count);

/**
 * retrieve the parts of speech for a given term (word).
 * note that many words, phrases and other expressions do not have parts of speech data.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @param[out] pos
 *     pointer to an array of strings representing all parts of speech known to matchmaker
 * @param[out] flagged
 *     pointer to an array of booleans (stored as int8_t) such that the array's length is equal to the
 *     length of the array pointed to by pos
 *     elements are set to 1 if their cooresponding pos index pertains to the
 *     given term, or 0 otherwise.
 * @param[out] count
 *     number of array elements (same for both pos and flagged)
 * @returns
 *     true when index in range [0..mm_count() - 1] and false when index out of range
 */
bool mm_parts_of_speech(int index, char const * const * * pos, int8_t const * * flagged, int * count);

/**
 * discover whether a given term happens to be a name.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is a name, false otherwise
 */
bool mm_is_name(int index);

/**
 * discover whether a given term happens to be a male name.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is a male name, false otherwise
 */
bool mm_is_male_name(int index);

/**
 * discover whether a given term happens to be a female name.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is a female name, false otherwise
 */
bool mm_is_female_name(int index);

/**
 * discover whether a given term happens to be a place.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is a place, false otherwise
 */
bool mm_is_place(int index);

/**
 * discover whether a given term happens to be a compound word or phrase.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is a compound word or phrase, false otherwise
 */
bool mm_is_compound(int index);

/**
 * discover whether a given term happens to be an acronym.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the given term is an acronym, false otherwise
 */
bool mm_is_acronym(int index);

/**
 * discover whether a given term is a phrase
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the term is a phrase, false otherwise
 */
bool mm_is_phrase(int index);

/**
 * discover whether a given term is used in a given book
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book in the library
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @returns
 *     true if the term is used at least once in the given book, false otherwise
 */
bool mm_is_used_in_book(int book_index, int index);

/**
 * retrieve an array of synonyms for a given term.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @param[out] synonyms
 *     pointer to an array of alphabetic indexes for synonyms of the given term
 * @param[out] count
 *     number of synonyms in the array
 */
void mm_synonyms(int index, int const * * synonyms, int * count);

/**
 * retrieve an array of antonyms for a given term.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @param[out] antonyms
 *     pointer to an array of alphabetic indexes for antonyms of the given term
 * @param[out] count
 *     number of antonyms in the array
 */
void mm_antonyms(int index, int const * * antonyms, int * count);

/**
 * retrieve the embedded terms within a phrase or expression.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic index of a term in the dictionary
 * @param[out] embedded
 *     pointer to an array of embedded term alphabetic indexes
 * @param[out] count
 *     number of terms in the "embedded" array
 */
void mm_embedded(int index, int const * * embedded, int * count);

/**
 * retrieve all locations within books where a given term is used.
 * for any i in [0..(count - 1)],
 * book_indexes[i], chapter_indexes[i], paragraph_indexes[i], and word_indexes[i] together make up
 * a "location" that could be passed to mm_word(), which would return the same alphabetic dictionary
 * index used here as input.
 * the complexity for this function is O(1).
 * @param[in] index
 *     alphabetic dictionary index
 * @param[out] book_indexes
 *     pointer to an array of book_indexes
 * @param[out] chapter_indexes
 *     pointer to an array of chapter_indexes
 * @param[out] paragraph_indexes
 *     pointer to an array of paragraph_indexes
 * @param[out] word_indexes
 *     pointer to an array of "word indexes".
 *     note that "word indexes" relate to the position of a word within a paragraph
 *     and are not dictionary indexes!
 * @param[out] count
 *     number of indexes contained within each array
 */
void mm_locations(
    int index,
    int const * * book_indexes,
    int const * * chapter_indexes,
    int const * * paragraph_indexes,
    int const * * word_indexes,
    int * count
);

/**
 * retrieve all terms beginning with a given prefix.
 * the complexity for this function appears to be O(N)...
 *     However, completing the entire dictionary (when prefix is empty) shortcuts to O(1).
 *     Assuming even distribution (it isn't),
 *     the worst case is actually mm_count() / 52 possible first letters.
 *     Each additional letter in the prefix divides this by another 52, so the average complexity is
 *     actually something like mm_count() / pow(52, strlen(prefix)) for non-empty prefixes
 * @param[in] prefix
 *     prefix to complete
 * @param[out] start
 *     index of first word with the given prefix, or -1 if length is 0
 * @param[out] length
 *     number of words with the given prefix
 */
void mm_complete(char const * prefix, int * start, int * length);

/**
 * retrieve the library's book count.
 * The complexity for this function is O(1).
 * @returns
 *     the number of books in the library
 */
int mm_book_count();

/**
 * retrieve a book's title.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book in the library
 * @param[out] book_title
 *     array of alphabetic indexes to terms making up the title
 * @param[out] count
 *     the number of terms in the title
 */
void mm_book_title(int book_index, int const * * book_title, int * count);

/**
 * retrieve a book's author.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book in the library
 * @param[out] book_author
 *     array of alphabetic indexes to terms making up the author
 * @param[out] count
 *     the number of terms making up the author
 */
void mm_book_author(int book_index, int const * * book_author, int * count);

/**
 * retrieve a book's chapter count.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book in the library
 * @returns
 *     the number of chapters within the given book
 */
int mm_chapter_count(int book_index);

/**
 * retrieve a chapter's title.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book within the library
 * @param[in] chapter_index
 *     index of a chapter within the book
 * @param[out] chapter_title
 *     array of alphabetic indexes to terms making up the title
 * @param[out] count
 *     the number of terms in the title
 */
void mm_chapter_title(int book_index, int chapter_index, int const * * chapter_title, int * count);

/**
 * retrieve a chapter's subtitle.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book within the library
 * @param[in] chapter_index
 *     index of a chapter within the book
 * @param[out] chapter_subtitle
 *     array of alphabetic indexes to terms making up the subtitle
 * @param[out] count
 *     the number of terms in the subtitle
 */
void mm_chapter_subtitle(int book_index, int chapter_index, int const * * chapter_subtitle, int * count);

/**
 * retrieve a chapter's paragraph count.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book within the library
 * @param[in] chapter_index
 *     index of a chapter within the book
 * @returns
 *     the number of paragraphs within the given chapter
 */
int mm_paragraph_count(int book_index, int chapter_index);

/**
 * retrieve a paragraph's word count.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book within the library
 * @param[in] chapter_index
 *     index of a chapter within the book
 * @param[in] paragraph_index
 *     index of a paragraph within the chapter
 * @returns
 *     the number of words within the given paragraph
 */
int mm_word_count(int book_index, int chapter_index, int paragraph_index);

/**
 * retrieve an alphabetic dictionary index given a location within a book.
 * the complexity for this function is O(1).
 * @param[in] book_index
 *     index of a book within the library
 * @param[in] chapter_index
 *     index of a chapter within the book
 * @param[in] paragraph_index
 *     index of a paragraph within the chapter
 * @param[in] word_index
 *     index of a word within the paragraph
 * @param[out] parent_phrase
 *     dictionary index of the given word's parent phrase if it has one or -1 otherwise.
 *     if the parent is not needed then this parameter should be NULL
 * @param[out] parent_phrase_word_index
 *     word index within the given word's parent phrase if it has one or -1 otherwise.
 *     if the parent is not needed then this parameter should be NULL
 * @returns
 *     the alphabetic dictionary index for a word in the paragraph or -1 if provided an invalid index
 */
int mm_word(
    int book_index,
    int chapter_index,
    int paragraph_index,
    int word_index,
    int * parent_phrase,
    int * parent_phrase_word_index
);

#ifdef __cplusplus
}
#endif
