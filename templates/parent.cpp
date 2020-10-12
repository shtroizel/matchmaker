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



// 'aoeu' replaced by letters deliminated by '/'
// 'snth' replaced by letters deliminated by '_'



#include <matchmaker/generated_letters/aoeu/snth.h>

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <matchable/matchable.h>

#include <matchmaker/generated_letters/aoeu/A/snth_A.h>
#include <matchmaker/generated_letters/aoeu/B/snth_B.h>
#include <matchmaker/generated_letters/aoeu/C/snth_C.h>
#include <matchmaker/generated_letters/aoeu/D/snth_D.h>
#include <matchmaker/generated_letters/aoeu/E/snth_E.h>
#include <matchmaker/generated_letters/aoeu/F/snth_F.h>
#include <matchmaker/generated_letters/aoeu/G/snth_G.h>
#include <matchmaker/generated_letters/aoeu/H/snth_H.h>
#include <matchmaker/generated_letters/aoeu/I/snth_I.h>
#include <matchmaker/generated_letters/aoeu/J/snth_J.h>
#include <matchmaker/generated_letters/aoeu/K/snth_K.h>
#include <matchmaker/generated_letters/aoeu/L/snth_L.h>
#include <matchmaker/generated_letters/aoeu/M/snth_M.h>
#include <matchmaker/generated_letters/aoeu/N/snth_N.h>
#include <matchmaker/generated_letters/aoeu/O/snth_O.h>
#include <matchmaker/generated_letters/aoeu/P/snth_P.h>
#include <matchmaker/generated_letters/aoeu/Q/snth_Q.h>
#include <matchmaker/generated_letters/aoeu/R/snth_R.h>
#include <matchmaker/generated_letters/aoeu/S/snth_S.h>
#include <matchmaker/generated_letters/aoeu/T/snth_T.h>
#include <matchmaker/generated_letters/aoeu/U/snth_U.h>
#include <matchmaker/generated_letters/aoeu/V/snth_V.h>
#include <matchmaker/generated_letters/aoeu/W/snth_W.h>
#include <matchmaker/generated_letters/aoeu/X/snth_X.h>
#include <matchmaker/generated_letters/aoeu/Y/snth_Y.h>
#include <matchmaker/generated_letters/aoeu/Z/snth_Z.h>
#include <matchmaker/generated_letters/aoeu/a/snth_a.h>
#include <matchmaker/generated_letters/aoeu/b/snth_b.h>
#include <matchmaker/generated_letters/aoeu/c/snth_c.h>
#include <matchmaker/generated_letters/aoeu/d/snth_d.h>
#include <matchmaker/generated_letters/aoeu/e/snth_e.h>
#include <matchmaker/generated_letters/aoeu/f/snth_f.h>
#include <matchmaker/generated_letters/aoeu/g/snth_g.h>
#include <matchmaker/generated_letters/aoeu/h/snth_h.h>
#include <matchmaker/generated_letters/aoeu/i/snth_i.h>
#include <matchmaker/generated_letters/aoeu/j/snth_j.h>
#include <matchmaker/generated_letters/aoeu/k/snth_k.h>
#include <matchmaker/generated_letters/aoeu/l/snth_l.h>
#include <matchmaker/generated_letters/aoeu/m/snth_m.h>
#include <matchmaker/generated_letters/aoeu/n/snth_n.h>
#include <matchmaker/generated_letters/aoeu/o/snth_o.h>
#include <matchmaker/generated_letters/aoeu/p/snth_p.h>
#include <matchmaker/generated_letters/aoeu/q/snth_q.h>
#include <matchmaker/generated_letters/aoeu/r/snth_r.h>
#include <matchmaker/generated_letters/aoeu/s/snth_s.h>
#include <matchmaker/generated_letters/aoeu/t/snth_t.h>
#include <matchmaker/generated_letters/aoeu/u/snth_u.h>
#include <matchmaker/generated_letters/aoeu/v/snth_v.h>
#include <matchmaker/generated_letters/aoeu/w/snth_w.h>
#include <matchmaker/generated_letters/aoeu/x/snth_x.h>
#include <matchmaker/generated_letters/aoeu/y/snth_y.h>
#include <matchmaker/generated_letters/aoeu/z/snth_z.h>



using count_func = std::function<int ()>;
using as_longest_func = std::function<int (int)>;
using at_func = std::function<std::string const & (int)>;
using lookup_func = std::function<int (std::string const &, bool *)>;
using flagged_parts_of_speech_func = std::function<std::vector<int8_t> const & (int)>;
using synonyms_func = std::function<std::vector<int> const & (int)>;
using antonyms_func = std::function<std::vector<int> const & (int)>;
using is_name_func = std::function<bool (int)>;
using is_male_name_func = std::function<bool (int)>;
using is_female_name_func = std::function<bool (int)>;
using is_place_func = std::function<bool (int)>;
using is_compound_func = std::function<bool (int)>;
using is_acronym_func = std::function<bool (int)>;

