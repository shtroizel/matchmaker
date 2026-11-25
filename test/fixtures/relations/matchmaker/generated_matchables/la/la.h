#pragma once


#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchmaker/word_order.h>



MATCHABLE_EXTERN_VARIANTS(word_lb, esc_b)
MATCHABLE_EXTERN_VARIANTS(word_lc, esc_c, esc_cat)
MATCHABLE_FWD(word_la)
MATCHABLE_SORTKEY(word_la, mm_word_key)
PROPERTYx13_MATCHABLE(int8_t, pos, ::matchable::VariantRef, syn, ::matchable::VariantRef, ant,
                      int, ordinal_summation, ::matchable::VariantRef, embedded,
                      ::matchable::VariantRef, definition, int8_t, is_name, int8_t, is_male_name,
                      int8_t, is_female_name, int8_t, is_place, int8_t, is_compound, int8_t, is_acronym,
                      int8_t, is_phrase, word_la, esc_a, esc_Ab, esc_abc)
MATCHABLE_VARIANT_PROPERTY_VALUES(word_la, esc_a, syn, ::matchable::variant_ref<word_lc::esc_cat>(),
                                  ::matchable::variant_ref<word_la::esc_Ab>())
MATCHABLE_VARIANT_PROPERTY_VALUES(word_la, esc_abc, embedded, ::matchable::variant_ref<word_la::esc_a>(),
                                  ::matchable::variant_ref<word_lb::esc_b>(),
                                  ::matchable::variant_ref<word_lc::esc_c>())
MATCHABLE_VARIANT_PROPERTY_VALUES(word_la, esc_abc, definition, ::matchable::variant_ref<word_la::esc_a>(),
                                  ::matchable::variant_ref<word_la::esc_a>())
