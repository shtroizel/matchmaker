#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>



// how the stage 0 reader cuts the words, in term order, into leaves, with nothing stored: a leaf ends
// before a word whose hash is 0 mod the divisor once it holds the minimum, and always at the budget, so the
// same words always give the same leaves and an added word usually changes only its own leaf
// (experiments/leaf_cuts_cdc_2026-11-22)
namespace leaf_cuts
{
    inline uint64_t fnv1a(std::string const & s)
    {
        uint64_t h = 0xcbf29ce484222325ull;
        for (unsigned char c : s)
        {
            h ^= c;
            h *= 0x100000001b3ull;
        }
        return h;
    }


    struct Rule
    {
        int budget;
        int divisor;
        int min_size;
    };


    // the divisor and minimum scale with the budget: 60 and 20 at the default budget of 170
    inline Rule rule(int budget)
    {
        return {budget, std::max(1, (budget * 6 + 8) / 17), std::max(1, (budget * 2 + 8) / 17)};
    }


    inline bool is_boundary(std::string const & word, Rule const & r)
    {
        return fnv1a(word) % (uint64_t) r.divisor == 0;
    }


    // the index of each leaf's first word
    inline std::vector<size_t> starts(std::vector<std::string> const & words, Rule const & r)
    {
        std::vector<size_t> ret;
        int size = 0;
        for (size_t i = 0; i < words.size(); ++i)
        {
            if (ret.empty() || size >= r.budget || (size >= r.min_size && is_boundary(words[i], r)))
            {
                ret.push_back(i);
                size = 0;
            }
            ++size;
        }
        return ret;
    }


    // a leaf's name: up to 4 bytes of its first word, letters and digits as they are and other bytes as x
    // and two hex digits, then an underscore and 12 hex digits of the first word's hash
    inline std::string name(std::string const & first_word)
    {
        std::string ret;
        char buf[17];
        for (size_t i = 0; i < first_word.size() && i < 4; ++i)
        {
            unsigned char const c = first_word[i];
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            {
                ret += (char) c;
            }
            else
            {
                std::snprintf(buf, sizeof(buf), "x%02x", c);
                ret += buf;
            }
        }
        unsigned long long const hash = fnv1a(first_word) & 0xffffffffffffull;
        std::snprintf(buf, sizeof(buf), "%012llx", hash);
        return ret + "_" + buf;
    }
}
