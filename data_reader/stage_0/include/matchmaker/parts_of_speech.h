#pragma once


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


#include <matchable/matchable.h>


MATCHABLE(
    pos_desc,
    Noun,
    Plural,
    Noun_Phrase,
    Verb_spc__pl_usu_spc_part_pr_,
    Verb_spc__pl_trans_pr_,
    Verb_spc__pl_intrans_pr_,
    Adjective,
    Adverb,
    Conjunction,
    Preposition,
    Interjection,
    Pronoun,
    Def_spc_Article,
    Indef_spc_Article,
    Nominative
);

PROPERTYx1_MATCHABLE(
    pos_desc::Type,
    pos_desc,
    parts_of_speech,
    N,
    p,
    h,
    V,
    t,
    i,
    A,
    v,
    C,
    P,
    n, // !
    r,
    D,
    I,
    o
)

SET_PROPERTY(parts_of_speech, N, pos_desc, pos_desc::Noun::grab())
SET_PROPERTY(parts_of_speech, p, pos_desc, pos_desc::Plural::grab())
SET_PROPERTY(parts_of_speech, h, pos_desc, pos_desc::Noun_Phrase::grab())
SET_PROPERTY(parts_of_speech, V, pos_desc, pos_desc::Verb_spc__pl_usu_spc_part_pr_::grab())
SET_PROPERTY(parts_of_speech, t, pos_desc, pos_desc::Verb_spc__pl_trans_pr_::grab())
SET_PROPERTY(parts_of_speech, i, pos_desc, pos_desc::Verb_spc__pl_intrans_pr_::grab())
SET_PROPERTY(parts_of_speech, A, pos_desc, pos_desc::Adjective::grab())
SET_PROPERTY(parts_of_speech, v, pos_desc, pos_desc::Adverb::grab())
SET_PROPERTY(parts_of_speech, C, pos_desc, pos_desc::Conjunction::grab())
SET_PROPERTY(parts_of_speech, P, pos_desc, pos_desc::Preposition::grab())
SET_PROPERTY(parts_of_speech, n, pos_desc, pos_desc::Interjection::grab())
SET_PROPERTY(parts_of_speech, r, pos_desc, pos_desc::Pronoun::grab())
SET_PROPERTY(parts_of_speech, D, pos_desc, pos_desc::Def_spc_Article::grab())
SET_PROPERTY(parts_of_speech, I, pos_desc, pos_desc::Indef_spc_Article::grab())
SET_PROPERTY(parts_of_speech, o, pos_desc, pos_desc::Nominative::grab())
