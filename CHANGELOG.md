# Changelog

All notable changes to matchmaker are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions follow
[Semantic Versioning](https://semver.org/spec/v2.0.0.html). The public interface is the mm_* C API in
matchmaker.h: a removed function or a changed signature is a major change. Each release is tagged vX.Y.Z.

## [Unreleased]

### Added
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
  The configure stops when the compiler can't link with it. Without it, gold is used when installed
- link_refuses_unknown and options_linker_needs_name tests, and mini_lld tests (when the compiler can
  link with lld): a mini build with `--linker=lld`, checked for lld in every library and against
  mini_library
- `--leaf_budget=<words>` in build_and_install.py (default 170): the most words in a leaf
- `include/matchmaker/word_order.h`: mm_word_key() and mm_word_less(), matchmaker's word order
- mm_lookup_any_case(term, start, length) returns every case form of a term as one range
- `-e` installs `include/matchmaker/leaf_types/leaf_list`, each leaf's name and first word in term order,
  so a unit can find the leaf a word belongs in
- leaf_cuts and generator refusal tests, routing over leaf first words, and `--by-spelling` and
  `--no-checks` in mini_library_check with order tests (test/order.py) for a later sort order change

### Changed
- Running stage 1 again (`-r -f`) compiles only the generated files whose content changed: files it
  writes with the same content keep their earlier modification times
- `--time_compiles=<file>` works with `-r` and `-l`: a kept build logs to the file given, and changing the
  file no longer compiles everything again
- Requires matchable 16.0.0 (unreleased: matchable's dev branch at d085397), which compares names as
  unsigned bytes and lets each leaf declare its sort key
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

### Fixed
- The `--clang` help no longer says the clang build is broken; it builds and passes the mini build checks

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
