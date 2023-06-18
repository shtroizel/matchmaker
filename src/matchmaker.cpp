#include <matchmaker/matchmaker.h>

#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>

#include <matchmaker/generated_symbols/_spc_/_spc_.h>
#include <matchmaker/generated_symbols/_bng_/_bng_.h>
#include <matchmaker/generated_symbols/_quot_/_quot_.h>
#include <matchmaker/generated_symbols/_hsh_/_hsh_.h>
#include <matchmaker/generated_symbols/_dol_/_dol_.h>
#include <matchmaker/generated_symbols/_pct_/_pct_.h>
#include <matchmaker/generated_symbols/_und_/_und_.h>
#include <matchmaker/generated_symbols/_sqt_/_sqt_.h>
#include <matchmaker/generated_symbols/_parl_/_parl_.h>
#include <matchmaker/generated_symbols/_parr_/_parr_.h>
#include <matchmaker/generated_symbols/_ast_/_ast_.h>
#include <matchmaker/generated_symbols/_plus_/_plus_.h>
#include <matchmaker/generated_symbols/_cma_/_cma_.h>
#include <matchmaker/generated_symbols/_mns_/_mns_.h>
#include <matchmaker/generated_symbols/_dot_/_dot_.h>
#include <matchmaker/generated_symbols/_slsh_/_slsh_.h>
#include <matchmaker/generated_symbols/esc_0/esc_0.h>
#include <matchmaker/generated_symbols/esc_1/esc_1.h>
#include <matchmaker/generated_symbols/esc_2/esc_2.h>
#include <matchmaker/generated_symbols/esc_3/esc_3.h>
#include <matchmaker/generated_symbols/esc_4/esc_4.h>
#include <matchmaker/generated_symbols/esc_5/esc_5.h>
#include <matchmaker/generated_symbols/esc_6/esc_6.h>
#include <matchmaker/generated_symbols/esc_7/esc_7.h>
#include <matchmaker/generated_symbols/esc_8/esc_8.h>
#include <matchmaker/generated_symbols/esc_9/esc_9.h>
#include <matchmaker/generated_symbols/_cln_/_cln_.h>
#include <matchmaker/generated_symbols/_scln_/_scln_.h>
#include <matchmaker/generated_symbols/_lt_/_lt_.h>
#include <matchmaker/generated_symbols/_eq_/_eq_.h>
#include <matchmaker/generated_symbols/_gt_/_gt_.h>
#include <matchmaker/generated_symbols/_qstn_/_qstn_.h>
#include <matchmaker/generated_symbols/_atsym_/_atsym_.h>
#include <matchmaker/generated_symbols/_sbl_/_sbl_.h>
#include <matchmaker/generated_symbols/_bslsh_/_bslsh_.h>
#include <matchmaker/generated_symbols/_sbr_/_sbr_.h>
#include <matchmaker/generated_symbols/_dach_/_dach_.h>
#include <matchmaker/generated_symbols/_/_.h>
#include <matchmaker/generated_symbols/_bqt_/_bqt_.h>
#include <matchmaker/generated_symbols/_cbl_/_cbl_.h>
#include <matchmaker/generated_symbols/_pip_/_pip_.h>
#include <matchmaker/generated_symbols/_cbr_/_cbr_.h>
#include <matchmaker/generated_symbols/_tld_/_tld_.h>
#include <matchmaker/generated_symbols/esc_A/esc_A.h>
#include <matchmaker/generated_symbols/esc_B/esc_B.h>
#include <matchmaker/generated_symbols/esc_C/esc_C.h>
#include <matchmaker/generated_symbols/esc_D/esc_D.h>
#include <matchmaker/generated_symbols/esc_E/esc_E.h>
#include <matchmaker/generated_symbols/esc_F/esc_F.h>
#include <matchmaker/generated_symbols/esc_G/esc_G.h>
#include <matchmaker/generated_symbols/esc_H/esc_H.h>
#include <matchmaker/generated_symbols/esc_I/esc_I.h>
#include <matchmaker/generated_symbols/esc_J/esc_J.h>
#include <matchmaker/generated_symbols/esc_K/esc_K.h>
#include <matchmaker/generated_symbols/esc_L/esc_L.h>
#include <matchmaker/generated_symbols/esc_M/esc_M.h>
#include <matchmaker/generated_symbols/esc_N/esc_N.h>
#include <matchmaker/generated_symbols/esc_O/esc_O.h>
#include <matchmaker/generated_symbols/esc_P/esc_P.h>
#include <matchmaker/generated_symbols/esc_Q/esc_Q.h>
#include <matchmaker/generated_symbols/esc_R/esc_R.h>
#include <matchmaker/generated_symbols/esc_S/esc_S.h>
#include <matchmaker/generated_symbols/esc_T/esc_T.h>
#include <matchmaker/generated_symbols/esc_U/esc_U.h>
#include <matchmaker/generated_symbols/esc_V/esc_V.h>
#include <matchmaker/generated_symbols/esc_W/esc_W.h>
#include <matchmaker/generated_symbols/esc_X/esc_X.h>
#include <matchmaker/generated_symbols/esc_Y/esc_Y.h>
#include <matchmaker/generated_symbols/esc_Z/esc_Z.h>
#include <matchmaker/generated_symbols/a/a.h>
#include <matchmaker/generated_symbols/b/b.h>
#include <matchmaker/generated_symbols/c/c.h>
#include <matchmaker/generated_symbols/d/d.h>
#include <matchmaker/generated_symbols/e/e.h>
#include <matchmaker/generated_symbols/f/f.h>
#include <matchmaker/generated_symbols/g/g.h>
#include <matchmaker/generated_symbols/h/h.h>
#include <matchmaker/generated_symbols/i/i.h>
#include <matchmaker/generated_symbols/j/j.h>
#include <matchmaker/generated_symbols/k/k.h>
#include <matchmaker/generated_symbols/l/l.h>
#include <matchmaker/generated_symbols/m/m.h>
#include <matchmaker/generated_symbols/n/n.h>
#include <matchmaker/generated_symbols/o/o.h>
#include <matchmaker/generated_symbols/p/p.h>
#include <matchmaker/generated_symbols/q/q.h>
#include <matchmaker/generated_symbols/r/r.h>
#include <matchmaker/generated_symbols/s/s.h>
#include <matchmaker/generated_symbols/t/t.h>
#include <matchmaker/generated_symbols/u/u.h>
#include <matchmaker/generated_symbols/v/v.h>
#include <matchmaker/generated_symbols/w/w.h>
#include <matchmaker/generated_symbols/x/x.h>
#include <matchmaker/generated_symbols/y/y.h>
#include <matchmaker/generated_symbols/z/z.h>


