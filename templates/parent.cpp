/*
Copyright (c) 2020-2023, shtroizel
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



// 'a o e u' (without spaces) replaced by letters deliminated by '/' (aoeu)
// 's n t h' (without spaces) replaced by letters deliminated by '_' (snth)



#include <matchmaker/generated_symbols/aoeu/snth.h>

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <matchable/matchable.h>

#include <matchmaker/generated_symbols/aoeu/_spc_/snth__spc_.h>
#include <matchmaker/generated_symbols/aoeu/_bng_/snth__bng_.h>
#include <matchmaker/generated_symbols/aoeu/_quot_/snth__quot_.h>
#include <matchmaker/generated_symbols/aoeu/_hsh_/snth__hsh_.h>
#include <matchmaker/generated_symbols/aoeu/_dol_/snth__dol_.h>
#include <matchmaker/generated_symbols/aoeu/_pct_/snth__pct_.h>
#include <matchmaker/generated_symbols/aoeu/_und_/snth__und_.h>
#include <matchmaker/generated_symbols/aoeu/_sqt_/snth__sqt_.h>
#include <matchmaker/generated_symbols/aoeu/_parl_/snth__parl_.h>
#include <matchmaker/generated_symbols/aoeu/_parr_/snth__parr_.h>
#include <matchmaker/generated_symbols/aoeu/_ast_/snth__ast_.h>
#include <matchmaker/generated_symbols/aoeu/_plus_/snth__plus_.h>
#include <matchmaker/generated_symbols/aoeu/_cma_/snth__cma_.h>
#include <matchmaker/generated_symbols/aoeu/_mns_/snth__mns_.h>
#include <matchmaker/generated_symbols/aoeu/_dot_/snth__dot_.h>
#include <matchmaker/generated_symbols/aoeu/_slsh_/snth__slsh_.h>
#include <matchmaker/generated_symbols/aoeu/esc_0/snth_esc_0.h>
#include <matchmaker/generated_symbols/aoeu/esc_1/snth_esc_1.h>
#include <matchmaker/generated_symbols/aoeu/esc_2/snth_esc_2.h>
#include <matchmaker/generated_symbols/aoeu/esc_3/snth_esc_3.h>
#include <matchmaker/generated_symbols/aoeu/esc_4/snth_esc_4.h>
#include <matchmaker/generated_symbols/aoeu/esc_5/snth_esc_5.h>
#include <matchmaker/generated_symbols/aoeu/esc_6/snth_esc_6.h>
#include <matchmaker/generated_symbols/aoeu/esc_7/snth_esc_7.h>
#include <matchmaker/generated_symbols/aoeu/esc_8/snth_esc_8.h>
#include <matchmaker/generated_symbols/aoeu/esc_9/snth_esc_9.h>
#include <matchmaker/generated_symbols/aoeu/_cln_/snth__cln_.h>
#include <matchmaker/generated_symbols/aoeu/_scln_/snth__scln_.h>
#include <matchmaker/generated_symbols/aoeu/_lt_/snth__lt_.h>
#include <matchmaker/generated_symbols/aoeu/_eq_/snth__eq_.h>
#include <matchmaker/generated_symbols/aoeu/_gt_/snth__gt_.h>
#include <matchmaker/generated_symbols/aoeu/_qstn_/snth__qstn_.h>
#include <matchmaker/generated_symbols/aoeu/_atsym_/snth__atsym_.h>
#include <matchmaker/generated_symbols/aoeu/_sbl_/snth__sbl_.h>
#include <matchmaker/generated_symbols/aoeu/_bslsh_/snth__bslsh_.h>
#include <matchmaker/generated_symbols/aoeu/_sbr_/snth__sbr_.h>
#include <matchmaker/generated_symbols/aoeu/_dach_/snth__dach_.h>
#include <matchmaker/generated_symbols/aoeu/_/snth__.h>
#include <matchmaker/generated_symbols/aoeu/_bqt_/snth__bqt_.h>
#include <matchmaker/generated_symbols/aoeu/_cbl_/snth__cbl_.h>
#include <matchmaker/generated_symbols/aoeu/_pip_/snth__pip_.h>
#include <matchmaker/generated_symbols/aoeu/_cbr_/snth__cbr_.h>
#include <matchmaker/generated_symbols/aoeu/_tld_/snth__tld_.h>
#include <matchmaker/generated_symbols/aoeu/esc_A/snth_esc_A.h>
#include <matchmaker/generated_symbols/aoeu/esc_B/snth_esc_B.h>
#include <matchmaker/generated_symbols/aoeu/esc_C/snth_esc_C.h>
#include <matchmaker/generated_symbols/aoeu/esc_D/snth_esc_D.h>
#include <matchmaker/generated_symbols/aoeu/esc_E/snth_esc_E.h>
#include <matchmaker/generated_symbols/aoeu/esc_F/snth_esc_F.h>
#include <matchmaker/generated_symbols/aoeu/esc_G/snth_esc_G.h>
#include <matchmaker/generated_symbols/aoeu/esc_H/snth_esc_H.h>
#include <matchmaker/generated_symbols/aoeu/esc_I/snth_esc_I.h>
#include <matchmaker/generated_symbols/aoeu/esc_J/snth_esc_J.h>
#include <matchmaker/generated_symbols/aoeu/esc_K/snth_esc_K.h>
#include <matchmaker/generated_symbols/aoeu/esc_L/snth_esc_L.h>
#include <matchmaker/generated_symbols/aoeu/esc_M/snth_esc_M.h>
#include <matchmaker/generated_symbols/aoeu/esc_N/snth_esc_N.h>
#include <matchmaker/generated_symbols/aoeu/esc_O/snth_esc_O.h>
#include <matchmaker/generated_symbols/aoeu/esc_P/snth_esc_P.h>
#include <matchmaker/generated_symbols/aoeu/esc_Q/snth_esc_Q.h>
#include <matchmaker/generated_symbols/aoeu/esc_R/snth_esc_R.h>
#include <matchmaker/generated_symbols/aoeu/esc_S/snth_esc_S.h>
#include <matchmaker/generated_symbols/aoeu/esc_T/snth_esc_T.h>
#include <matchmaker/generated_symbols/aoeu/esc_U/snth_esc_U.h>
#include <matchmaker/generated_symbols/aoeu/esc_V/snth_esc_V.h>
#include <matchmaker/generated_symbols/aoeu/esc_W/snth_esc_W.h>
#include <matchmaker/generated_symbols/aoeu/esc_X/snth_esc_X.h>
#include <matchmaker/generated_symbols/aoeu/esc_Y/snth_esc_Y.h>
#include <matchmaker/generated_symbols/aoeu/esc_Z/snth_esc_Z.h>
#include <matchmaker/generated_symbols/aoeu/a/snth_a.h>
#include <matchmaker/generated_symbols/aoeu/b/snth_b.h>
#include <matchmaker/generated_symbols/aoeu/c/snth_c.h>
#include <matchmaker/generated_symbols/aoeu/d/snth_d.h>
#include <matchmaker/generated_symbols/aoeu/e/snth_e.h>
#include <matchmaker/generated_symbols/aoeu/f/snth_f.h>
#include <matchmaker/generated_symbols/aoeu/g/snth_g.h>
#include <matchmaker/generated_symbols/aoeu/h/snth_h.h>
#include <matchmaker/generated_symbols/aoeu/i/snth_i.h>
#include <matchmaker/generated_symbols/aoeu/j/snth_j.h>
#include <matchmaker/generated_symbols/aoeu/k/snth_k.h>
#include <matchmaker/generated_symbols/aoeu/l/snth_l.h>
#include <matchmaker/generated_symbols/aoeu/m/snth_m.h>
#include <matchmaker/generated_symbols/aoeu/n/snth_n.h>
#include <matchmaker/generated_symbols/aoeu/o/snth_o.h>
#include <matchmaker/generated_symbols/aoeu/p/snth_p.h>
#include <matchmaker/generated_symbols/aoeu/q/snth_q.h>
#include <matchmaker/generated_symbols/aoeu/r/snth_r.h>
#include <matchmaker/generated_symbols/aoeu/s/snth_s.h>
#include <matchmaker/generated_symbols/aoeu/t/snth_t.h>
#include <matchmaker/generated_symbols/aoeu/u/snth_u.h>
#include <matchmaker/generated_symbols/aoeu/v/snth_v.h>
#include <matchmaker/generated_symbols/aoeu/w/snth_w.h>
#include <matchmaker/generated_symbols/aoeu/x/snth_x.h>
#include <matchmaker/generated_symbols/aoeu/y/snth_y.h>
#include <matchmaker/generated_symbols/aoeu/z/snth_z.h>



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
    symbol_snth,

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


#define _set_properties(_letter)                                                                           \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, count, &mm_count_snth_##_letter)                    \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, as_longest, &mm_as_longest_snth_##_letter)          \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, at, &mm_at_snth_##_letter)                          \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, lookup, &mm_lookup_snth_##_letter)                  \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter,                                                     \
                                 ordinal_summation, &mm_ordinal_summation_snth_##_letter)                  \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter,                                                     \
                                 flagged_parts_of_speech, &mm_flagged_parts_of_speech_snth_##_letter)      \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, synonyms, &mm_synonyms_snth_##_letter)              \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, antonyms, &mm_antonyms_snth_##_letter)              \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, embedded, &mm_embedded_snth_##_letter)              \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, definition, &mm_definition_snth_##_letter)          \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, locations, &mm_locations_snth_##_letter)            \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_name, &mm_is_name_snth_##_letter)                \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_male_name, &mm_is_male_name_snth_##_letter)      \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_female_name, &mm_is_female_name_snth_##_letter)  \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_place, &mm_is_place_snth_##_letter)              \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_compound, &mm_is_compound_snth_##_letter)        \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_acronym, &mm_is_acronym_snth_##_letter)          \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_phrase, &mm_is_phrase_snth_##_letter)            \
MATCHABLE_VARIANT_PROPERTY_VALUE(symbol_snth, _letter, is_used_in_book, &mm_is_used_in_book_snth_##_letter)

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



matchable::MatchBox<symbol_snth::Type, int> boundaries_snth =
    []()
    {
        int i{0};
        matchable::MatchBox<symbol_snth::Type, int> b;
        for (auto const & sym_snth : symbol_snth::variants_by_string())
        {
            b.set(sym_snth, i);
            i += sym_snth.as_count()();
        }
        return b;
    }();


int mm_count_snth()
{
    static int const ret =
        [&]()
        {
            int r{0};
            r += mm_count_snth__spc_();
            r += mm_count_snth__bng_();
            r += mm_count_snth__quot_();
            r += mm_count_snth__hsh_();
            r += mm_count_snth__dol_();
            r += mm_count_snth__pct_();
            r += mm_count_snth__und_();
            r += mm_count_snth__sqt_();
            r += mm_count_snth__parl_();
            r += mm_count_snth__parr_();
            r += mm_count_snth__ast_();
            r += mm_count_snth__plus_();
            r += mm_count_snth__cma_();
            r += mm_count_snth__mns_();
            r += mm_count_snth__dot_();
            r += mm_count_snth__slsh_();
            r += mm_count_snth_esc_0();
            r += mm_count_snth_esc_1();
            r += mm_count_snth_esc_2();
            r += mm_count_snth_esc_3();
            r += mm_count_snth_esc_4();
            r += mm_count_snth_esc_5();
            r += mm_count_snth_esc_6();
            r += mm_count_snth_esc_7();
            r += mm_count_snth_esc_8();
            r += mm_count_snth_esc_9();
            r += mm_count_snth__cln_();
            r += mm_count_snth__scln_();
            r += mm_count_snth__lt_();
            r += mm_count_snth__eq_();
            r += mm_count_snth__gt_();
            r += mm_count_snth__qstn_();
            r += mm_count_snth__atsym_();
            r += mm_count_snth__sbl_();
            r += mm_count_snth__bslsh_();
            r += mm_count_snth__sbr_();
            r += mm_count_snth__dach_();
            r += mm_count_snth__();
            r += mm_count_snth__bqt_();
            r += mm_count_snth__cbl_();
            r += mm_count_snth__pip_();
            r += mm_count_snth__cbr_();
            r += mm_count_snth__tld_();
            r += mm_count_snth_esc_A();
            r += mm_count_snth_esc_B();
            r += mm_count_snth_esc_C();
            r += mm_count_snth_esc_D();
            r += mm_count_snth_esc_E();
            r += mm_count_snth_esc_F();
            r += mm_count_snth_esc_G();
            r += mm_count_snth_esc_H();
            r += mm_count_snth_esc_I();
            r += mm_count_snth_esc_J();
            r += mm_count_snth_esc_K();
            r += mm_count_snth_esc_L();
            r += mm_count_snth_esc_M();
            r += mm_count_snth_esc_N();
            r += mm_count_snth_esc_O();
            r += mm_count_snth_esc_P();
            r += mm_count_snth_esc_Q();
            r += mm_count_snth_esc_R();
            r += mm_count_snth_esc_S();
            r += mm_count_snth_esc_T();
            r += mm_count_snth_esc_U();
            r += mm_count_snth_esc_V();
            r += mm_count_snth_esc_W();
            r += mm_count_snth_esc_X();
            r += mm_count_snth_esc_Y();
            r += mm_count_snth_esc_Z();
            r += mm_count_snth_a();
            r += mm_count_snth_b();
            r += mm_count_snth_c();
            r += mm_count_snth_d();
            r += mm_count_snth_e();
            r += mm_count_snth_f();
            r += mm_count_snth_g();
            r += mm_count_snth_h();
            r += mm_count_snth_i();
            r += mm_count_snth_j();
            r += mm_count_snth_k();
            r += mm_count_snth_l();
            r += mm_count_snth_m();
            r += mm_count_snth_n();
            r += mm_count_snth_o();
            r += mm_count_snth_p();
            r += mm_count_snth_q();
            r += mm_count_snth_r();
            r += mm_count_snth_s();
            r += mm_count_snth_t();
            r += mm_count_snth_u();
            r += mm_count_snth_v();
            r += mm_count_snth_w();
            r += mm_count_snth_x();
            r += mm_count_snth_y();
            r += mm_count_snth_z();
            return r;
        }();

    return ret;
}


int mm_lookup_snth(std::string const & term, bool * found)
{
    static std::string const d_str{"aoeu"};
    static int const depth = std::count(d_str.begin(), d_str.end(), '/') + 1;

    if (term.size() < 1)
        goto lookup_failed;

    if ((int) term.size() <= depth)
        return mm_lookup_snth__spc_(term, found);

    if (term[depth] < 32 || term[depth] > 126)
        goto lookup_failed;

    {
        int i = term[depth];
        if (term[depth] >= 'a' && term[depth] <= 'z')
        {
            // lower case letters start after the 43 non-letter symbols and the 26 capital letters
            i = (int16_t) (term[depth] - 'a' + 69);
        }

        else if (term[depth] >= 'A' && term[depth] <= 'Z')
        {
            // in our list, capital letters start just after the last non-letter symbol
            // there are a total of 95 symbols, and 52 are letters
            // 95 - 52 = 43 non-letter symbols that precede all letters
            i = (int16_t) (term[depth] - 'A' + 43);
        }

        // 33 symbols precede the capital letters on the ascii table, and occur first in our list
        else if (term[depth] >= ' ' && term[depth] <= '@')
        {
            i = (int16_t) (term[depth] - ' ');
        }

        // these symbols fall between the capital and lower case letters on the ascii table
        else if (term[depth] >= '[' && term[depth] <= '`')
        {
            // 33 symbols precede 'A' in the ascii table, so these symbols start at index 33 in our list
            i = (int16_t) (term[depth] - '[' + 33);
        }

        // these symbols fall after the lower case letters on the ascii table
        else if (term[depth] >= '{' && term[depth] <= '~')
        {
            // for our list, this last group of symbols starts after the initial group of 33
            // and the second group of 6
            i = (int16_t) (term[depth] - '{' + 39);
        }

        symbol_snth::Type sym = symbol_snth::from_by_string_index(i);
        if (sym.is_nil())
        {
            std::cout << "\nlookup_snth() :  bug detected! index [" << i
                        << "] is out of bounds! (lookup_snth() failed)" << std::endl;
            goto lookup_failed;
        }

        int ret = sym.as_lookup()(term, found);
        ret += boundaries_snth.at(sym);

        return ret;
    }

lookup_failed:
    if (nullptr != found)
        *found = false;
    return 0;
}


std::string const & mm_at_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_at_snth(" << index << ") out of bounds with mm_count_snth() of: "
                    << mm_count_snth() << std::endl;
        static std::string const empty_str_snth;
        return empty_str_snth;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_at()(index - boundaries_snth.at(*iter));
}


int mm_as_longest_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_as_longest_snth(" << index << ") out of bounds with mm_count_snth() of: "
                    << mm_count_snth() << std::endl;
        return 0;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_as_longest()(index - boundaries_snth.at(*iter));
}


int mm_ordinal_summation_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return 0;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_ordinal_summation()(index - boundaries_snth.at(*iter));
}


std::vector<int8_t> const & mm_flagged_parts_of_speech_snth(int index)
{
    static std::vector<int8_t> const empty{};

    // only possible when parts_of_speech() buggy (matchmaker.cpp)
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_flagged_parts_of_speech_snth(" << index
                  << ") out of bounds with mm_count_snth() of: " << mm_count_snth() << std::endl;
        return empty;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_flagged_parts_of_speech()(index - boundaries_snth.at(*iter));
}


bool mm_is_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_name()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_male_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_male_name()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_female_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_female_name()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_place_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_place()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_compound_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_compound()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_acronym_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_acronym()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_phrase_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_phrase()(index - boundaries_snth.at(*iter)) != 0;
}


bool mm_is_used_in_book_snth(int book_index, int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_is_used_in_book()(book_index, index - boundaries_snth.at(*iter)) != 0;
}


std::vector<int> const & mm_synonyms_snth(int index)
{
    static std::vector<int> const empty{};

    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_synonyms_snth(" << index << ") out of bounds with mm_count_snth() of: "
                    << mm_count_snth() << std::endl;
        return empty;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_synonyms()(index - boundaries_snth.at(*iter));
}


std::vector<int> const & mm_antonyms_snth(int index)
{
    static std::vector<int> const empty{};

    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_antonyms_snth(" << index << ") out of bounds with mm_count_snth() of: "
                    << mm_count_snth() << std::endl;
        return empty;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_antonyms()(index - boundaries_snth.at(*iter));
}


std::vector<int> const & mm_embedded_snth(int index)
{
    static std::vector<int> const empty{};

    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_embedded_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_embedded()(index - boundaries_snth.at(*iter));
}


std::vector<int> const & mm_definition_snth(int index)
{
    static std::vector<int> const empty{};

    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_definition_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    return iter->as_definition()(index - boundaries_snth.at(*iter));
}


void mm_locations_snth(
    int index,
    int const * * book_indexes,
    int const * * chapter_indexes,
    int const * * paragraph_indexes,
    int const * * word_indexes,
    int * count
)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_locations_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        *book_indexes = nullptr;
        *chapter_indexes = nullptr;
        *paragraph_indexes = nullptr;
        *word_indexes = nullptr;
        *count = 0;
        return;
    }

    auto iter = std::lower_bound(
                    symbol_snth::variants_by_string().begin(),
                    symbol_snth::variants_by_string().end(),
                    index,
                    [&](symbol_snth::Type const & sym, int i){ return boundaries_snth.at(sym) <= i; }
                );

    if (iter != symbol_snth::variants_by_string().begin())
        --iter;

    iter->as_locations()(
        index - boundaries_snth.at(*iter),
        book_indexes,
        chapter_indexes,
        paragraph_indexes,
        word_indexes,
        count
    );
}
