#include <arrays.h>

#define MM_STR_(x) #x
#define MM_STR(x) MM_STR_(x)
#define MM_CAT_(a, b) a##b
#define MM_CAT(a, b) MM_CAT_(a, b)

#define MM_ARRAY lengths
#include "array.inc"
#define MM_ARRAY length_firsts
#include "array.inc"
#define MM_ARRAY length_counts
#include "array.inc"
#define MM_ARRAY book_chapter_starts
#include "array.inc"
#define MM_ARRAY book_groups
#include "array.inc"
#define MM_ARRAY headings
#include "array.inc"
#define MM_ARRAY book_title_starts
#include "array.inc"
#define MM_ARRAY book_author_starts
#include "array.inc"

#define MM_UNITS from_longest
#include "units.inc"
#define MM_UNITS as_longest
#include "units.inc"
#define MM_UNITS used_in_book
#include "units.inc"
#define MM_UNITS locations
#include "units.inc"
#define MM_UNITS chapters
#include "units.inc"

#ifdef STAGE0
int const arrays::unit_shift{0};
int const arrays::location_shift{0};
#else
int const arrays::unit_shift{
#include "arrays/unit_shift.inc"
};
int const arrays::location_shift{
#include "arrays/location_shift.inc"
};
#endif