#include <matchmaker/parts_of_speech.h>
#include <matchmaker/longest_words.h>
#include <matchmaker/ordinal_summation.h>


#ifndef STAGE0
#include <matchmaker/books/books_content.h>
#endif


using count_func = std::function<int ()>;
using as_longest_func = std::function<int (int)>;
using at_func = std::function<std::string const & (int)>;
using lookup_func = std::function<int (std::string const &, bool *)>;
using ordinal_summation_func = std::function<int (int)>;
using flagged_parts_of_speech_func = std::function<std::vector<int8_t> const & (int)>;
using synonyms_func = std::function<std::vector<int> const & (int)>;
using antonyms_func = std::function<std::vector<int> const & (int)>;
using embedded_func = std::function<std::vector<int> const & (int)>;
using definition_func = std::function<std::vector<int> const & (int)>;
using is_name_func = std::function<bool (int)>;
using is_male_name_func = std::function<bool (int)>;
using is_female_name_func = std::function<bool (int)>;
using is_place_func = std::function<bool (int)>;
using is_compound_func = std::function<bool (int)>;
using is_acronym_func = std::function<bool (int)>;
using is_phrase_func = std::function<bool (int)>;
using is_used_in_book_func = std::function<bool (int, int)>;
using locations_func = std::function<void (int,
                                           int const * *,
                                           int const * *,
                                           int const * *,
                                           int const * *,
                                           int *)>;

