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



namespace matchmaker
{
    using size_func = std::function<int ()>;
    using as_longest_func = std::function<int (int)>;
    using at_func = std::function<std::string const & (int)>;
    using lookup_func = std::function<int (std::string const &, bool *)>;
    using flagged_parts_of_speech_func = std::function<std::vector<int8_t> const & (int)>;
    using synonyms_func = std::function<std::vector<int> const & (int)>;
    using antonyms_func = std::function<std::vector<int> const & (int)>;

    PROPERTYx7_MATCHABLE(
        size_func,
        size,
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
        letter_snth,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
    )


#define _set_properties(_letter)                                                                           \
    SET_PROPERTY(letter_snth, _letter, size, &size_snth_##_letter)                                         \
    SET_PROPERTY(letter_snth, _letter, as_longest, &as_longest_snth_##_letter)                             \
    SET_PROPERTY(letter_snth, _letter, at, &at_snth_##_letter)                                             \
    SET_PROPERTY(letter_snth, _letter, lookup, &lookup_snth_##_letter)                                     \
    SET_PROPERTY(letter_snth, _letter, flagged_parts_of_speech, &flagged_parts_of_speech_snth_##_letter)   \
    SET_PROPERTY(letter_snth, _letter, synonyms, &synonyms_snth_##_letter)                                 \
    SET_PROPERTY(letter_snth, _letter, antonyms, &antonyms_snth_##_letter)

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


    int size_snth()
    {
        static int const ret =
            [&](){
                int r{0};
                r += size_snth_A();
                r += size_snth_B();
                r += size_snth_C();
                r += size_snth_D();
                r += size_snth_E();
                r += size_snth_F();
                r += size_snth_G();
                r += size_snth_H();
                r += size_snth_I();
                r += size_snth_J();
                r += size_snth_K();
                r += size_snth_L();
                r += size_snth_M();
                r += size_snth_N();
                r += size_snth_O();
                r += size_snth_P();
                r += size_snth_Q();
                r += size_snth_R();
                r += size_snth_S();
                r += size_snth_T();
                r += size_snth_U();
                r += size_snth_V();
                r += size_snth_W();
                r += size_snth_X();
                r += size_snth_Y();
                r += size_snth_Z();
                r += size_snth_a();
                r += size_snth_b();
                r += size_snth_c();
                r += size_snth_d();
                r += size_snth_e();
                r += size_snth_f();
                r += size_snth_g();
                r += size_snth_h();
                r += size_snth_i();
                r += size_snth_j();
                r += size_snth_k();
                r += size_snth_l();
                r += size_snth_m();
                r += size_snth_n();
                r += size_snth_o();
                r += size_snth_p();
                r += size_snth_q();
                r += size_snth_r();
                r += size_snth_s();
                r += size_snth_t();
                r += size_snth_u();
                r += size_snth_v();
                r += size_snth_w();
                r += size_snth_x();
                r += size_snth_y();
                r += size_snth_z();
                return r;
            }();

        return ret;
    }


    static std::vector<std::pair<int, letter_snth::Type>> const letter_boundries =
        [](){
            int b{0};
            std::vector<std::pair<int, letter_snth::Type>> boundries;

            for (auto const & l : letter_snth::variants())
            {
                boundries.push_back(std::make_pair(b, l));
                b += l.as_size()();
            }

            return boundries;
        }();


    int as_longest_snth(int index)
    {
        if (index < 0 || index >= size_snth())
        {
            std::cout << "as_longest_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
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


    std::string const & at_snth(int index)
    {
        if (index < 0 || index >= size_snth())
        {
            std::cout << "at_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
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


    int lookup_snth(std::string const & word, bool * found)
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
            return lookup_snth_a(word, found);

        if (word[depth] < 'A' || (word[depth] > 'Z' && word[depth] < 'a') || word[depth] > 'z')
            goto lookup_failed;

        {
            int i = word[depth];
            if (word[depth] > 'Z')
            {
                i -= 'a';
                i += 26;
            }
            else
            {
                i -= 'A';
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


    std::vector<int8_t> const & flagged_parts_of_speech_snth(int index)
    {
        static std::vector<int8_t> const empty{};

        // only possible when parts_of_speech() buggy (matchmaker.cpp)
        if (index < 0 || index >= size_snth())
        {
            std::cout << "flagged_parts_of_speech_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
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


    std::vector<int> const & synonyms_snth(int index)
    {
        static std::vector<int> const empty{};

        if (index < 0 || index >= size_snth())
        {
            std::cout << "synonyms_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
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


    std::vector<int> const & antonyms_snth(int index)
    {
        static std::vector<int> const empty{};

        if (index < 0 || index >= size_snth())
        {
            std::cout << "antonyms_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
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
}
