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
}
```


## clone and initialize
```
git clone https://github.com/shtroizel/matchmaker.git
cd matchmaker
git submodule update --init --recursive
```

## building matchmaker
```
scripts/build_and_install.py -h
```

## building completable
completable is an application that uses the matchmaker library
```
completable/scripts/build_and_install.py -h
```

## quick start example
```
scripts/build_and_install.py -q
completable/scripts/build_and_install.py -q
completable/install_q/bin/completable
```