PROPERTYx19_MATCHABLE(
    // properties
    count_func, count,
    as_longest_func, as_longest,
    at_func, at,
    lookup_func, lookup,
    ordinal_summation_func, ordinal_summation,
    flagged_parts_of_speech_func, flagged_parts_of_speech,
    synonyms_func, synonyms,
    antonyms_func, antonyms,
    embedded_func, embedded,
    definition_func, definition,
    locations_func, locations,
    is_name_func, is_name,
    is_male_name_func, is_male_name,
    is_female_name_func, is_female_name,
    is_place_func, is_place,
    is_compound_func, is_compound,
    is_acronym_func, is_acronym,
    is_phrase_func, is_phrase,
    is_used_in_book_func, is_used_in_book,

    // name
    symbol,

    // variants
    _spc_, _bng_, _quot_, _hsh_, _dol_, _pct_, _und_, _sqt_, _parl_, _parr_,
    _ast_, _plus_, _cma_, _mns_, _dot_, _slsh_,
    esc_0, esc_1, esc_2, esc_3, esc_4, esc_5, esc_6, esc_7, esc_8, esc_9,
    _cln_, _scln_, _lt_, _eq_, _gt_, _qstn_, _atsym_,
    _sbl_, _bslsh_, _sbr_, _dach_, _, _bqt_,
    _cbl_, _pip_, _cbr_, _tld_,
    esc_A, esc_B, esc_C, esc_D, esc_E, esc_F, esc_G, esc_H, esc_I, esc_J, esc_K, esc_L, esc_M,
    esc_N, esc_O, esc_P, esc_Q, esc_R, esc_S, esc_T, esc_U, esc_V, esc_W, esc_X, esc_Y, esc_Z,
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
)

#define _set_properties(_symbol)                                                                           \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, count, &mm_count_##_symbol)                              \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, as_longest, &mm_as_longest_##_symbol)                    \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, at, &mm_at_##_symbol)                                    \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, lookup, &mm_lookup_##_symbol)                            \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, ordinal_summation, &mm_ordinal_summation_##_symbol)      \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol,                                                          \
                                 flagged_parts_of_speech, &mm_flagged_parts_of_speech_##_symbol)           \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, synonyms, &mm_synonyms_##_symbol)                        \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, antonyms, &mm_antonyms_##_symbol)                        \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, embedded, &mm_embedded_##_symbol)                        \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, definition, &mm_definition_##_symbol)                    \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, locations, &mm_locations_##_symbol)                      \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_name, &mm_is_name_##_symbol)                          \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_male_name, &mm_is_male_name_##_symbol)                \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_female_name, &mm_is_female_name_##_symbol)            \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_place, &mm_is_place_##_symbol)                        \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_compound, &mm_is_compound_##_symbol)                  \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_acronym, &mm_is_acronym_##_symbol)                    \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_phrase, &mm_is_phrase_##_symbol)                      \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol, _symbol, is_used_in_book, &mm_is_used_in_book_##_symbol)

_set_properties(_spc_)
_set_properties(_bng_)
_set_properties(_quot_)
_set_properties(_hsh_)
_set_properties(_dol_)
_set_properties(_pct_)
_set_properties(_und_)
_set_properties(_sqt_)
_set_properties(_parl_)
_set_properties(_parr_)
_set_properties(_ast_)
_set_properties(_plus_)
_set_properties(_cma_)
_set_properties(_mns_)
_set_properties(_dot_)
_set_properties(_slsh_)
_set_properties(esc_0)
_set_properties(esc_1)
_set_properties(esc_2)
_set_properties(esc_3)
_set_properties(esc_4)
_set_properties(esc_5)
_set_properties(esc_6)
_set_properties(esc_7)
_set_properties(esc_8)
_set_properties(esc_9)
_set_properties(_cln_)
_set_properties(_scln_)
_set_properties(_lt_)
_set_properties(_eq_)
_set_properties(_gt_)
_set_properties(_qstn_)
_set_properties(_atsym_)
_set_properties(_sbl_)
_set_properties(_bslsh_)
_set_properties(_sbr_)
_set_properties(_dach_)
_set_properties(_)
_set_properties(_bqt_)
_set_properties(_cbl_)
_set_properties(_pip_)
_set_properties(_cbr_)
_set_properties(_tld_)
_set_properties(esc_A)
_set_properties(esc_B)
_set_properties(esc_C)
_set_properties(esc_D)
_set_properties(esc_E)
_set_properties(esc_F)
_set_properties(esc_G)
_set_properties(esc_H)
_set_properties(esc_I)
_set_properties(esc_J)
_set_properties(esc_K)
_set_properties(esc_L)
_set_properties(esc_M)
_set_properties(esc_N)
_set_properties(esc_O)
_set_properties(esc_P)
_set_properties(esc_Q)
_set_properties(esc_R)
_set_properties(esc_S)
_set_properties(esc_T)
_set_properties(esc_U)
_set_properties(esc_V)
_set_properties(esc_W)
_set_properties(esc_X)
_set_properties(esc_Y)
_set_properties(esc_Z)
_set_properties(a)
_set_properties(b)
_set_properties(c)
_set_properties(d)
_set_properties(e)
_set_properties(f)
_set_properties(g)
_set_properties(h)
_set_properties(i)
_set_properties(j)
_set_properties(k)
_set_properties(l)
_set_properties(m)
_set_properties(n)
_set_properties(o)
_set_properties(p)
_set_properties(q)
_set_properties(r)
_set_properties(s)
_set_properties(t)
_set_properties(u)
_set_properties(v)
_set_properties(w)
_set_properties(x)
_set_properties(y)
_set_properties(z)

