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
     *     valid indexes range from 0 to size() - 1 with 0 being the first word in the
     *     dictionary and size() - 1 the last
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
     *     the index of the given word if it exists,
     *     or the index that the given word would have if it did exist
     */
    int lookup(std::string const & word, bool * found);

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
     *     given word, or 0 otherwise. For performance and implementation details int8_t is used instead
     *     of bool. Specifically, the vectors already exist as MATCHABLE properties and are simply
     *     forwarded, meaning that this function is a constant time operation. MATCHABLE does not support
     *     properties of type bool, so using bool here would result in needless copying.
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
