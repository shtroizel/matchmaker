#pragma once

#ifdef MATCHABLE_OMIT_BY_INDEX
matchable with by_index required for stage 0 reader
#endif

#include <matchable/matchable_fwd.h>
#include <matchable/matchable.h>

#include <functional>

#include <matchmaker/data_reader_common.h>
#include <matchmaker/parts_of_speech.h>

#include <Prefix.h>
#include <SerialTask.h>



int const DIM = 95;
using index_t = uint16_t;

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

    q_usage::Type & q_mode();

    std::vector<Prefix> & prefixes_2d_to_5d();

    // A 6D lookup table would require too much RAM,
    // so the 6th dimension is handled separately
    std::vector<Prefix> & prefixes_6d();

    // lookup table for constant lookup of indexes into prefixes_2d_to_5d
    using LookupTable =
            std::array<std::array<std::array<std::array<std::array<index_t, DIM>, DIM>, DIM>, DIM>, DIM>;
    LookupTable & lookup_table_2d_to_5d();

    // ***************************







    // ******* functions **************

    void update_word_attribute(word_attribute::Flags & flags, int ch);

    void add_book_vocab(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task,
        FILE * vocab_file,
        std::map<std::string, Encountered::Type> & encounters
    );

    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task
    );

    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task,
        parts_of_speech::Flags const & pos_flags
    );

    Prefix * prefix_for_d1_symbol(char sym);

    index_t calc_index(char ch);

    Prefix * lookup(std::string const & str);

    void foreach_prefix(std::function<void (Prefix & p)> func);

    // ********************************
}
