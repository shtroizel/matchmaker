// a unit of words for a matchmaker library built with build_and_install.py -e, compiled with
// MATCHABLE_UNLOADABLE so its words go when it is unloaded. UNIT_VERSION 1 adds Abzz1 and Abzz2;
// UNIT_VERSION 2 adds Abzz1 with another ordinal summation, and Abzz3
#include LEAF_TYPE_HEADER



#if UNIT_VERSION == 1
GROW_MATCHABLE(LEAF, esc_Abzz1, esc_Abzz2)
MATCHABLE_VARIANT_PROPERTY_VALUE(LEAF, esc_Abzz1, ordinal_summation, 81)
MATCHABLE_VARIANT_PROPERTY_VALUE(LEAF, esc_Abzz2, ordinal_summation, 82)
#else
GROW_MATCHABLE(LEAF, esc_Abzz1, esc_Abzz3)
MATCHABLE_VARIANT_PROPERTY_VALUE(LEAF, esc_Abzz1, ordinal_summation, 91)
MATCHABLE_VARIANT_PROPERTY_VALUE(LEAF, esc_Abzz3, ordinal_summation, 93)
#endif
