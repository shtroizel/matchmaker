#pragma once

#include <string>



// matchmaker's word order: words compare by their key, the word with its ASCII letters lowercased, as
// unsigned bytes, then by the word itself, so a word's case forms sit together (capitals first) and
// completion can ignore case. Every leaf declares it with MATCHABLE_SORTKEY(word_<leaf>, mm_word_key)

inline std::string mm_word_key(std::string const & word)
{
    std::string key{word};
    for (char & c : key)
        if (c >= 'A' && c <= 'Z')
            c = (char) (c - 'A' + 'a');
    return key;
}


inline bool mm_word_less(std::string const & l, std::string const & r)
{
    std::string const l_key = mm_word_key(l);
    std::string const r_key = mm_word_key(r);
    if (l_key != r_key)
        return l_key < r_key;
    return l < r;
}
