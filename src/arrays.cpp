#include <arrays.h>

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
#define MM_ARRAY location_starts
#include "array.inc"
#define MM_ARRAY location_books
#include "array.inc"
#define MM_ARRAY location_chapters
#include "array.inc"
#define MM_ARRAY location_paragraphs
#include "array.inc"
#define MM_ARRAY location_words
#include "array.inc"
#define MM_ARRAY book_chapter_starts
#include "array.inc"
#define MM_ARRAY chapter_paragraph_starts
#include "array.inc"
#define MM_ARRAY paragraph_word_starts
#include "array.inc"
#define MM_ARRAY text_words
#include "array.inc"
#define MM_ARRAY text_ancestor_starts
#include "array.inc"
#define MM_ARRAY text_ancestors
#include "array.inc"
#define MM_ARRAY text_within_first_ancestor
#include "array.inc"
#define MM_ARRAY text_referenced
#include "array.inc"
#define MM_ARRAY headings
#include "array.inc"
#define MM_ARRAY book_title_starts
#include "array.inc"
#define MM_ARRAY book_author_starts
#include "array.inc"
#define MM_ARRAY chapter_title_starts
#include "array.inc"
#define MM_ARRAY chapter_subtitle_starts
#include "array.inc"
#define MM_ARRAY used_in_book
#include "array.inc"
