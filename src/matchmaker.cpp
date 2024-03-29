#include <matchmaker/matchmaker.h>

#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>

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
#include <matchmaker/generated_symbols/_mns_/_mns_.h>
#include <matchmaker/generated_symbols/_dot_/_dot_.h>
#include <matchmaker/generated_symbols/_slsh_/_slsh_.h>
#include <matchmaker/generated_symbols/_cln_/_cln_.h>
#include <matchmaker/generated_symbols/_quot_/_quot_.h>
#include <matchmaker/generated_symbols/_hsh_/_hsh_.h>
#include <matchmaker/generated_symbols/_dol_/_dol_.h>
#include <matchmaker/generated_symbols/_sqt_/_sqt_.h>
#include <matchmaker/generated_symbols/_parl_/_parl_.h>
#include <matchmaker/generated_symbols/_plus_/_plus_.h>
#include <matchmaker/generated_symbols/_gt_/_gt_.h>
#include <matchmaker/generated_symbols/_sbr_/_sbr_.h>
#include <matchmaker/generated_symbols/_/_.h>
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
    count_func,
    count,
    as_longest_func,
    as_longest,
    at_func,
    at,
    lookup_func,
    lookup,
    ordinal_summation_func,
    ordinal_summation,
    flagged_parts_of_speech_func,
    flagged_parts_of_speech,
    synonyms_func,
    synonyms,
    antonyms_func,
    antonyms,
    embedded_func,
    embedded,
    definition_func,
    definition,
    locations_func,
    locations,
    is_name_func,
    is_name,
    is_male_name_func,
    is_male_name,
    is_female_name_func,
    is_female_name,
    is_place_func,
    is_place,
    is_compound_func,
    is_compound,
    is_acronym_func,
    is_acronym,
    is_phrase_func,
    is_phrase,
    is_used_in_book_func,
    is_used_in_book,

    // name
    symbol,

    // variants
    _quot_, _hsh_, _dol_, _sqt_, _parl_, _plus_, _mns_, _dot_, _slsh_,
    esc_0, esc_1, esc_2, esc_3, esc_4, esc_5, esc_6, esc_7, esc_8, esc_9,
    _cln_, _gt_, _sbr_, _, _tld_,
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

_set_properties(_quot_)
_set_properties(_hsh_)
_set_properties(_dol_)
_set_properties(_sqt_)
_set_properties(_parl_)
_set_properties(_plus_)
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
_set_properties(_gt_)
_set_properties(_sbr_)
_set_properties(_)
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


