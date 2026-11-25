#pragma once


#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchmaker/word_order.h>



MATCHABLE_EXTERN_VARIANTS(word_la, esc_a)
MATCHABLE_FWD(word_lb)
MATCHABLE_SORTKEY(word_lb, mm_word_key)
PROPERTYx13_MATCHABLE(int8_t, pos, ::matchable::VariantRef, syn, ::matchable::VariantRef, ant,
                      int, ordinal_summation, ::matchable::VariantRef, embedded,
                      ::matchable::VariantRef, definition, int8_t, is_name, int8_t, is_male_name,
                      int8_t, is_female_name, int8_t, is_place, int8_t, is_compound, int8_t, is_acronym,
                      int8_t, is_phrase, word_lb, esc_bee, esc_b)
MATCHABLE_VARIANT_PROPERTY_VALUES(word_lb, esc_bee, syn, ::matchable::variant_ref<word_lb::esc_bee>())
MATCHABLE_VARIANT_PROPERTY_VALUES(word_lb, esc_bee, ant, ::matchable::variant_ref<word_la::esc_a>())
