/*
Copyright (c) 2020, Eric Hyer
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


#include <matchmaker/matchmaker.h>

#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>

#include <matchmaker/generated_letters/A/A.h>
#include <matchmaker/generated_letters/B/B.h>
#include <matchmaker/generated_letters/C/C.h>
#include <matchmaker/generated_letters/D/D.h>
#include <matchmaker/generated_letters/E/E.h>
#include <matchmaker/generated_letters/F/F.h>
#include <matchmaker/generated_letters/G/G.h>
#include <matchmaker/generated_letters/H/H.h>
#include <matchmaker/generated_letters/I/I.h>
#include <matchmaker/generated_letters/J/J.h>
#include <matchmaker/generated_letters/K/K.h>
#include <matchmaker/generated_letters/L/L.h>
#include <matchmaker/generated_letters/M/M.h>
#include <matchmaker/generated_letters/N/N.h>
#include <matchmaker/generated_letters/O/O.h>
#include <matchmaker/generated_letters/P/P.h>
#include <matchmaker/generated_letters/Q/Q.h>
#include <matchmaker/generated_letters/R/R.h>
#include <matchmaker/generated_letters/S/S.h>
#include <matchmaker/generated_letters/T/T.h>
#include <matchmaker/generated_letters/U/U.h>
#include <matchmaker/generated_letters/V/V.h>
#include <matchmaker/generated_letters/W/W.h>
#include <matchmaker/generated_letters/X/X.h>
#include <matchmaker/generated_letters/Y/Y.h>
#include <matchmaker/generated_letters/Z/Z.h>
#include <matchmaker/generated_letters/a/a.h>
#include <matchmaker/generated_letters/b/b.h>
#include <matchmaker/generated_letters/c/c.h>
#include <matchmaker/generated_letters/d/d.h>
#include <matchmaker/generated_letters/e/e.h>
#include <matchmaker/generated_letters/f/f.h>
#include <matchmaker/generated_letters/g/g.h>
#include <matchmaker/generated_letters/h/h.h>
#include <matchmaker/generated_letters/i/i.h>
#include <matchmaker/generated_letters/j/j.h>
#include <matchmaker/generated_letters/k/k.h>
#include <matchmaker/generated_letters/l/l.h>
#include <matchmaker/generated_letters/m/m.h>
#include <matchmaker/generated_letters/n/n.h>
#include <matchmaker/generated_letters/o/o.h>
#include <matchmaker/generated_letters/p/p.h>
#include <matchmaker/generated_letters/q/q.h>
#include <matchmaker/generated_letters/r/r.h>
#include <matchmaker/generated_letters/s/s.h>
#include <matchmaker/generated_letters/t/t.h>
#include <matchmaker/generated_letters/u/u.h>
#include <matchmaker/generated_letters/v/v.h>
#include <matchmaker/generated_letters/w/w.h>
#include <matchmaker/generated_letters/x/x.h>
#include <matchmaker/generated_letters/y/y.h>
#include <matchmaker/generated_letters/z/z.h>

#include <matchmaker/generated_symbols/zero/zero.h>
#include <matchmaker/generated_symbols/one/one.h>
#include <matchmaker/generated_symbols/two/two.h>
#include <matchmaker/generated_symbols/three/three.h>
#include <matchmaker/generated_symbols/four/four.h>
#include <matchmaker/generated_symbols/five/five.h>
#include <matchmaker/generated_symbols/six/six.h>
#include <matchmaker/generated_symbols/seven/seven.h>
#include <matchmaker/generated_symbols/eight/eight.h>
#include <matchmaker/generated_symbols/nine/nine.h>
#include <matchmaker/generated_symbols/mns/mns.h>
#include <matchmaker/generated_symbols/dot/dot.h>
#include <matchmaker/generated_symbols/slsh/slsh.h>
#include <matchmaker/generated_symbols/cln/cln.h>
#include <matchmaker/generated_symbols/quot/quot.h>
#include <matchmaker/generated_symbols/hsh/hsh.h>
#include <matchmaker/generated_symbols/dol/dol.h>
#include <matchmaker/generated_symbols/sqt/sqt.h>
#include <matchmaker/generated_symbols/pl/pl.h>
#include <matchmaker/generated_symbols/pls/pls.h>
#include <matchmaker/generated_symbols/gt/gt.h>
#include <matchmaker/generated_symbols/uscr/uscr.h>
#include <matchmaker/generated_symbols/tld/tld.h>

#include <matchmaker/parts_of_speech.h>
#include <matchmaker/longest_words.h>
#include <matchmaker/ordinal_summation.h>
#include <matchmaker/books.h>

#include <matchmaker/generated_matchables/A/A/A_A.h>


using count_func = std::function<int ()>;
using as_longest_func = std::function<int (int)>;
using at_func = std::function<std::string const & (int)>;
using lookup_func = std::function<int (std::string const &, bool *)>;
using ordinal_summation_func = std::function<int (int)>;
using flagged_parts_of_speech_func = std::function<std::vector<int8_t> const & (int)>;
using synonyms_func = std::function<std::vector<int> const & (int)>;
using antonyms_func = std::function<std::vector<int> const & (int)>;
using embedded_func = std::function<std::vector<int> const & (int)>;
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

PROPERTYx18_MATCHABLE(
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
    quot, hsh, dol, sqt, pl, pls, mns, dot, slsh, zero,
    one, two, three, four, five, six, seven, eight, nine,
    cln, gt, uscr, tld,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
)

#define _set_properties(_symbol)                                                                           \
SET_PROPERTY(symbol, _symbol, count, &mm_count_##_symbol)                                                  \
SET_PROPERTY(symbol, _symbol, as_longest, &mm_as_longest_##_symbol)                                        \
SET_PROPERTY(symbol, _symbol, at, &mm_at_##_symbol)                                                        \
SET_PROPERTY(symbol, _symbol, lookup, &mm_lookup_##_symbol)                                                \
SET_PROPERTY(symbol, _symbol, ordinal_summation, &mm_ordinal_summation_##_symbol)                          \
SET_PROPERTY(symbol, _symbol, flagged_parts_of_speech, &mm_flagged_parts_of_speech_##_symbol)              \
SET_PROPERTY(symbol, _symbol, synonyms, &mm_synonyms_##_symbol)                                            \
SET_PROPERTY(symbol, _symbol, antonyms, &mm_antonyms_##_symbol)                                            \
SET_PROPERTY(symbol, _symbol, embedded, &mm_embedded_##_symbol)                                            \
SET_PROPERTY(symbol, _symbol, locations, &mm_locations_##_symbol)                                          \
SET_PROPERTY(symbol, _symbol, is_name, &mm_is_name_##_symbol)                                              \
SET_PROPERTY(symbol, _symbol, is_male_name, &mm_is_male_name_##_symbol)                                    \
SET_PROPERTY(symbol, _symbol, is_female_name, &mm_is_female_name_##_symbol)                                \
SET_PROPERTY(symbol, _symbol, is_place, &mm_is_place_##_symbol)                                            \
SET_PROPERTY(symbol, _symbol, is_compound, &mm_is_compound_##_symbol)                                      \
SET_PROPERTY(symbol, _symbol, is_acronym, &mm_is_acronym_##_symbol)                                        \
SET_PROPERTY(symbol, _symbol, is_phrase, &mm_is_phrase_##_symbol)                                          \
SET_PROPERTY(symbol, _symbol, is_used_in_book, &mm_is_used_in_book_##_symbol)

_set_properties(quot)
_set_properties(hsh)
_set_properties(dol)
_set_properties(sqt)
_set_properties(pl)
_set_properties(pls)
_set_properties(mns)
_set_properties(dot)
_set_properties(slsh)
_set_properties(zero)
_set_properties(one)
_set_properties(two)
_set_properties(three)
_set_properties(four)
_set_properties(five)
_set_properties(six)
_set_properties(seven)
_set_properties(eight)
_set_properties(nine)
_set_properties(cln)
_set_properties(gt)
_set_properties(uscr)
_set_properties(tld)
_set_properties(A)
_set_properties(B)
_set_properties(C)
_set_properties(D)
_set_properties(E)
_set_properties(F)
_set_properties(G)
_set_properties(H)
_set_properties(I)
_set_properties(J)
_set_properties(K)
_set_properties(L)
_set_properties(M)
_set_properties(N)
_set_properties(O)
_set_properties(P)
_set_properties(Q)
_set_properties(R)
_set_properties(S)
_set_properties(T)
_set_properties(U)
_set_properties(V)
_set_properties(W)
_set_properties(X)
_set_properties(Y)
_set_properties(Z)
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
//     //
//     //     3) implement "by_index" iterators in matchable instead of using a second vector. this could
//     //        be cool but violates kiss, destablizes matchable and for what? this? if the terms need it
//     //        then maybe someday...
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

        boundries.push_back(std::make_pair(b, symbol::quot::grab()));
        b += symbol::quot::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::hsh::grab()));
        b += symbol::hsh::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::dol::grab()));
        b += symbol::dol::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::sqt::grab()));
        b += symbol::sqt::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::pl::grab()));
        b += symbol::pl::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::pls::grab()));
        b += symbol::pls::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::mns::grab()));
        b += symbol::mns::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::dot::grab()));
        b += symbol::dot::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::slsh::grab()));
        b += symbol::slsh::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::zero::grab()));
        b += symbol::zero::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::one::grab()));
        b += symbol::one::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::two::grab()));
        b += symbol::two::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::three::grab()));
        b += symbol::three::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::four::grab()));
        b += symbol::four::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::five::grab()));
        b += symbol::five::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::six::grab()));
        b += symbol::six::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::seven::grab()));
        b += symbol::seven::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::eight::grab()));
        b += symbol::eight::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::nine::grab()));
        b += symbol::nine::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::cln::grab()));
        b += symbol::cln::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::gt::grab()));
        b += symbol::gt::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::uscr::grab()));
        b += symbol::uscr::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::tld::grab()));
        b += symbol::tld::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::A::grab()));
        b += symbol::A::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::B::grab()));
        b += symbol::B::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::C::grab()));
        b += symbol::C::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::D::grab()));
        b += symbol::D::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::E::grab()));
        b += symbol::E::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::F::grab()));
        b += symbol::F::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::G::grab()));
        b += symbol::G::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::H::grab()));
        b += symbol::H::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::I::grab()));
        b += symbol::I::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::J::grab()));
        b += symbol::J::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::K::grab()));
        b += symbol::K::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::L::grab()));
        b += symbol::L::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::M::grab()));
        b += symbol::M::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::N::grab()));
        b += symbol::N::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::O::grab()));
        b += symbol::O::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::P::grab()));
        b += symbol::P::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::Q::grab()));
        b += symbol::Q::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::R::grab()));
        b += symbol::R::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::S::grab()));
        b += symbol::S::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::T::grab()));
        b += symbol::T::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::U::grab()));
        b += symbol::U::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::V::grab()));
        b += symbol::V::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::W::grab()));
        b += symbol::W::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::X::grab()));
        b += symbol::X::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::Y::grab()));
        b += symbol::Y::grab().as_count()();

        boundries.push_back(std::make_pair(b, symbol::Z::grab()));
        b += symbol::Z::grab().as_count()();

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
    static std::array<std::pair<symbol::Type, int>, 75> const boundries_by_symbol {
        std::make_pair(symbol::quot::grab(), boundries_by_index[0].first),
        std::make_pair(symbol::hsh::grab(), boundries_by_index[1].first),
        std::make_pair(symbol::dol::grab(), boundries_by_index[2].first),
        std::make_pair(symbol::sqt::grab(), boundries_by_index[3].first),
        std::make_pair(symbol::pl::grab(), boundries_by_index[4].first),
        std::make_pair(symbol::pls::grab(), boundries_by_index[5].first),
        std::make_pair(symbol::mns::grab(), boundries_by_index[6].first),
        std::make_pair(symbol::dot::grab(), boundries_by_index[7].first),
        std::make_pair(symbol::slsh::grab(), boundries_by_index[8].first),
        std::make_pair(symbol::zero::grab(), boundries_by_index[9].first),
        std::make_pair(symbol::one::grab(), boundries_by_index[10].first),
        std::make_pair(symbol::two::grab(), boundries_by_index[11].first),
        std::make_pair(symbol::three::grab(), boundries_by_index[12].first),
        std::make_pair(symbol::four::grab(), boundries_by_index[13].first),
        std::make_pair(symbol::five::grab(), boundries_by_index[14].first),
        std::make_pair(symbol::six::grab(), boundries_by_index[15].first),
        std::make_pair(symbol::seven::grab(), boundries_by_index[16].first),
        std::make_pair(symbol::eight::grab(), boundries_by_index[17].first),
        std::make_pair(symbol::nine::grab(), boundries_by_index[18].first),
        std::make_pair(symbol::cln::grab(), boundries_by_index[19].first),
        std::make_pair(symbol::gt::grab(), boundries_by_index[20].first),
        std::make_pair(symbol::uscr::grab(), boundries_by_index[21].first),
        std::make_pair(symbol::tld::grab(), boundries_by_index[22].first),
        std::make_pair(symbol::A::grab(), boundries_by_index[23].first),
        std::make_pair(symbol::B::grab(), boundries_by_index[24].first),
        std::make_pair(symbol::C::grab(), boundries_by_index[25].first),
        std::make_pair(symbol::D::grab(), boundries_by_index[26].first),
        std::make_pair(symbol::E::grab(), boundries_by_index[27].first),
        std::make_pair(symbol::F::grab(), boundries_by_index[28].first),
        std::make_pair(symbol::G::grab(), boundries_by_index[29].first),
        std::make_pair(symbol::H::grab(), boundries_by_index[30].first),
        std::make_pair(symbol::I::grab(), boundries_by_index[31].first),
        std::make_pair(symbol::J::grab(), boundries_by_index[32].first),
        std::make_pair(symbol::K::grab(), boundries_by_index[33].first),
        std::make_pair(symbol::L::grab(), boundries_by_index[34].first),
        std::make_pair(symbol::M::grab(), boundries_by_index[35].first),
        std::make_pair(symbol::N::grab(), boundries_by_index[36].first),
        std::make_pair(symbol::O::grab(), boundries_by_index[37].first),
        std::make_pair(symbol::P::grab(), boundries_by_index[38].first),
        std::make_pair(symbol::Q::grab(), boundries_by_index[39].first),
        std::make_pair(symbol::R::grab(), boundries_by_index[40].first),
        std::make_pair(symbol::S::grab(), boundries_by_index[41].first),
        std::make_pair(symbol::T::grab(), boundries_by_index[42].first),
        std::make_pair(symbol::U::grab(), boundries_by_index[43].first),
        std::make_pair(symbol::V::grab(), boundries_by_index[44].first),
        std::make_pair(symbol::W::grab(), boundries_by_index[45].first),
        std::make_pair(symbol::X::grab(), boundries_by_index[46].first),
        std::make_pair(symbol::Y::grab(), boundries_by_index[47].first),
        std::make_pair(symbol::Z::grab(), boundries_by_index[48].first),
        std::make_pair(symbol::a::grab(), boundries_by_index[49].first),
        std::make_pair(symbol::b::grab(), boundries_by_index[50].first),
        std::make_pair(symbol::c::grab(), boundries_by_index[51].first),
        std::make_pair(symbol::d::grab(), boundries_by_index[52].first),
        std::make_pair(symbol::e::grab(), boundries_by_index[53].first),
        std::make_pair(symbol::f::grab(), boundries_by_index[54].first),
        std::make_pair(symbol::g::grab(), boundries_by_index[55].first),
        std::make_pair(symbol::h::grab(), boundries_by_index[56].first),
        std::make_pair(symbol::i::grab(), boundries_by_index[57].first),
        std::make_pair(symbol::j::grab(), boundries_by_index[58].first),
        std::make_pair(symbol::k::grab(), boundries_by_index[59].first),
        std::make_pair(symbol::l::grab(), boundries_by_index[60].first),
        std::make_pair(symbol::m::grab(), boundries_by_index[61].first),
        std::make_pair(symbol::n::grab(), boundries_by_index[62].first),
        std::make_pair(symbol::o::grab(), boundries_by_index[63].first),
        std::make_pair(symbol::p::grab(), boundries_by_index[64].first),
        std::make_pair(symbol::q::grab(), boundries_by_index[65].first),
        std::make_pair(symbol::r::grab(), boundries_by_index[66].first),
        std::make_pair(symbol::s::grab(), boundries_by_index[67].first),
        std::make_pair(symbol::t::grab(), boundries_by_index[68].first),
        std::make_pair(symbol::u::grab(), boundries_by_index[69].first),
        std::make_pair(symbol::v::grab(), boundries_by_index[70].first),
        std::make_pair(symbol::w::grab(), boundries_by_index[71].first),
        std::make_pair(symbol::x::grab(), boundries_by_index[72].first),
        std::make_pair(symbol::y::grab(), boundries_by_index[73].first),
        std::make_pair(symbol::z::grab(), boundries_by_index[74].first),
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
//             i += 49;
            // "simplify for speed!
            i -= 48;
        }
        else if (word[0] >= 'A' && word[0] <= 'Z')
        {
//             i -= 'A';  // ascii table location
//             i += 23;   // boundries_by_index location
            // "simplify" for speed!
            i -= 42;
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
        else if (word[0] == '_')
        {
//             i -= '_';
//             i += 21;
            // "simplify for speed!
            i -= 74;
        }
        else if (word[0] == '~')
        {
//             i -= '~';
//             i += 22;
            // "simplify for speed!
            i -= 104;
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
            i = 23; // 'A'
        }

        if (i < 0 || i > 74)
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
    int const prefix_len = strlen(prefix);

    // shortcut empty prefix to everything
    if (prefix_len == 0)
    {
        *start = 0;
        *length = mm_count();
        return;
    }

    auto str_starts_with =
        [&prefix_len](char const * str, int str_len, char const * starts_with)
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
    return books.size();
}


void mm_book_title(int book_index, int const * * book_title, int * word_count)
{
    if (book_index < 0 || book_index >= mm_book_count())
    {
        *book_title = nullptr;
        *word_count = 0;
        return;
    }

    *book_title = books[book_index].title.data();
    *word_count = (int) books[book_index].title.size();
}


void mm_book_author(int book_index, int const * * book_author, int * word_count)
{
    if (book_index < 0 || book_index >= mm_book_count())
    {
        *book_author = nullptr;
        *word_count = 0;
        return;
    }

    *book_author = books[book_index].author.data();
    *word_count = (int) books[book_index].author.size();
}


int mm_chapter_count(int book_index)
{
    if (book_index < 0 || book_index >= mm_book_count())
        return 0;

    return (int) books[book_index].chapters.size();
}


void mm_chapter_title(int book_index, int chapter_index, int const * * chapter_title, int * word_count)
{
    if (book_index < 0 || book_index >= mm_book_count() ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_title = nullptr;
        *word_count = 0;
        return;
    }

    *chapter_title = books[book_index].chapters[chapter_index].title.data();
    *word_count = (int) books[book_index].chapters[chapter_index].title.size();
}


void mm_chapter_subtitle(int book_index, int chapter_index,
                         int const * * chapter_subtitle, int * word_count)
{
    if (book_index < 0 || book_index >= mm_book_count() ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
    {
        *chapter_subtitle = nullptr;
        *word_count = 0;
        return;
    }

    *chapter_subtitle = books[book_index].chapters[chapter_index].subtitle.data();
    *word_count = (int) books[book_index].chapters[chapter_index].subtitle.size();
}


int mm_paragraph_count(int book_index, int chapter_index)
{
    if (book_index < 0 || book_index >= mm_book_count() ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index))
        return 0;

    return (int) books[book_index].chapters[chapter_index].paragraphs.size();
}


int mm_word_count(int book_index, int chapter_index, int paragraph_index)
{
    if (book_index < 0 || book_index >= mm_book_count() ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index))
        return 0;

    return (int) books[book_index].chapters[chapter_index].paragraphs[paragraph_index].size();
}


int mm_word(
    int book_index,
    int chapter_index,
    int paragraph_index,
    int word_index,
    int * parent_phrase,
    int * parent_phrase_word_index
)
{
    if (book_index < 0 || book_index >= mm_book_count() ||
            chapter_index < 0 || chapter_index >= mm_chapter_count(book_index) ||
            paragraph_index < 0 || paragraph_index >= mm_paragraph_count(book_index, chapter_index) ||
            word_index < 0 || word_index >= mm_word_count(book_index, chapter_index, paragraph_index))
        return -1;

    if (nullptr != parent_phrase)
        *parent_phrase = (int) books[book_index].
                               chapters[chapter_index].
                               paragraphs[paragraph_index][word_index].parent_phrase;

    if (nullptr != parent_phrase_word_index)
        *parent_phrase_word_index = (int) books[book_index].
                                          chapters[chapter_index].
                                          paragraphs[paragraph_index][word_index].parent_phrase_start_index;

    return (int) books[book_index].chapters[chapter_index].paragraphs[paragraph_index][word_index].word;
}
