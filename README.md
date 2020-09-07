# MATCHMAKER
"Memory is neither created nor destroyed, it is simply compiled into existence."<br/>
--107<br/>

We give us an English dictionary with the following interface:<br>
```
namespace matchmaker
{
    /**
     * @returns
     *     The number of words in the dictionary
     */
    int size();

    /**
     * @param[in] index
     *     An index into an alphabetically sorted dictionary with 0 for the first word
     *     and size() - 1 for the last.
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
     * @param[in] index
     *     index of a word in the dictionary
     * @returns
     *     A "length index", which is the index the given word would have in a dictionary
     *     sorted from longest to shortest, with 0 for the longest word and size() - 1 for
     *     the shortest.
     * @see from_longest()
     */
    int as_longest(int index);

    /**
     * @param[in] length_index
     *     The index a word would have in a dictionary sorted from longest to shortest,
     *     with 0 for the longest word and size() - 1 for the shortest.
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

```


## clone and initialize
```
git clone https://github.com/shtroizel/matchmaker.git
cd matchmaker
git submodule update --init
```

## build and install
### options
```
scripts/build_and_install.py -h
```

### EXAMPLE: build q only (quick)
```
scripts/build_and_install.py -q
```

### EXAMPLE: entire dictionary
```
scripts/build_and_install.py
```

## additional notes
* "quick" builds are isolated and can be run in parallel with complete builds
* the "completable" repository depends on matchmaker via submodule so clone it instead if you want
  an example application along with the library
  - https://github.com/shtroizel/completable.git
