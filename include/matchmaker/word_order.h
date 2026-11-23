#pragma once

#include <cstddef>
#include <cstring>
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


// compares the keys of two words where they are, without building the keys: negative when l's key is
// first, 0 when the keys are the same, positive when r's key is first
inline int mm_word_key_compare(char const * l, size_t l_length, char const * r, size_t r_length)
{
    size_t const length = l_length < r_length ? l_length : r_length;
    for (size_t i = 0; i < length; ++i)
    {
        unsigned char lc = (unsigned char) l[i];
        unsigned char rc = (unsigned char) r[i];
        if (lc >= 'A' && lc <= 'Z')
            lc = (unsigned char) (lc - 'A' + 'a');
        if (rc >= 'A' && rc <= 'Z')
            rc = (unsigned char) (rc - 'A' + 'a');
        if (lc != rc)
            return lc < rc ? -1 : 1;
    }
    return l_length < r_length ? -1 : (l_length > r_length ? 1 : 0);
}


// compares two words where they are, by key and then by the words themselves: negative when l is first,
// 0 when they are the same word, positive when r is first
inline int mm_word_compare(char const * l, size_t l_length, char const * r, size_t r_length)
{
    int const keys = mm_word_key_compare(l, l_length, r, r_length);
    // the same key means the same length
    return keys != 0 ? keys : std::memcmp(l, r, l_length);
}


inline bool mm_word_less(std::string const & l, std::string const & r)
{
    return mm_word_compare(l.data(), l.size(), r.data(), r.size()) < 0;
}