#undef _set_properties



matchable::MatchBox<symbol::Type, int> boundaries =
    []()
    {
        int i{0};
        matchable::MatchBox<symbol::Type, int> b;
        for (auto const & sym : symbol::variants_by_string())
        {
            b.set(sym, i);
            i += sym.as_count()();
        }
        return b;
    }();


int mm_count()
{
    static int const ret =
        [&]()
        {
            int r{0};
            for (auto const & s : symbol::variants())
                r += s.as_count()();
            return r;
        }();

    return ret;
}


int mm_lookup(char const * term, bool * found)
{
    if (strlen(term) == 0)
        goto lookup_failed;

    if (term[0] < 32 || term[0] > 126)
        goto lookup_failed;

    {
        int i = -1;

        if (term[0] >= 'a' && term[0] <= 'z')
        {
            // lower case letters start after the 43 non-letter symbols and the 26 capital letters
            i = (int16_t) (term[0] - 'a' + 69);
        }

        else if (term[0] >= 'A' && term[0] <= 'Z')
        {
            // in our list, capital letters start just after the last non-letter symbol
            // there are a total of 95 symbols, and 52 are letters
            // 95 - 52 = 43 non-letter symbols that precede all letters
            i = (int16_t) (term[0] - 'A' + 43);
        }

        // 33 symbols precede the capital letters on the ascii table, and occur first in our list
        else if (term[0] >= ' ' && term[0] <= '@')
        {
            i = (int16_t) (term[0] - ' ');
        }

        // these symbols fall between the capital and lower case letters on the ascii table
        else if (term[0] >= '[' && term[0] <= '`')
        {
            // 33 symbols precede 'A' in the ascii table, so these symbols start at index 33 in our list
            i = (int16_t) (term[0] - '[' + 33);
        }

        // these symbols fall after the lower case letters on the ascii table
        else if (term[0] >= '{' && term[0] <= '~')
        {
            // for our list, this last group of symbols starts after the initial group of 33
            // and the second group of 6
            i = (int16_t) (term[0] - '{' + 39);
        }

        symbol::Type sym = symbol::from_by_string_index(i);
        if (sym.is_nil())
        {
            std::cout << "\nmm_lookup() :  bug detected! index [" << i
                        << "] is out of bounds! (lookup failed)" << std::endl;
            goto lookup_failed;
        }

        int ret = sym.as_lookup()(term, found);
        ret += boundaries.at(sym);

        // int ret = boundaries_by_symbol[i].first.as_lookup()(term, found);
        // ret += boundaries_by_symbol[i].second;
        return ret;
    }

lookup_failed:
    if (nullptr != found)
        *found = false;
    return 0;
}


