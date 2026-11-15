#pragma once

#ifdef MATCHABLE_OMIT_BY_INDEX
matchable with by_index required for stage 0 reader
#endif

#include <matchable/matchable_fwd.h>
#include <matchable/matchable.h>

#include <functional>
#include <map>
#include <set>
#include <string>

#include <matchmaker/data_reader_common.h>
#include <matchmaker/parts_of_speech.h>

#include <Prefix.h>
#include <SerialTask.h>



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



    // ******* data **************

    std::string & data_dir();
    std::string & book_vocab_dir();
    std::string & output_dir();
    std::string & parents_filename();
    std::string & leaves_filename();

    dictionary_usage::Type & dictionary_mode();
    bool adds_words(SerialTask::Type task);

    // characters of each parent prefix, the root ("") included
    std::set<std::string> & parent_prefixes();

    struct TermLess
    {
        bool operator()(std::string const & l, std::string const & r) const
        {
            return matchable::str_lt_str(l, r);
        }
    };

    // leaves holding at least one word, keyed by their characters, in term order
    std::map<std::string, Prefix, TermLess> & leaves();

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

    // the leaf for str, created when missing only if create is true (else nullptr)
    Prefix * lookup(std::string const & str, bool create);

    void foreach_prefix(std::function<void (Prefix & p)> func);

    // ********************************
}
