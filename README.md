# matchmaker
matchmaker tests the limits of compilers/linkers and MATCHABLE by compiling the dictionary<br/><br/>
the result is a library with the following interface<br>
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

### EXAMPLE: entire dictionary limiting memory usage to 17 GB
```
scripts/build_and_install.py -l 17
```

## additional notes
* "quick" builds are isolated and can be run in parallel with complete builds
* the "completable" repository depends on matchmaker via submodule so clone it instead if you want
  an example application along with the library