// store the start indexes for each symbol
static std::vector<std::pair<int, symbol::Type>> const boundries_by_index =
//     // this code requires variants_by_index() which is disabled!
//     //
//     // variants() is the same as variants_by_string() when MATCHABLE_OMIT_BY_INDEX is used!
//     // this worked back when all we had were A-Z,a-z since for this case the two sorting orders
//     // are equivalent
//     //
//     // we now have symbols, so the sorting orders are different!
//     //
//     // potential solutions:
//     //
//     //     0) stop using MATCHABLE_OMIT_BY_INDEX which will bloat all the term matchables even though
//     //        they are fine with alphabetical order only - and the library is already huge!
//     //
//     //     1) conjure up some fragile way of using MATCHABLE_OMIT_BY_INDEX for the terms while using
//     //        the normal full featured matchable here
//     //
//     //     2) unroll the super-cool matchable variant iteration which is tedious and typo prone but
//     //        if correct will preserve stability, simplicity and performance
//     [](){
//         int b{0};
//         std::vector<std::pair<int, symbol::Type>> boundries;
//         for (auto const & s : symbol::variants())
//         {
//             boundries.push_back(std::make_pair(b, s));
//             b += s.as_count()();
//         }
//         return boundries;
//     }();

    // solution 2
    [](){
        int b{0};
        std::vector<std::pair<int, symbol::Type>> boundries;

        boundries.push_back(std::make_pair(b, symbol::_quot_::grab()));
        b += symbol::_quot_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_hsh_::grab()));
        b += symbol::_hsh_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_dol_::grab()));
        b += symbol::_dol_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_sqt_::grab()));
        b += symbol::_sqt_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_parl_::grab()));
        b += symbol::_parl_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_plus_::grab()));
        b += symbol::_plus_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_mns_::grab()));
        b += symbol::_mns_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_dot_::grab()));
        b += symbol::_dot_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_slsh_::grab()));
        b += symbol::_slsh_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_0::grab()));
        b += symbol::esc_0::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_1::grab()));
        b += symbol::esc_1::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_2::grab()));
        b += symbol::esc_2::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_3::grab()));
        b += symbol::esc_3::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_4::grab()));
        b += symbol::esc_4::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_5::grab()));
        b += symbol::esc_5::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_6::grab()));
        b += symbol::esc_6::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_7::grab()));
        b += symbol::esc_7::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_8::grab()));
        b += symbol::esc_8::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_9::grab()));
        b += symbol::esc_9::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_cln_::grab()));
        b += symbol::_cln_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_gt_::grab()));
        b += symbol::_gt_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_sbr_::grab()));
        b += symbol::_sbr_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_::grab()));
        b += symbol::_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::_tld_::grab()));
        b += symbol::_tld_::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_A::grab()));
        b += symbol::esc_A::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_B::grab()));
        b += symbol::esc_B::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_C::grab()));
        b += symbol::esc_C::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_D::grab()));
        b += symbol::esc_D::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_E::grab()));
        b += symbol::esc_E::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_F::grab()));
        b += symbol::esc_F::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_G::grab()));
        b += symbol::esc_G::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_H::grab()));
        b += symbol::esc_H::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_I::grab()));
        b += symbol::esc_I::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_J::grab()));
        b += symbol::esc_J::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_K::grab()));
        b += symbol::esc_K::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_L::grab()));
        b += symbol::esc_L::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_M::grab()));
        b += symbol::esc_M::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_N::grab()));
        b += symbol::esc_N::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_O::grab()));
        b += symbol::esc_O::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_P::grab()));
        b += symbol::esc_P::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_Q::grab()));
        b += symbol::esc_Q::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_R::grab()));
        b += symbol::esc_R::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_S::grab()));
        b += symbol::esc_S::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_T::grab()));
        b += symbol::esc_T::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_U::grab()));
        b += symbol::esc_U::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_V::grab()));
        b += symbol::esc_V::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_W::grab()));
        b += symbol::esc_W::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_X::grab()));
        b += symbol::esc_X::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_Y::grab()));
        b += symbol::esc_Y::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::esc_Z::grab()));
        b += symbol::esc_Z::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::a::grab()));
        b += symbol::a::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::b::grab()));
        b += symbol::b::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::c::grab()));
        b += symbol::c::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::d::grab()));
        b += symbol::d::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::e::grab()));
        b += symbol::e::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::f::grab()));
        b += symbol::f::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::g::grab()));
        b += symbol::g::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::h::grab()));
        b += symbol::h::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::i::grab()));
        b += symbol::i::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::j::grab()));
        b += symbol::j::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::k::grab()));
        b += symbol::k::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::l::grab()));
        b += symbol::l::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::m::grab()));
        b += symbol::m::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::n::grab()));
        b += symbol::n::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::o::grab()));
        b += symbol::o::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::p::grab()));
        b += symbol::p::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::q::grab()));
        b += symbol::q::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::r::grab()));
        b += symbol::r::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::s::grab()));
        b += symbol::s::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::t::grab()));
        b += symbol::t::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::u::grab()));
        b += symbol::u::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::v::grab()));
        b += symbol::v::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::w::grab()));
        b += symbol::w::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::x::grab()));
        b += symbol::x::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::y::grab()));
        b += symbol::y::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::z::grab()));
        b += symbol::z::grab().as_count()();

        return boundries;
    }();



int mm_count()
{
    static int const ret =
        [&](){
            int r{0};
            for (auto const & l : symbol::variants())
                r += l.as_count()();
            return r;
        }();

    return ret;
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
        boundries_by_index.begin(),
        boundries_by_index.end(),
        index,
        [](auto const & b, auto const & i){ return b.first <= i; }
    );
    if (iter != boundries_by_index.begin())
        --iter;

    auto & w = iter->second.as_at()(index - iter->first);
    if (nullptr != length)
        *length = (int) w.length();
    return w.c_str();
}


