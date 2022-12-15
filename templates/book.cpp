/*
Copyright (c) 2020-2023, shtroizel
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



// 'a o e u' (without spaces) replaced by book title (aoeu)



#include <matchmaker/books/aoeu.h>

#include <vector>
#include <functional>

#include "aoeu_content.h"



void mm_aoeu_title(int const * * book_title, int * word_count)
{
    *book_title = aoeu_book_title.data();
    *word_count = (int) aoeu_book_title.size();
}


void mm_aoeu_author(int const * * book_author, int * word_count)
{
    *book_author = aoeu_book_author.data();
    *word_count = (int) aoeu_book_author.size();
}


int mm_aoeu_chapter_count()
{
    return aoeu_CHAPTER_COUNT;
}


void mm_aoeu_chapter_title(int chapter_index, int const * * chapter_title, int * word_count)
{
    if (chapter_index < 0 || chapter_index >= aoeu_CHAPTER_COUNT)
    {
        *chapter_title = nullptr;
        *word_count = 0;
        return;
    }
    aoeu_chapter_title_funcs[chapter_index](chapter_title, word_count);
}


void mm_aoeu_chapter_subtitle(int chapter_index, int const * * chapter_subtitle, int * word_count)
{
    if (chapter_index < 0 || chapter_index >= aoeu_CHAPTER_COUNT)
    {
        *chapter_subtitle = nullptr;
        *word_count = 0;
        return;
    }

    aoeu_chapter_subtitle_funcs[chapter_index](chapter_subtitle, word_count);
}


int mm_aoeu_paragraph_count(int chapter_index)
{
    if (chapter_index < 0 || chapter_index >= aoeu_CHAPTER_COUNT)
        return 0;

    return aoeu_paragraph_count_funcs[chapter_index]();
}


int mm_aoeu_word_count(int chapter_index, int paragraph_index)
{
    if (chapter_index < 0 || chapter_index >= aoeu_CHAPTER_COUNT ||
            paragraph_index < 0 || paragraph_index >= mm_aoeu_paragraph_count(chapter_index))
        return 0;

    return aoeu_word_count_funcs[chapter_index](paragraph_index);
}


int mm_aoeu_word(
    int chapter_index,
    int paragraph_index,
    int word_index,
    int const * * ancestors,
    int * ancestor_count,
    int * index_within_first_ancestor
)
{
    if (chapter_index < 0 || chapter_index >= mm_aoeu_chapter_count() ||
            paragraph_index < 0 || paragraph_index >= mm_aoeu_paragraph_count(chapter_index) ||
            word_index < 0 || word_index >= mm_aoeu_word_count(chapter_index, paragraph_index))
        return -1;

    return aoeu_word_funcs[chapter_index](paragraph_index,
                                            word_index,
                                            ancestors,
                                            ancestor_count,
                                            index_within_first_ancestor);
}