char const * mm_at(int index, int * length)
{
    static char const * empty_str = "";

    if (index < 0 || index >= mm_count())
    {
        if (nullptr != length)
            *length = 0;
        return empty_str;
    }

    auto iter = std::lower_bound(
        symbol::variants_by_string().begin(),
        symbol::variants_by_string().end(),
        index,
        [](symbol::Type const & sym, int i){ return boundaries.at(sym) <= i; }
    );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto & ret_str = iter->as_at()(index - boundaries.at(*iter));
    if (nullptr != length)
        *length = (int) ret_str.length();
    return ret_str.c_str();
}


int mm_as_longest(int index)
{
    if (index < 0 || index >= mm_count())
        return -1;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, int i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_as_longest()(index - boundaries.at(*iter));
}


int mm_from_longest(int length_index)
{
    if (length_index < 0 || length_index >= mm_count())
        return -1;

    return LONGEST_WORDS[length_index];
}


void mm_lengths(int const * * lengths, int * count)
{
    static std::vector<int> const v = []()
                                      {
                                          std::vector<int> v;
                                          for (auto const & [l, o] : LONGEST_WORDS_OFFSETS)
                                              v.push_back(l);
                                          return v;
                                      }();
    *lengths = v.data();
    *count = v.size();
}


bool mm_length_location(int length, int * index, int * count)
{
    auto iter = LONGEST_WORDS_OFFSETS.find(length);
    if (iter == LONGEST_WORDS_OFFSETS.end())
        return false;

    *index = iter->second.first;
    *count = iter->second.second - iter->second.first + 1;
    return true;
}


int mm_ordinal_summation(int index)
{
    if (index < 0 || index >= mm_count())
        return 0;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, int i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_ordinal_summation()(index - boundaries.at(*iter));
}


void mm_from_ordinal_summation(int summation, int const * * words, int * count)
{
    if (count == nullptr)
        return;

    if (summation < 0 || summation >= (int) ORDINAL_SUMMATIONS.size())
    {
        words = nullptr;
        *count = 0;
    }

    *words = ORDINAL_SUMMATIONS[summation].data();
    *count = (int) ORDINAL_SUMMATIONS[summation].size();
}


bool mm_parts_of_speech(int index, char const * const * * pos, int8_t const * * flagged, int * count)
{
    static std::vector<char const *> const all = []()
                                                 {
                                                     std::vector<char const *> ret;
                                                     for (auto p : parts_of_speech::variants())
                                                         ret.push_back(p.as_pos_desc().as_string().c_str());
                                                             return ret;
                                                 }();
    *pos = all.data();
    *count = (int) all.size();

    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, int i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto const & f = iter->as_flagged_parts_of_speech()(index - boundaries.at(*iter));
    if (f.size() != all.size())
    {
        std::cout << "******* mm_parts_of_speech() -> f.size() != all.size() *******"
                  << "buggy code detected!" << std::endl;
        return false;
    }

    *flagged = f.data();

    return true;
}


