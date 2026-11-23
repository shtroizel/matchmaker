#pragma once

#include <string>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
    #include <matchmaker_q/word_order.h>
#else
    #include <matchmaker/matchmaker.h>
    #include <matchmaker/word_order.h>
#endif



// the thesaurus readers' one case rule: a word means its exact spelling when the dictionary has it,
// otherwise its only case form; a word with several case forms and none exact means nothing, so it is
// skipped. Returns false for nothing, else sets the spelling and index it means
inline bool resolve_word(std::string const & word, std::string & spelling, int & index)
{
    if (word.empty())
        return false;
    bool found = false;
    index = mm_lookup(word.c_str(), &found);
    if (found)
    {
        spelling = word;
        return true;
    }
    int start = 0;
    int length = 0;
    mm_lookup_any_case(word.c_str(), &start, &length);
    if (length != 1)
        return false;
    index = start;
    spelling = mm_at(start, nullptr);
    return true;
}


// the 51155 thesaurus capitalizes the first word of each KEY, SYN and ANT line as typography, so that word
// means its lower case form, or, when the dictionary lacks that, its only case form
// (experiments/case_51155_2026-11-22)
inline bool resolve_line_start(std::string const & word, std::string & spelling, int & index)
{
    return resolve_word(mm_word_key(word), spelling, index);
}
