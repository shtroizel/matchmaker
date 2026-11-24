#include <arrays.h>

#if !defined(STAGE0) && !defined(__has_embed)
#error "embedding the generated arrays takes a compiler with #embed (GCC 15 or Clang 19 and later)"
#endif

static_assert(sizeof(int) == 4);

#define MM_STR_(x) #x
#define MM_STR(x) MM_STR_(x)
#define MM_CAT_(a, b) a##b
#define MM_CAT(a, b) MM_CAT_(a, b)

#define MM_ARRAY from_longest
#include "array.inc"
#define MM_ARRAY as_longest
#include "array.inc"
#define MM_ARRAY lengths
#include "array.inc"
#define MM_ARRAY length_firsts
#include "array.inc"
#define MM_ARRAY length_counts
#include "array.inc"
