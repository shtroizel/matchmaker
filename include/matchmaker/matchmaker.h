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


#include <string>
#include <vector>


namespace matchmaker
{
    /**
     * @returns
     *     The number of words in the dictionary
     */
    int size();

    /**
     * @param[in] index
     *     An index into an alphabetically sorted dictionary with 0 for the first word and size() - 1 for
     *     the last.
     * @returns
     *     The word at the given index, or empty string when index is out of range
     */
    std::string const & at(int index);

    /**
     * @param[in] word
     *     string to look up in the dictionary
     * @param[out] found
     *     true if word is found, false otherwise
     *     ignored when nullptr
     * @returns
     *     the alphabetic index of the given word if it exists,
     *     or the index that the given word would have if it did exist
     */
    int lookup(std::string const & word, bool * found);

    /**
     * DEPRECATED
     *     use from_longest(0) instead
     *
     * @returns
     *     The index of the longest word in the dictionary
     */
    int longest_word();

    /**
     * DEPRECATED
     *     use from_longest() intead
     *
     * @returns
     *     A vector of all words (as indexes) sorted by length (# of letters) such that the first is the
     *     longest and the last is the shortest. Since the calculation is done at build time, this is a
     *     constant time operation (just retrieves the vector that already exists).
     */
    std::vector<int> const & by_longest();

    /**
     * @param[in] index
     *     index of a word in the dictionary
     * @returns
     *     A "length index", which is the index the given word would have in a dictionary sorted from
     *     longest to shortest, with 0 for the longest word and size() - 1 for the shortest.
     * @see from_longest()
     */
    int as_longest(int index);

    /**
     * @param[in] length_index
     *     The index a word would have in a dictionary sorted from longest to shortest, with 0 for the
     *     longest word and size() - 1 for the shortest.
     * @returns
     *     An alphabetic index for the word of the given length index
     * @see as_longest()
     */
    int from_longest(int length_index);

    /**
     * @param[in] length
     *     length of a word
     * @param[out] length_index
     *     words of given length begin at this "length index"
     * @param[out] count
     *     number of words with the given length
     * @returns
     *     true if any word has the given length,
     *     false otherwise for invalid input (in which case index and count remain unchanged)
     * @see lengths()
     */
    bool length_location(std::size_t length, int & length_index, int & count);

    /**
     * @returns
     *     all possible lengths that a matchmaker word could have in ascending order
     * @see
     *     length_location()
     */
    std::vector<std::size_t> const & lengths();

    /**
     * @returns
     *     A vector containing all parts of speech known to matchmaker
     */
    std::vector<std::string> const & all_parts_of_speech();

    /**
     * @param[in] index
     *     index of a word in the dictionary
     * @returns
     *     A vector of booleans such that the vector's size is equal to all_parts_of_speech.size().
     *     Elements are set to 1 if their cooresponding all_parts_of_speech() index pertains to the
     *     given word, or 0 otherwise.
     * @see all_parts_of_speech()
     */
    std::vector<int8_t> const & flagged_parts_of_speech(int index);

    /**
     * @param[in] index
     *     index of a word in the dictionary
     * @param[out] pos
     *     A vector containing the parts of speech associated with the given word.
     *     Pointed to strings are static (avoids allocation / destruction)
     */
    void parts_of_speech(int index, std::vector<std::string const *> & pos);

    /**
     * @param[in] index
     *     index of a word in the dictionary
     * @returns
     *     vector of indexes of synonyms
     */
    std::vector<int> const & synonyms(int index);

    /**
     * @param[in] index
     *     index of a word in the dictionary
     * @returns
     *     vector of indexes of antonyms
     */
    std::vector<int> const & antonyms(int index);

    /**
     * @param[in] prefix
     *     prefix to complete
     * @param[out] start
     *     index of first word with the given prefix, or -1 if length is 0
     * @param[out] length
     *     number of words with the given prefix
     */
    void complete(std::string const & prefix, int & start, int & length);
}
