# matchmaker

git clone https://github.com/shtroizel/matchmaker.git

cd matchmaker

git submodule update --init --recursive

scripts/build_and_install.py -h


## get started quickly
scripts/build_and_install.py -q

scripts/build_and_install_test_program.py

test_program/install/bin/test_program

## test program command line
* words separated by spaces will be looked up
* single string starting with ':' will be completed (ex: ':quiet' will show all words starting with 'quiet')
* ':abc' will list the entire dictionary
