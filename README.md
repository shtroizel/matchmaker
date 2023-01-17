# MATCHMAKER
matchmaker is a C library of compiled english-language words, phrases and books<br>
<br>
* matchmaker provides a large array of elements referred to here as "terms"<br>
* terms may be words, compound words, entire phrases, or a single symbol like "^"<br>
* terms are referenced by index, which is analogous to how a normal array works<br>
* the term, "term", is used loosly here for both the index and the word or phrase obtainable from it<br>
* there are mm_count() terms accessible with indexes in the range of 0..(mm_count() - 1)<br>
* mm_lookup() will get you an index from a string<br>
* mm_at() will get you a string from an index<br>
* all strings used by matchmaker are ascii null-terminated byte-strings<br>
* all strings returned by mm_at() are static<br>
* actually, everything retrieved by matchmaker is static<br>
* all matchmaker functions avoid dynamic memory allocation - always.<br>
* deleting matchmaker data will result in self-inflicted pain and suffering (avoidance recommended)<br>
<br>
* books are made up of dictionary terms (as indexes) organized within paragraphs and chapters<br>
* like dictionary terms, books are also referenced by index<br>
* there are mm_book_count() books accessible with book indexes in the range of 0..(mm_book_count() - 1)<br>
* books contain chapters that are accessed in a similar way<br>
* chapters contain paragraphs that are accessed in a similar way<br>
* finally, paragraphs contain words that are accessed in a similar way<br>
* indexes for book, chapter, paragraph and word can be used to retrieve dictionary indexes (terms)<br>
<br>
* terms are smart and know things like who their synonyms are<br>
* terms that are compound words or phrases know who their embedded words are<br>
* terms sometimes know their parts of speech<br>
* terms always know every location within every book that they are used in<br>
* all of this data including book locations is static and accessible in O(1) time<br>
<br>
* matchmaker avoids exposing any custom data types in its interface<br>
* matchmaker does just fine using the built in types only (int, char, etc)<br>
* matchmaker only has one header file: <machmaker/matchmaker.h><br>
* matchmaker.h is where to go to learn about all the cool matchmaker functions<br>
https://github.com/shtroizel/matchmaker/blob/master/include/matchmaker/matchmaker.h <br>

## supported systems
* gentoo on sparc64<br>
* artix on amd64<br>
* matchmaker might build on other linuxes or other linux-like operating systems.<br>

## dependencies
the matchmaker library relies only on standard C/C++ libraries for its implementation<br>
for building however, matchmaker requires both cmake and python<br>

## clone and initialize
* remember to initialize submodules before building
* the "completable" project depends on matchmaker via submodule so clone it instead if you want
  an example application along with the library
  - https://github.com/shtroizel/completable.git
### example: matchmaker only
```
git clone https://github.com/shtroizel/matchmaker.git
cd matchmaker
git submodule update --init --recursive
```
### example: matchmaker with completable
```
git clone https://github.com/shtroizel/completable.git
cd completable
git submodule update --init --recursive
cd matchmaker
```

## build and install
building matchmaker requires patience<br>
building matchmaker is a good way to test your hardware<br>
the build process is split into two stages - the library must be built twice<br>
after the first stage all the terms are there but nothing else works<br>
in the second stage, the first stage is used to patch its code before recompiling<br>
the build_and_install.py script handles all of this including several layers of code generation<br>

### quick build option
"quick" builds are faster but the resulting library will contain limited vocabulary<br>
"quick" builds may be installed to the same prefix as complete builds<br>
"quick" builds use "_q" suffixes and install to lib/matchmaker_q and include/matchmaker_q<br>

### options
```
scripts/build_and_install.py -h
```

### example: "quick" build
```
scripts/build_and_install.py -q
```

### example: entire dictionary
```
scripts/build_and_install.py
```

### example: entire dictionary w/ custom build & install locations
```
scripts/build_and_install.py -b build_2020-12-08 -i ../install_2020-12-08
```

## using matchmaker in your cmake project
just use find_package() like completable does:<br>
https://github.com/shtroizel/completable/blob/master/CMakeLists.txt <br>

## current progress and direction
* matchmaker currently supports several hundreds of thousands of symbols, words and phrases<br>
* support for books is new and experimental - the first book has just been added<br>
* recent addition of symbol-littered phrases exposed some lookup issues that have now been fixed<br>
* matchmaker currently uses poor and incomplete raw input data from free sources containing some misspelled words<br>
* matchmaker needs its own defined raw input data format as a basis for many fixes to the poor data problem<br>
* the scalability of book support is unknown - more need to be added to see how things break<br>

## donating to this project
donations are greatly appreciated, thank you!<br>
* monero: 4BETuKtvLjkT7VBW85HLJ1XPqhr1TqQj11UmEMnphgG4DzNtSgJmK8b3ZmTudnbbjf7oHfNpCPxDG3BWnmVSCrtrNXMnhyv<br>
