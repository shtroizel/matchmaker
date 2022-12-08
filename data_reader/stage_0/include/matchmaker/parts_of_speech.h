#pragma once

#include <matchable/matchable.h>



MATCHABLE(
    pos_desc,
    Noun,
    Plural,
    Noun_spc_Phrase,
    Verb_spc__pl_usu_spc_participle_pr_,
    Verb_spc__pl_transitive_pr_,
    Verb_spc__pl_intransitive_pr_,
    Adjective,
    Adverb,
    Conjunction,
    Preposition,
    Interjection,
    Pronoun,
    Definite_spc_Article,
    Indefinite_spc_Article,
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

MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, N, pos_desc, pos_desc::Noun::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, p, pos_desc, pos_desc::Plural::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, h, pos_desc, pos_desc::Noun_spc_Phrase::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, V,
                                 pos_desc, pos_desc::Verb_spc__pl_usu_spc_participle_pr_::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, t,
                                 pos_desc, pos_desc::Verb_spc__pl_transitive_pr_::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, i, pos_desc,
                                 pos_desc::Verb_spc__pl_intransitive_pr_::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, A, pos_desc, pos_desc::Adjective::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, v, pos_desc, pos_desc::Adverb::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, C, pos_desc, pos_desc::Conjunction::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, P, pos_desc, pos_desc::Preposition::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, n, pos_desc, pos_desc::Interjection::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, r, pos_desc, pos_desc::Pronoun::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, D, pos_desc, pos_desc::Definite_spc_Article::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, I, pos_desc, pos_desc::Indefinite_spc_Article::grab())
MATCHABLE_VARIANT_PROPERTY_VALUE(parts_of_speech, o, pos_desc, pos_desc::Nominative::grab())