PROPERTYx13_MATCHABLE(
    count_func,
    count,
    as_longest_func,
    as_longest,
    at_func,
    at,
    lookup_func,
    lookup,
    flagged_parts_of_speech_func,
    flagged_parts_of_speech,
    synonyms_func,
    synonyms,
    antonyms_func,
    antonyms,
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
    letter_snth,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
)


#define _set_properties(_letter)                                                                           \
SET_PROPERTY(letter_snth, _letter, count, &mm_count_snth_##_letter)                                         \
SET_PROPERTY(letter_snth, _letter, as_longest, &mm_as_longest_snth_##_letter)                              \
SET_PROPERTY(letter_snth, _letter, at, &mm_at_snth_##_letter)                                              \
SET_PROPERTY(letter_snth, _letter, lookup, &mm_lookup_snth_##_letter)                                      \
SET_PROPERTY(letter_snth, _letter, flagged_parts_of_speech, &mm_flagged_parts_of_speech_snth_##_letter)    \
SET_PROPERTY(letter_snth, _letter, synonyms, &mm_synonyms_snth_##_letter)                                  \
SET_PROPERTY(letter_snth, _letter, antonyms, &mm_antonyms_snth_##_letter)                                  \
SET_PROPERTY(letter_snth, _letter, is_name, &mm_is_name_snth_##_letter)                                    \
SET_PROPERTY(letter_snth, _letter, is_male_name, &mm_is_male_name_snth_##_letter)                          \
SET_PROPERTY(letter_snth, _letter, is_female_name, &mm_is_female_name_snth_##_letter)                      \
SET_PROPERTY(letter_snth, _letter, is_place, &mm_is_place_snth_##_letter)                                  \
SET_PROPERTY(letter_snth, _letter, is_compound, &mm_is_compound_snth_##_letter)                            \
SET_PROPERTY(letter_snth, _letter, is_acronym, &mm_is_acronym_snth_##_letter)

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


int mm_count_snth()
{
    static int const ret =
        [&](){
            int r{0};
            r += mm_count_snth_A();
            r += mm_count_snth_B();
            r += mm_count_snth_C();
            r += mm_count_snth_D();
            r += mm_count_snth_E();
            r += mm_count_snth_F();
            r += mm_count_snth_G();
            r += mm_count_snth_H();
            r += mm_count_snth_I();
            r += mm_count_snth_J();
            r += mm_count_snth_K();
            r += mm_count_snth_L();
            r += mm_count_snth_M();
            r += mm_count_snth_N();
            r += mm_count_snth_O();
            r += mm_count_snth_P();
            r += mm_count_snth_Q();
            r += mm_count_snth_R();
            r += mm_count_snth_S();
            r += mm_count_snth_T();
            r += mm_count_snth_U();
            r += mm_count_snth_V();
            r += mm_count_snth_W();
            r += mm_count_snth_X();
            r += mm_count_snth_Y();
            r += mm_count_snth_Z();
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


static std::vector<std::pair<int, letter_snth::Type>> const letter_boundries =
    []()
    {
        int b{0};
        std::vector<std::pair<int, letter_snth::Type>> boundries;

        for (auto const & l : letter_snth::variants())
        {
            boundries.push_back(std::make_pair(b, l));
            b += l.as_count()();
        }

        return boundries;
    }();


int mm_as_longest_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_as_longest_snth(" << index << ") out of bounds with mm_count_snth() of: "
                    << mm_count_snth() << std::endl;
        return 0;
    }

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_as_longest()(index - iter->first);
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
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_at()(index - iter->first);
}


int mm_lookup_snth(std::string const & word, bool * found)
{
    static std::array<std::pair<letter_snth::Type, int>, 52> const io_snth {
        std::make_pair(letter_snth::A::grab(), letter_boundries[0].first),
        std::make_pair(letter_snth::B::grab(), letter_boundries[1].first),
        std::make_pair(letter_snth::C::grab(), letter_boundries[2].first),
        std::make_pair(letter_snth::D::grab(), letter_boundries[3].first),
        std::make_pair(letter_snth::E::grab(), letter_boundries[4].first),
        std::make_pair(letter_snth::F::grab(), letter_boundries[5].first),
        std::make_pair(letter_snth::G::grab(), letter_boundries[6].first),
        std::make_pair(letter_snth::H::grab(), letter_boundries[7].first),
        std::make_pair(letter_snth::I::grab(), letter_boundries[8].first),
        std::make_pair(letter_snth::J::grab(), letter_boundries[9].first),
        std::make_pair(letter_snth::K::grab(), letter_boundries[10].first),
        std::make_pair(letter_snth::L::grab(), letter_boundries[11].first),
        std::make_pair(letter_snth::M::grab(), letter_boundries[12].first),
        std::make_pair(letter_snth::N::grab(), letter_boundries[13].first),
        std::make_pair(letter_snth::O::grab(), letter_boundries[14].first),
        std::make_pair(letter_snth::P::grab(), letter_boundries[15].first),
        std::make_pair(letter_snth::Q::grab(), letter_boundries[16].first),
        std::make_pair(letter_snth::R::grab(), letter_boundries[17].first),
        std::make_pair(letter_snth::S::grab(), letter_boundries[18].first),
        std::make_pair(letter_snth::T::grab(), letter_boundries[19].first),
        std::make_pair(letter_snth::U::grab(), letter_boundries[20].first),
        std::make_pair(letter_snth::V::grab(), letter_boundries[21].first),
        std::make_pair(letter_snth::W::grab(), letter_boundries[22].first),
        std::make_pair(letter_snth::X::grab(), letter_boundries[23].first),
        std::make_pair(letter_snth::Y::grab(), letter_boundries[24].first),
        std::make_pair(letter_snth::Z::grab(), letter_boundries[25].first),
        std::make_pair(letter_snth::a::grab(), letter_boundries[26].first),
        std::make_pair(letter_snth::b::grab(), letter_boundries[27].first),
        std::make_pair(letter_snth::c::grab(), letter_boundries[28].first),
        std::make_pair(letter_snth::d::grab(), letter_boundries[29].first),
        std::make_pair(letter_snth::e::grab(), letter_boundries[30].first),
        std::make_pair(letter_snth::f::grab(), letter_boundries[31].first),
        std::make_pair(letter_snth::g::grab(), letter_boundries[32].first),
        std::make_pair(letter_snth::h::grab(), letter_boundries[33].first),
        std::make_pair(letter_snth::i::grab(), letter_boundries[34].first),
        std::make_pair(letter_snth::j::grab(), letter_boundries[35].first),
        std::make_pair(letter_snth::k::grab(), letter_boundries[36].first),
        std::make_pair(letter_snth::l::grab(), letter_boundries[37].first),
        std::make_pair(letter_snth::m::grab(), letter_boundries[38].first),
        std::make_pair(letter_snth::n::grab(), letter_boundries[39].first),
        std::make_pair(letter_snth::o::grab(), letter_boundries[40].first),
        std::make_pair(letter_snth::p::grab(), letter_boundries[41].first),
        std::make_pair(letter_snth::q::grab(), letter_boundries[42].first),
        std::make_pair(letter_snth::r::grab(), letter_boundries[43].first),
        std::make_pair(letter_snth::s::grab(), letter_boundries[44].first),
        std::make_pair(letter_snth::t::grab(), letter_boundries[45].first),
        std::make_pair(letter_snth::u::grab(), letter_boundries[46].first),
        std::make_pair(letter_snth::v::grab(), letter_boundries[47].first),
        std::make_pair(letter_snth::w::grab(), letter_boundries[48].first),
        std::make_pair(letter_snth::x::grab(), letter_boundries[49].first),
        std::make_pair(letter_snth::y::grab(), letter_boundries[50].first),
        std::make_pair(letter_snth::z::grab(), letter_boundries[51].first),
    };

    static std::string const snth{"snth"};
    static int const depth = std::count(snth.begin(), snth.end(), '_') + 1;

    if (word.size() < 1)
        goto lookup_failed;

    if ((int) word.size() <= depth)
        return mm_lookup_snth_A(word, found);

    if (word[depth] < 32 || word[depth] > 126)
        goto lookup_failed;

    {
        int i = word[depth];
        if (word[depth] < 'A')
        {
            i = 0;
        }
        else if (word[depth] <= 'Z')
        {
            i -= 'A';
        }
        else if (word[depth] < 'a')
        {
            i = 0;
        }
        else if (word[depth] <= 'z')
        {
            i -= 'a';
            i += 26;
        }
        else // word[depth] < 127
        {
            i = 0;
        }

        if (i < 0 || i > 51)
        {
            std::cout << "\nlookup_snth() :  bug detected! index [" << i
                        << "] is out of bounds! (lookup_snth() failed)" << std::endl;
            goto lookup_failed;
        }

        int ret = io_snth[i].first.as_lookup()(word, found);
        ret += io_snth[i].second;
        return ret;
    }

lookup_failed:
    if (nullptr != found)
        *found = false;
    return 0;
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
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_flagged_parts_of_speech()(index - iter->first);
}


bool mm_is_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_name()(index - iter->first) != 0;
}


bool mm_is_male_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_male_name()(index - iter->first) != 0;
}


bool mm_is_female_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_female_name()(index - iter->first) != 0;
}


bool mm_is_place_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );
    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_place()(index - iter->first) != 0;
}


bool mm_is_compound_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_compound()(index - iter->first) != 0;
}


bool mm_is_acronym_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
        return false;

    auto iter = std::lower_bound(
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_is_acronym()(index - iter->first) != 0;
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
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_synonyms()(index - iter->first);
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
                    letter_boundries.begin(),
                    letter_boundries.end(),
                    index,
                    [](auto const & b, auto const & i){ return b.first <= i; }
                );

    if (iter != letter_boundries.begin())
        --iter;

    return iter->second.as_antonyms()(index - iter->first);
}
