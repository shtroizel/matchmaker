#pragma once


/*
Copyright (c) 2020, Eric Hyer
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


/**
 * @returns
 *     the number of words in the dictionary
 */
int mm_count();

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @param[out] length
 *     the length of the given word (ignored if NULL)
 * @returns
 *     the word at the given index, or empty string when index is out of range
 */
char const * mm_at(int index, int * length);

/**
 * @param[in] word
 *     string to look up in the dictionary
 * @param[out] found
 *     true if word is found, false otherwise
 *     ignored when nullptr
 * @returns
 *     the alphabetic index of the given word if it exists,
 *     or the index that the given word would have if it did exist.
 *     note that this could be mm_count() if *found == false
 */
int mm_lookup(char const * word, bool * found);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     a "length index", which is the index the given word would have in a dictionary
 *     sorted from longest to shortest, with 0 for the longest word and mm_count() - 1 for
 *     the shortest.
 * @see from_longest()
 */
int mm_as_longest(int index);

/**
 * @param[in] length_index
 *     the index a word would have in a dictionary sorted from longest to shortest,
 *     with 0 for the longest word and mm_count() - 1 for the shortest.
 * @returns
 *     an alphabetic index for the word of the given length index
 * @see as_longest()
 */
int mm_from_longest(int length_index);

/**
 * @param[out] lengths
 *     array of all possible lengths that a matchmaker word could have in ascending order
 * @param[out] count
 *     number of elements in the array
 * @see
 *     length_location()
 */
void mm_lengths(int const * * lengths, int * count);

/**
 * @param[in] length
 *     length of a word
 * @param[out] length_index
 *     words of given length begin at this "length index"
 * @param[out] count
 *     number of words with the given length
 * @returns
 *     true if any word has the given length,
 *     false otherwise for invalid input (in which case length_index and count remain unchanged)
 * @see lengths()
 */
bool mm_length_location(int length, int * length_index, int * count);


/**
 * @param[in] index
 *     index of a word in the dictionary
 * @param[out] pos
 *     pointer to an array of strings representing all parts of speech known to matchmaker
 * @param[out] flagged
 *     pointer to an array of booleans (stored as int8_t) such that the array's length is equal to the
 *     length of the array pointed to by pos
 *     elements are set to 1 if their cooresponding pos index pertains to the
 *     given word, or 0 otherwise.
 * @param[out] count
 *     number of array elements (same for both pos and flagged)
 * @returns true when index in range [0..mm_count() - 1] and false when index out of range
 */
bool mm_parts_of_speech(int index, char const * const * * pos, int8_t const * * flagged, int * count);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is a name, false otherwise
 */
bool mm_is_name(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is a male name, false otherwise
 */
bool mm_is_male_name(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is a female name, false otherwise
 */
bool mm_is_female_name(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is a place, false otherwise
 */
bool mm_is_place(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is a compound word, false otherwise
 */
bool mm_is_compound(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @returns
 *     true if the given word is an acronym, false otherwise
 */
bool mm_is_acronym(int index);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @param[out] synonyms
 *     pointer to an array of indexes to synonyms of the given word
 * @param[out] count
 *     number of synonyms in the array
 */
void mm_synonyms(int index, int const * * synonyms, int * count);

/**
 * @param[in] index
 *     index of a word in the dictionary
 * @param[out] antonyms
 *     pointer to an array of indexes to antonyms of the given word
 * @param[out] count
 *     number of antonyms in the array
 */
void mm_antonyms(int index, int const * * antonyms, int * count);

/**
 * @param[in] prefix
 *     prefix to complete
 * @param[out] start
 *     index of first word with the given prefix, or -1 if length is 0
 * @param[out] length
 *     number of words with the given prefix
 */
void mm_complete(char const * prefix, int * start, int * length);


#ifdef __cplusplus
}
#endif
