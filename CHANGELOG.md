# Changelog

All notable changes to matchmaker are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions follow
[Semantic Versioning](https://semver.org/spec/v2.0.0.html). The public interface is the mm_* C API in
matchmaker.h: a removed function or a changed signature is a major change. Each release is tagged vX.Y.Z.

## [Unreleased]

### Added
- scripts/build_toolchain.py builds the pinned toolchain, LLVM 23.1.6 (clang, lld, the LLVM libraries,
  libc++, libc++abi and libunwind), once per user into `~/.cache/matchmaker/toolchain/`: it downloads the
  source release (or takes `--tarball`), checks its sha256 and builds it with the system compiler
- `--toolchain=<LLVM install>` in build_and_install.py names another install of the pinned toolchain
- `-S` (`--system_compiler`) in build_and_install.py builds with the system compiler in place of the
  toolchain, linking with gold when it is installed; mini_system tests check its library against
  mini_library
- toolchain tests for build_toolchain.py, and mini_toolchain, which checks that the toolchain compiled and
  linked each library of a mini build
- `-e` (`--export`) in build_and_install.py exports every symbol and installs a type header per leaf in
  `include/matchmaker/leaf_types/`, so units of words compiled with MATCHABLE_UNLOADABLE can be loaded and
  unloaded at run time. Programs start slower with it
- mini_export and mini_export_units tests (Linux): a mini build with `-e`, then two units loaded, unloaded
  and swapped on top of it
- Builds warn when a library's loaded sections pass 1.5 GiB, well before the 2 GiB that 32-bit offsets
  reach, and when the largest leaf library's link needs more than half of the machine's memory
- link tests: the link pool calculation and the library size warning
- `--linker=<linker>` in build_and_install.py (`MM_LINKER` in CMake) picks the linker for every library:
  gold, lld, mold, `default` for the compiler's own, or another name the compiler takes as `-fuse-ld=`.
  The configure stops when the compiler can't link with it
- link_refuses_unknown and options_linker_needs_name tests
- `--leaf_budget=<words>` in build_and_install.py (default 170): the most words in a leaf
- `include/matchmaker/word_order.h`: mm_word_key() and mm_word_less(), matchmaker's word order, and
  mm_word_key_compare() and mm_word_compare(), which compare words where they are
- mini_library_check fails when mm_lookup(), mm_lookup_any_case(), mm_complete() or mm_at() allocates
- mm_lookup_any_case(term, start, length) returns every case form of a term as one range
- `-e` installs `include/matchmaker/leaf_types/leaf_list`, each leaf's name and first word in term order,
  so a unit can find the leaf a word belongs in
- leaf_cuts and generator refusal tests, routing over leaf first words, and `--by-spelling` and
  `--no-checks` in mini_library_check with order tests (test/order.py) for a later sort order change
- mini_moved tests: stage 1 run again on a mini build with book text moved compiles only the files of the
  generated arrays that hold the moved posts and their terms' locations, the library shows the text where
  it now is, and the fixture data brings the first library back
- `--check=<name>` in mini_library_check runs the one check named, so a check that finishes on a full-size
  library can run there alone; each check reports its problem count on standard error, and
  mini_library_one_check and mini_library_unknown_check test the option
- `--array_budget=<ints>` in build_and_install.py (default 1000000): the most ints in a compiled file of
  the generated arrays. The stage 1 reader cuts its arrays to fit, so each file compiles within bounded
  memory however many books there are. Arrays read an entry at a time come in units of a power of two of
  entries, locations in blocks of a power of two of terms, and a book's chapters in groups of a power of
  two of chapters that the book's own sizes decide; a term or a chapter over the budget gets a file to
  itself
- array_cuts tests for the cut rule, mini_array_units for the cut of a mini build, and mini_recut tests:
  stage 1 run again with another budget compiles the generated arrays again and no leaf, and the library
  answers as before
- relations_at_load test for the indexes computed from references, mini_added tests (a mini build from
  the fixture data with one more word has the same header for every leaf but the word's own, and every
  link on the terms where they now are), mini_exports for what the libraries export, and leaf_type for
  the type header `-e` installs

### Changed
- Leaves hold each word's synonyms, antonyms, embedded terms and definition as references to words
  (matchable::VariantRef) that the linker resolves by name, instead of alphabetic indexes. An added word
  moves the index of every term after it, and a leaf's references stay as they were, so its header
  changes only when its own words or the words it links to do. Each leaf defines the instances of its
  words with protected visibility, the leaf libraries export the instances' addresses beside the mm_*
  functions, and a leaf declares the words of other leaves it refers to with MATCHABLE_EXTERN_VARIANTS.
  The mm_* answers are unchanged
- mm_synonyms(), mm_antonyms(), mm_embedded() and mm_definition() are O(1): they read one table per kind
  of link, indexed by term, which the first of these calls builds from the leaves' references
- Builds use the pinned toolchain for matchable, the data readers and both stages unless `-S` is given,
  building it first when it isn't there yet. libc++, libc++abi and libunwind are linked into each library, so an
  install loads into programs built with any compiler. Libraries link with lld unless `--linker` names
  another. Toolchain builds need cmake 3.21 or newer and ninja. The tests keep the system compiler, and
  mini_export_units builds its units with the toolchain
- Running stage 1 again (`-r -f`) compiles only the generated files whose content changed: files it
  writes with the same content keep their earlier modification times
- `--time_compiles=<file>` works with `-r` and `-l`: a kept build logs to the file given, and changing the
  file no longer compiles everything again
- Requires matchable 16.0.0 (unreleased: matchable's dev branch at 38634fb), which compares
  names as unsigned bytes, lets each leaf declare its sort key, lets matchmaker search a leaf's names
  with its own comparison, and lets a leaf define the instances of its words and refer to the words of
  other leaves
- mm_lookup(), mm_lookup_any_case() and mm_complete() search the leaves' first words and then one leaf's
  names, comparing words where they are: none of them allocates, and mm_complete() takes O(log N) string
  comparisons however many terms match. Each leaf gives its names table (mm_names_<leaf>) instead of a
  lookup function
- Words sort in matchmaker's own order instead of matchable's: by the word with its ASCII letters
  lowercased, then by the word itself, so a word's case forms sit together, capitals first (Apple, apple,
  apply). Symbols and digits no longer all sort before letters. Every leaf declares the order with
  MATCHABLE_SORTKEY, and the leaf type headers installed with `-e` carry it too. Alphabetic indexes
  differ from earlier builds
- mm_complete() ignores case: "ab" and "AB" both complete to Abbey, abbey and ABBA. Its signature is
  unchanged. mm_lookup() still matches case exactly
- The thesaurus readers resolve a word's case with one rule: its exact spelling when the dictionary has
  it, otherwise its only case form, otherwise it is skipped. 51155, a scanned book with stray capitals,
  is read in lower case first, so a proper noun with no lower case form, such as Croesus, keeps its synonyms
- Leaves are ranges of the sorted words instead of prefix tree nodes. The stage 0 reader cuts the words
  by content: a leaf ends before a word whose hash is 0 mod 60 once it holds 20 words, and always at the
  budget, so the same words always give the same leaves and an added word usually changes only its own
  leaf. Leaves are named by up to 4 bytes of their first word and 12 hex digits of its hash, and lookups
  binary search the leaves' first words. The reader still sorts words into the parent prefix buckets while
  reading. Term indexes and mm_* answers are unchanged
- The leaves go into 4 libraries instead of 16 (`LEAFLIB_COUNT`); each holds about a quarter of a
  complete build
- Every library links with gold where it is available (except on sparc64), the leaf libraries too; gold is
  no longer passed on to programs that link matchmaker
- build_and_install.py builds with Ninja when it is installed, and Ninja links only as many leaf libraries
  at once as fit in half of the machine's memory, estimated from the size of each library's generated
  headers. A build directory keeps the generator it was made with. The estimate follows the linker: lld
  and mold need less memory than gold
- The longest order, book locations, book text and which books use each term are plain arrays of int:
  the stage 1 reader writes them into the workspace (`generated_src/arrays`) as text and libmatchmaker
  compiles them in. The leaves no longer hold a longest-order index, book locations or book flags, so
  moved book text compiles no leaf, and books are no longer C++ generated from templates. The large
  arrays are compiled in several files (`--array_budget`), and a lookup finds its file with a shift.
  mm_as_longest(), mm_locations() and mm_is_used_in_book() are O(1). The mm_* answers are unchanged

### Removed
- The README's supported systems list, until builds with the pinned toolchain are tested on them
- `-c` (`--clang`) in build_and_install.py: the pinned toolchain builds every library by default, and
  `-S` with `CC` and `CXX` in the environment picks a system compiler

### Fixed
- The stage 1 reader read one element before the start of its longest order while finding the terms of
  each length
- mm_is_used_in_book() is true for every word of a book's text and every phrase around one. It answered
  from the stage 0 reader's vocabulary alone, so a term could have locations in a book that didn't use
  it: a word inside a longer token, such as bark in "4bark", or a dictionary phrase such as ice cream. The
  vocabulary's marks stay, so no term loses one. The fixture has a post with both, and the locations
  check of mini_library_check expects a book to use each phrase around its words

## [4.1.0] - 2026-11-19

### Added
- `--dictionary=<all|reduced|none>` in build_and_install.py chooses which dictionary sources add words,
  replacing the q usage modes. `-q` is still short for `--dictionary=none --books=Crumbs`
- `--books=<books>` chooses which books are included, independently of the dictionary sources
- `--data_dir=<dir>` builds from other data with the same layout as data_reader/data
- `--time_compiles=<file>` logs each compile's time and peak memory (Linux and macOS)
- `-s` (`--skip_tests`) skips the tests build_and_install.py now runs before the stage 0 data reader
- A ctest suite in test/: generator and routing checks, data reader fixtures for each source, a stage 0
  run over the full data, and a mini build of a whole library from fixture data
- CHANGELOG.md

### Changed
- Requires matchable 15.0.0
- Prefixes cover every printable symbol explicitly, so symbols sort the way matchable compares strings.
  Term indexes for symbols differ from 4.0.0 libraries
- The prefix tree is defined once, in scripts/parent_prefixes.txt, and walked with generated byte tables
  instead of parent matchables
- Only the leaves that hold words are generated and compiled, which removes most leaf compiles
- The libraries export only the mm_* API, so the loader resolves far fewer relocations at startup
- mm_complete finds the end of a completion by search instead of visiting every match
- Leaf terms are read by reference instead of through a copied matchable handle
- Parts of speech are merged across sources in stage 0, so words keep the flags from 3203
- Annotate-only sources run after the sources that add words, so their marks are kept
- Sub-builds of matchable and the data readers live under the -b build directories, and -j reaches every
  step
- matchmaker.h states the real complexities (O(log N) for functions taking an index) and documents that
  mm_complete's start is where the prefix would be, possibly mm_count(), when nothing matches. The
  mm_definition parameter `embedded` is named `definition`

### Fixed
- mm_from_ordinal_summation and mm_parts_of_speech return empty results for out-of-range arguments
- The stage 0 reader keeps the last word of each 3201 file
- The stage 0 reader adds the Crumbs title and author, and the names the saving adds, in every mode
- Both data readers convert Crumbs post hours to 24-hour time the same way: 00 to 23, zero padded
- The stage 1 reader reads each thesaurus line whole, so no word becomes a key with the rest of a line as
  its synonyms
- The stage 0 reader refuses parent prefixes deeper than 5 and no longer divides by zero in progress for
  small inputs, and neither does the stage 1 reader
- Long options in the build scripts take their values (`--build_dir=x`, `--jobs=4`), and the leftover
  `-m` option is refused
