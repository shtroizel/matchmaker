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
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

#include <matchable/matchable.h>

#ifdef Q_ONLY
    #include <matchmaker/generated_letters/Q/Q.h>
    #include <matchmaker/generated_letters/q/q.h>
#else
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
#endif

#include <matchmaker/parts_of_speech.h>
#include <matchmaker/longest_words.h>



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
        letter,
#ifdef Q_ONLY
        Q, q
#else
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
#endif
    )

#define _set_properties(_letter)                                                                           \
    SET_PROPERTY(letter, _letter, size, &size_##_letter)                                                   \
    SET_PROPERTY(letter, _letter, as_longest, &as_longest_##_letter)                                       \
    SET_PROPERTY(letter, _letter, at, &at_##_letter)                                                       \
    SET_PROPERTY(letter, _letter, lookup, &lookup_##_letter)                                               \
    SET_PROPERTY(letter, _letter, flagged_parts_of_speech, &flagged_parts_of_speech_##_letter)             \
    SET_PROPERTY(letter, _letter, synonyms, &synonyms_##_letter)                                           \
    SET_PROPERTY(letter, _letter, antonyms, &antonyms_##_letter)

#ifdef Q_ONLY
    _set_properties(Q)
    _set_properties(q)
#else
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
#endif

#undef _set_properties



    int size()
    {
        static int const ret =
            [&](){
                int r{0};
                for (auto const & l : letter::variants())
                    r += l.as_size()();
                return r;
            }();

        return ret;
    }


    int longest_word()
    {
        return LONGEST_WORDS.size() > 0 ? LONGEST_WORDS.front() : 0;
    }


    std::vector<int> const & by_longest()
    {
        return LONGEST_WORDS;
    }


    static std::vector<std::pair<int, letter::Type>> const letter_boundries =
        [](){
            int b{0};
            std::vector<std::pair<int, letter::Type>> boundries;
            for (auto const & l : letter::variants())
            {
                boundries.push_back(std::make_pair(b, l));
                b += l.as_size()();
            }
            return boundries;
        }();


    int as_longest(int index)
    {
        if (index < 0 || index >= size())
            return -1;

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


    int from_longest(int length_index)
    {
        if (length_index < 0 || length_index >= size())
            return -1;

        return LONGEST_WORDS[length_index];
    }


    bool length_location(std::size_t length, int & index, int & count)
    {
        auto iter = LONGEST_WORDS_OFFSETS.find(length);
        if (iter == LONGEST_WORDS_OFFSETS.end())
            return false;

        index = iter->second.first;
        count = iter->second.second - iter->second.first + 1;
        return true;
    }


    std::vector<std::size_t> const & lengths()
    {
        static std::vector<std::size_t> const ret =
            []()
            {
                std::vector<std::size_t> v;
                for (auto const & [l, o] : LONGEST_WORDS_OFFSETS)
                    v.push_back(l);
                return v;
            }();
        return ret;
    }


    std::string const & at(int index)
    {
        static std::string const empty_str;

        if (index < 0 || index >= size())
            return empty_str;

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


    int lookup(std::string const & word, bool * found)
    {
        static std::array<std::pair<letter::Type, int>, 52> const io {
#ifdef Q_ONLY
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::Q::grab(), letter_boundries[0].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
            std::make_pair(letter::q::grab(), letter_boundries[1].first),
#else
            std::make_pair(letter::A::grab(), letter_boundries[0].first),
            std::make_pair(letter::B::grab(), letter_boundries[1].first),
            std::make_pair(letter::C::grab(), letter_boundries[2].first),
            std::make_pair(letter::D::grab(), letter_boundries[3].first),
            std::make_pair(letter::E::grab(), letter_boundries[4].first),
            std::make_pair(letter::F::grab(), letter_boundries[5].first),
            std::make_pair(letter::G::grab(), letter_boundries[6].first),
            std::make_pair(letter::H::grab(), letter_boundries[7].first),
            std::make_pair(letter::I::grab(), letter_boundries[8].first),
            std::make_pair(letter::J::grab(), letter_boundries[9].first),
            std::make_pair(letter::K::grab(), letter_boundries[10].first),
            std::make_pair(letter::L::grab(), letter_boundries[11].first),
            std::make_pair(letter::M::grab(), letter_boundries[12].first),
            std::make_pair(letter::N::grab(), letter_boundries[13].first),
            std::make_pair(letter::O::grab(), letter_boundries[14].first),
            std::make_pair(letter::P::grab(), letter_boundries[15].first),
            std::make_pair(letter::Q::grab(), letter_boundries[16].first),
            std::make_pair(letter::R::grab(), letter_boundries[17].first),
            std::make_pair(letter::S::grab(), letter_boundries[18].first),
            std::make_pair(letter::T::grab(), letter_boundries[19].first),
            std::make_pair(letter::U::grab(), letter_boundries[20].first),
            std::make_pair(letter::V::grab(), letter_boundries[21].first),
            std::make_pair(letter::W::grab(), letter_boundries[22].first),
            std::make_pair(letter::X::grab(), letter_boundries[23].first),
            std::make_pair(letter::Y::grab(), letter_boundries[24].first),
            std::make_pair(letter::Z::grab(), letter_boundries[25].first),
            std::make_pair(letter::a::grab(), letter_boundries[26].first),
            std::make_pair(letter::b::grab(), letter_boundries[27].first),
            std::make_pair(letter::c::grab(), letter_boundries[28].first),
            std::make_pair(letter::d::grab(), letter_boundries[29].first),
            std::make_pair(letter::e::grab(), letter_boundries[30].first),
            std::make_pair(letter::f::grab(), letter_boundries[31].first),
            std::make_pair(letter::g::grab(), letter_boundries[32].first),
            std::make_pair(letter::h::grab(), letter_boundries[33].first),
            std::make_pair(letter::i::grab(), letter_boundries[34].first),
            std::make_pair(letter::j::grab(), letter_boundries[35].first),
            std::make_pair(letter::k::grab(), letter_boundries[36].first),
            std::make_pair(letter::l::grab(), letter_boundries[37].first),
            std::make_pair(letter::m::grab(), letter_boundries[38].first),
            std::make_pair(letter::n::grab(), letter_boundries[39].first),
            std::make_pair(letter::o::grab(), letter_boundries[40].first),
            std::make_pair(letter::p::grab(), letter_boundries[41].first),
            std::make_pair(letter::q::grab(), letter_boundries[42].first),
            std::make_pair(letter::r::grab(), letter_boundries[43].first),
            std::make_pair(letter::s::grab(), letter_boundries[44].first),
            std::make_pair(letter::t::grab(), letter_boundries[45].first),
            std::make_pair(letter::u::grab(), letter_boundries[46].first),
            std::make_pair(letter::v::grab(), letter_boundries[47].first),
            std::make_pair(letter::w::grab(), letter_boundries[48].first),
            std::make_pair(letter::x::grab(), letter_boundries[49].first),
            std::make_pair(letter::y::grab(), letter_boundries[50].first),
            std::make_pair(letter::z::grab(), letter_boundries[51].first),
#endif
        };

        if (word.size() == 0)
            goto lookup_failed;

        if (word[0] < 32 || word[0] > 126)
            goto lookup_failed;

#ifdef Q_ONLY
        if (word[0] != 'Q' && word[0] != 'q')
            goto lookup_failed;
#endif

        {
            int i = word[0];
            if (word[0] < 'A')
            {
                i = 0;
            }
            else if (word[0] <= 'Z')
            {
                i -= 'A';
            }
            else if (word[0] < 'a')
            {
                i = 0;
            }
            else if (word[0] <= 'z')
            {
                i -= 'a';
                i += 26;
            }
            else
            {
                i = 0;
            }

            if (i < 0 || i > 51)
            {
                std::cout << "\nindex_of() :  bug detected! index [" << i
                          << "] is out of bounds! (lookup failed)" << std::endl;
                goto lookup_failed;
            }

            int ret = io[i].first.as_lookup()(word, found);
            ret += io[i].second;
            return ret;
        }

lookup_failed:
        if (nullptr != found)
            *found = false;
        return 0;
    }


    std::vector<std::string> const & all_parts_of_speech()
    {
        static std::vector<std::string> const pos = [](){
            std::vector<std::string> ret;
            for (auto p : parts_of_speech::variants())
                ret.push_back(p.as_pos_desc().as_string());
            return ret;
        }();
        return pos;
    }


    std::vector<int8_t> const & flagged_parts_of_speech(int index)
    {
        static std::vector<int8_t> const empty{};
        if (index < 0 || index >= size())
            return empty;

        auto iter = std::lower_bound(
            letter_boundries.begin(),
            letter_boundries.end(),
            index,
            [](auto const & b, auto const & i){ return b.first <= i; }
        );
        if (iter != letter_boundries.begin())
            --iter;

        auto const & ret = iter->second.as_flagged_parts_of_speech()(index - iter->first);
        if (ret.size() != all_parts_of_speech().size())
            std::cout << "******* flagged_parts_of_speech().size != all_parts_of_speech().size *******"
                      << "buggy code detected!" << std::endl;
        return ret;
    }


    void parts_of_speech(int index, std::vector<std::string const *> & pos)
    {
        pos.clear();
        if (index < 0 || index >= size())
            return;

        auto const & flagged = flagged_parts_of_speech(index);

        for (int i = 0; i < (int) flagged.size(); ++i)
            if (flagged[i] != 0)
                pos.push_back(&all_parts_of_speech()[i]);
    }


    std::vector<int> const & synonyms(int index)
    {
        static std::vector<int> const empty{};
        if (index < 0 || index >= size())
            return empty;

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


    std::vector<int> const & antonyms(int index)
    {
        static std::vector<int> const empty{};
        if (index < 0 || index >= size())
            return empty;

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


    void complete(std::string const & prefix, int & start, int & length)
    {
        if (prefix.size() == 0)
        {
            start = 0;
            length = size();
            return;
        }

        length = 0;

        int index{lookup(prefix, nullptr)};
        start = index;

        for (; index < size(); ++index)
        {
            std::string const & s = at(index);
            if (s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix)
                ++length;
            else
                break;
        }
    }
}