int mm_lookup(char const * word, bool * found)
{
    static std::array<std::pair<symbol::Type, int>, 76> const boundries_by_symbol {
        std::make_pair(symbol::_quot_::grab(), boundries_by_index[0].first),
        std::make_pair(symbol::_hsh_::grab(), boundries_by_index[1].first),
        std::make_pair(symbol::_dol_::grab(), boundries_by_index[2].first),
        std::make_pair(symbol::_sqt_::grab(), boundries_by_index[3].first),
        std::make_pair(symbol::_parl_::grab(), boundries_by_index[4].first),
        std::make_pair(symbol::_plus_::grab(), boundries_by_index[5].first),
        std::make_pair(symbol::_mns_::grab(), boundries_by_index[6].first),
        std::make_pair(symbol::_dot_::grab(), boundries_by_index[7].first),
        std::make_pair(symbol::_slsh_::grab(), boundries_by_index[8].first),
        std::make_pair(symbol::esc_0::grab(), boundries_by_index[9].first),
        std::make_pair(symbol::esc_1::grab(), boundries_by_index[10].first),
        std::make_pair(symbol::esc_2::grab(), boundries_by_index[11].first),
        std::make_pair(symbol::esc_3::grab(), boundries_by_index[12].first),
        std::make_pair(symbol::esc_4::grab(), boundries_by_index[13].first),
        std::make_pair(symbol::esc_5::grab(), boundries_by_index[14].first),
        std::make_pair(symbol::esc_6::grab(), boundries_by_index[15].first),
        std::make_pair(symbol::esc_7::grab(), boundries_by_index[16].first),
        std::make_pair(symbol::esc_8::grab(), boundries_by_index[17].first),
        std::make_pair(symbol::esc_9::grab(), boundries_by_index[18].first),
        std::make_pair(symbol::_cln_::grab(), boundries_by_index[19].first),
        std::make_pair(symbol::_gt_::grab(), boundries_by_index[20].first),
        std::make_pair(symbol::_sbr_::grab(), boundries_by_index[21].first),
        std::make_pair(symbol::_::grab(), boundries_by_index[22].first),
        std::make_pair(symbol::_tld_::grab(), boundries_by_index[23].first),
        std::make_pair(symbol::esc_A::grab(), boundries_by_index[24].first),
        std::make_pair(symbol::esc_B::grab(), boundries_by_index[25].first),
        std::make_pair(symbol::esc_C::grab(), boundries_by_index[26].first),
        std::make_pair(symbol::esc_D::grab(), boundries_by_index[27].first),
        std::make_pair(symbol::esc_E::grab(), boundries_by_index[28].first),
        std::make_pair(symbol::esc_F::grab(), boundries_by_index[29].first),
        std::make_pair(symbol::esc_G::grab(), boundries_by_index[30].first),
        std::make_pair(symbol::esc_H::grab(), boundries_by_index[31].first),
        std::make_pair(symbol::esc_I::grab(), boundries_by_index[32].first),
        std::make_pair(symbol::esc_J::grab(), boundries_by_index[33].first),
        std::make_pair(symbol::esc_K::grab(), boundries_by_index[34].first),
        std::make_pair(symbol::esc_L::grab(), boundries_by_index[35].first),
        std::make_pair(symbol::esc_M::grab(), boundries_by_index[36].first),
        std::make_pair(symbol::esc_N::grab(), boundries_by_index[37].first),
        std::make_pair(symbol::esc_O::grab(), boundries_by_index[38].first),
        std::make_pair(symbol::esc_P::grab(), boundries_by_index[39].first),
        std::make_pair(symbol::esc_Q::grab(), boundries_by_index[40].first),
        std::make_pair(symbol::esc_R::grab(), boundries_by_index[41].first),
        std::make_pair(symbol::esc_S::grab(), boundries_by_index[42].first),
        std::make_pair(symbol::esc_T::grab(), boundries_by_index[43].first),
        std::make_pair(symbol::esc_U::grab(), boundries_by_index[44].first),
        std::make_pair(symbol::esc_V::grab(), boundries_by_index[45].first),
        std::make_pair(symbol::esc_W::grab(), boundries_by_index[46].first),
        std::make_pair(symbol::esc_X::grab(), boundries_by_index[47].first),
        std::make_pair(symbol::esc_Y::grab(), boundries_by_index[48].first),
        std::make_pair(symbol::esc_Z::grab(), boundries_by_index[49].first),
        std::make_pair(symbol::a::grab(), boundries_by_index[50].first),
        std::make_pair(symbol::b::grab(), boundries_by_index[51].first),
        std::make_pair(symbol::c::grab(), boundries_by_index[52].first),
        std::make_pair(symbol::d::grab(), boundries_by_index[53].first),
        std::make_pair(symbol::e::grab(), boundries_by_index[54].first),
        std::make_pair(symbol::f::grab(), boundries_by_index[55].first),
        std::make_pair(symbol::g::grab(), boundries_by_index[56].first),
        std::make_pair(symbol::h::grab(), boundries_by_index[57].first),
        std::make_pair(symbol::i::grab(), boundries_by_index[58].first),
        std::make_pair(symbol::j::grab(), boundries_by_index[59].first),
        std::make_pair(symbol::k::grab(), boundries_by_index[60].first),
        std::make_pair(symbol::l::grab(), boundries_by_index[61].first),
        std::make_pair(symbol::m::grab(), boundries_by_index[62].first),
        std::make_pair(symbol::n::grab(), boundries_by_index[63].first),
        std::make_pair(symbol::o::grab(), boundries_by_index[64].first),
        std::make_pair(symbol::p::grab(), boundries_by_index[65].first),
        std::make_pair(symbol::q::grab(), boundries_by_index[66].first),
        std::make_pair(symbol::r::grab(), boundries_by_index[67].first),
        std::make_pair(symbol::s::grab(), boundries_by_index[68].first),
        std::make_pair(symbol::t::grab(), boundries_by_index[69].first),
        std::make_pair(symbol::u::grab(), boundries_by_index[70].first),
        std::make_pair(symbol::v::grab(), boundries_by_index[71].first),
        std::make_pair(symbol::w::grab(), boundries_by_index[72].first),
        std::make_pair(symbol::x::grab(), boundries_by_index[73].first),
        std::make_pair(symbol::y::grab(), boundries_by_index[74].first),
        std::make_pair(symbol::z::grab(), boundries_by_index[75].first),
    };

    if (strlen(word) == 0)
        goto lookup_failed;

    if (word[0] < 32 || word[0] > 126)
        goto lookup_failed;

    {
        int i = word[0];
        if (word[0] >= 'a' && word[0] <= 'z')
        {
//             i -= 'a';
//             i += 50;
            // "simplify for speed!
            i -= 47;
        }
        else if (word[0] >= 'A' && word[0] <= 'Z')
        {
//             i -= 'A';  // ascii table location
//             i += 24;   // boundries_by_index location
            // "simplify" for speed!
            i -= 41;
        }
        else if (word[0] >= '-' && word[0] <= ':')
        {
//             i -= '-';
//             i += 6;
            // "simplify for speed!
            i -= 39;
        }
        else if (word[0] == '\'' || word[0] == '(')
        {
//             i -= '\'';
//             i += 3;
            // "simplify for speed!
            i -= 36;
        }
        else if (word[0] >= '"' && word[0] <= '$')
        {
            i -= '"';
//             i += 0;
        }
        else if (word[0] == '>')
        {
//             i -= '>';
//             i += 20;
            // "simplify for speed!
            i -= 42;
        }
        else if (word[0] == ']')
        {
//             i -= ']';
//             i += 21;
            // "simplify for speed!
            i -= 72;
        }
        else if (word[0] == '_')
        {
//             i -= '_';
//             i += 22;
            // "simplify for speed!
            i -= 73;
        }
        else if (word[0] == '~')
        {
//             i -= '~';
//             i += 23;
            // "simplify for speed!
            i -= 103;
        }
        else if (word[0] == '+')
        {
//             i -= '+';
//             i += 5;
            // "simplify for speed!
            i -= 38;
        }
        else
        {
            i = 24; // 'A'
        }

        if (i < 0 || i > 75)
        {
            std::cout << "\nmm_lookup() :  bug detected! index [" << i
                        << "] is out of bounds! (lookup failed)" << std::endl;
            goto lookup_failed;
        }

        int ret = boundries_by_symbol[i].first.as_lookup()(word, found);
        ret += boundries_by_symbol[i].second;
        return ret;
    }

lookup_failed:
    if (nullptr != found)
        *found = false;
    return 0;
}