bool mm_is_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_name()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_male_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;


    return iter->as_is_male_name()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_female_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_female_name()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_place(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_place()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_compound(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_compound()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_acronym(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_acronym()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_phrase(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_phrase()(index - boundaries.at(*iter)) != 0;
}


bool mm_is_used_in_book(int book_index, int index)
{
    if (book_index < 0 || book_index >= mm_book_count() || index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    return iter->as_is_used_in_book()(book_index, index - boundaries.at(*iter)) != 0;
}


void mm_synonyms(int index, int const * * synonyms, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *synonyms = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto & s = iter->as_synonyms()(index - boundaries.at(*iter));
    *synonyms = s.data();
    *count = (int) s.size();
}


void mm_antonyms(int index, int const * * antonyms, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *antonyms = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto & a = iter->as_antonyms()(index - boundaries.at(*iter));
    *antonyms = a.data();
    *count = (int) a.size();
}


void mm_embedded(int index, int const * * embedded, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *embedded = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto & e = iter->as_embedded()(index - boundaries.at(*iter));
    *embedded = e.data();
    *count = (int) e.size();
}


void mm_definition(int index, int const * * definition, int * count)
{
    if (index < 0 || index >= mm_count())
    {
        *definition = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    auto & d = iter->as_definition()(index - boundaries.at(*iter));
    *definition = d.data();
    *count = (int) d.size();
}


void mm_locations(
    int index,
    int const * * book_indexes,
    int const * * chapter_indexes,
    int const * * paragraph_indexes,
    int const * * word_indexes,
    int * count
)
{
    if (index < 0 || index >= mm_count())
    {
        *book_indexes = nullptr;
        *chapter_indexes = nullptr;
        *paragraph_indexes = nullptr;
        *word_indexes = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol::variants_by_string().begin(),
                    symbol::variants_by_string().end(),
                    index,
                    [](symbol::Type const & sym, auto const & i){ return boundaries.at(sym) <= i; }
                );
    if (iter != symbol::variants_by_string().begin())
        --iter;

    iter->as_locations()(
        index - boundaries.at(*iter),
        book_indexes,
        chapter_indexes,
        paragraph_indexes,
        word_indexes,
        count
    );
}


void mm_complete(char const * prefix, int * start, int * length)
{
    // shortcut empty prefix to everything
    if (0 == strlen(prefix))
    {
        *start = 0;
        *length = mm_count();
        return;
    }

    auto str_starts_with =
        [](char const * str, int str_len, char const * starts_with)
        {
            int starts_with_len = strlen(starts_with);

            if (starts_with_len > str_len)
                return false;

            for (int i = 0; i < starts_with_len; ++i)
                if (str[i] != starts_with[i])
                    return false;

            return true;
        };

    int index = mm_lookup(prefix, nullptr);

    *start = index;
    *length = 0;
    int s_len = 0;
    char const * s = nullptr;
    for (; index < mm_count(); ++index)
    {
        s = mm_at(index, &s_len);

        if (str_starts_with(s, s_len, prefix))
            *length += 1;
        else
            break;
    }
}


int mm_book_count()
{
#ifdef STAGE0
    return 0;
#else
    return BOOK_COUNT;
#endif
}


void mm_book_title(int book_index, int const * * book_title, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) book_title;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
    {
        *book_title = nullptr;
        *word_count = 0;
        return;
    }

    book_title_funcs[book_index](book_title, word_count);
#endif
}


void mm_book_author(int book_index, int const * * book_author, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) book_author;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
    {
        *book_author = nullptr;
        *word_count = 0;
        return;
    }

    book_author_funcs[book_index](book_author, word_count);
#endif
}


int mm_chapter_count(int book_index)
{
#ifdef STAGE0
    (void) book_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT)
        return 0;

    return chapter_count_funcs[book_index]();
#endif
}


void mm_chapter_title(int book_index, int chapter_index, int const * * chapter_title, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) chapter_title;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_title = nullptr;
        *word_count = 0;
        return;
    }

    chapter_title_funcs[book_index](chapter_index, chapter_title, word_count);
#endif
}


void mm_chapter_subtitle(int book_index, int chapter_index,
                         int const * * chapter_subtitle, int * word_count)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) chapter_subtitle;
    (void) word_count;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_subtitle = nullptr;
        *word_count = 0;
        return;
    }

    chapter_subtitle_funcs[book_index](chapter_index, chapter_subtitle, word_count);
#endif
}


int mm_paragraph_count(int book_index, int chapter_index)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
        return 0;

    return paragraph_count_funcs[book_index](chapter_index);
#endif
}


int mm_word_count(int book_index, int chapter_index, int paragraph_index)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) paragraph_index;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index))
        return 0;

    return word_count_funcs[book_index](chapter_index, paragraph_index);
#endif
}


int mm_word(
    int book_index,
    int chapter_index,
    int paragraph_index,
    int word_index,
    int const * * ancestors,
    int * ancestor_count,
    int * index_within_first_ancestor,
    bool * referenced
)
{
#ifdef STAGE0
    (void) book_index;
    (void) chapter_index;
    (void) paragraph_index;
    (void) word_index;
    (void) ancestors;
    (void) ancestor_count;
    (void) index_within_first_ancestor;
    (void) referenced;
    return 0;
#else
    if (book_index < 0 || book_index >= BOOK_COUNT ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index) ||
            word_index < 0 || word_index >= mm_word_count(book_index, chapter_index, paragraph_index))
        return -1;

    return word_funcs[book_index](chapter_index,
                                  paragraph_index,
                                  word_index,
                                  ancestors,
                                  ancestor_count,
                                  index_within_first_ancestor,
                                  referenced);
#endif
}
