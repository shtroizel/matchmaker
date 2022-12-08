#pragma once

#ifdef MATCHABLE_OMIT_BY_INDEX
matchable with by_index required for stage 0 reader
#endif

#include <matchable/matchable_fwd.h>
#include <matchable/matchable.h>

#include <functional>

#include <matchmaker/parts_of_speech.h>

#include <Prefix.h>



namespace Stage0Data
{
    MATCHABLE(Encountered, Yes)

    MATCHABLE(
        word_attribute,
        invisible_ascii,
        matchable_symbols,
        unmatchable_symbols,
        name,
        male_name,
        female_name,
        place,
        compound,
        acronym,
        phrase
    )

    int const MAX_PREFIX_DEPTH{6};


    // ******* data **************

    std::string & data_dir();
    std::string & book_vocab_dir();
    std::string & output_dir();
    std::string & prefixes_filename();

    // All prefixes with a depth of 1 are symbols
    std::array<Prefix, 23> & symbols_1d_prefixes();

    // All prefixes with a depth > 1 are letters
    std::vector<Prefix> & prefixes_2d_to_5d();

    // A 6D lookup table would require too much RAM,
    // so the 6th dimension is handled separately
    std::vector<Prefix> & prefixes_6d();

    // lookup table for constant lookup of indexes into prefixes_2d_to_5d
    using LookupTable = std::array<std::array<std::array<std::array<std::array<int16_t, 52>, 52>, 52>, 52>, 52>;
    LookupTable & lookup_table_2d_to_5d();

    // ***************************







    // ******* functions **************

    void update_word_attribute(word_attribute::Flags & flags, int ch);

    void add_book_vocab(
        std::string const & word,
        word_attribute::Flags const & wsf,
        FILE * vocab_file,
        std::map<std::string, Encountered::Type> & encounters
    );

    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf
    );

    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf,
        parts_of_speech::Flags const & pos_flags
    );

    Prefix * prefix_for_d1_symbol(char sym);

    int16_t calc_letter_index(char ch);

    Prefix * lookup(std::string const & str);

    void foreach_prefix(std::function<void (Prefix & p)> func);

    // ********************************
}