int mm_as_longest(int index)
{
    if (index < 0 || index >= mm_count())
        return -1;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_as_longest()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_ordinal_summation()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    auto const & f = iter->second.as_flagged_parts_of_speech()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_name()(index - iter->first) != 0;
}


bool mm_is_male_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_male_name()(index - iter->first) != 0;
}


bool mm_is_female_name(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_female_name()(index - iter->first) != 0;
}


bool mm_is_place(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_place()(index - iter->first) != 0;
}


bool mm_is_compound(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_compound()(index - iter->first) != 0;
}


bool mm_is_acronym(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_acronym()(index - iter->first) != 0;
}


bool mm_is_phrase(int index)
{
    if (index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_phrase()(index - iter->first) != 0;
}


bool mm_is_used_in_book(int book_index, int index)
{
    if (book_index < 0 || book_index >= mm_book_count() || index < 0 || index >= mm_count())
        return false;

    auto iter = std::lower_bound(
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    return iter->second.as_is_used_in_book()(book_index, index - iter->first) != 0;
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    auto & s = iter->second.as_synonyms()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    auto & a = iter->second.as_antonyms()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    auto & e = iter->second.as_embedded()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    auto & d = iter->second.as_definition()(index - iter->first);
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
                    boundries_by_index.begin(),
                    boundries_by_index.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != boundries_by_index.begin())
        --iter;

    iter->second.as_locations()(index - iter->first,
                                book_indexes,
                                chapter_indexes,
                                paragraph_indexes,
                                word_indexes,
                                count);
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
