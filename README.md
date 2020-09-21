# MATCHMAKER

The English Dictionary C++ Library<br>

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
* the "completable" projet depends on matchmaker via submodule so clone it instead if you want
  an example application along with the library
  - https://github.com/shtroizel/completable.git
* the entire interface can be found here: https://github.com/shtroizel/matchmaker/blob/master/include/matchmaker/matchmaker.h
