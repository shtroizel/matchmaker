# cmake -DMATCHMAKER_ROOT=<root> -DOUT=<dir> -P leaf_type_check.cmake
# Checks mm_leaf_type (workspace_common), which takes a leaf's type for the header that -e installs: the
# sort key and the properties without the words, found after any amount of declarations of other leaves'
# words. It ends with the error for a header that declares no such type, which the test expects.

include(${MATCHMAKER_ROOT}/workspace_common/leaf_type.cmake)

set(others "")
foreach(n RANGE 2000)
    string(APPEND others "MATCHABLE_EXTERN_VARIANTS(word_other, esc_word${n}, esc_another_word${n})\n")
endforeach()
string(LENGTH "${others}" others_length)
if(others_length LESS 100000)
    message(FATAL_ERROR "only ${others_length} bytes before the type")
endif()

set(properties "PROPERTYx2_MATCHABLE(int8_t, pos, ::matchable::VariantRef, syn, word_leaf")
file(WRITE ${OUT}/leaf.h
    "#pragma once\n#include <matchable/matchable.h>\n\n${others}MATCHABLE_FWD(word_leaf)\n"
    "MATCHABLE_SORTKEY(word_leaf, mm_word_key)\n${properties}, esc_a, esc_b)\n"
    "GROW_MATCHABLE(word_leaf, esc_c)\n"
    "MATCHABLE_VARIANT_PROPERTY_VALUES(word_leaf, esc_a, pos, 0, 1)\n"
)
mm_leaf_type(declaration ${OUT}/leaf.h leaf)
set(expected "MATCHABLE_SORTKEY(word_leaf, mm_word_key)\n${properties})\n")
if(NOT declaration STREQUAL expected)
    message(FATAL_ERROR "the type of a leaf with a sort key is:\n${declaration}")
endif()

file(WRITE ${OUT}/plain.h "${properties}, esc_a)\n")
mm_leaf_type(declaration ${OUT}/plain.h leaf)
if(NOT declaration STREQUAL "${properties})\n")
    message(FATAL_ERROR "the type of a leaf without a sort key is:\n${declaration}")
endif()

mm_leaf_type(declaration ${OUT}/plain.h other)
