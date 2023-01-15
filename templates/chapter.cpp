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
// 's n t h' (without spaces) replaced by chapter index (snth)



#include <vector>

#include <matchmaker/books/aoeu_chapter_snth.h>

#include "aoeu_chapter_snth_content.h"



void mm_aoeu_chapter_snth_title(int const * * chapter_title, int * word_count)
{
    *chapter_title = aoeu_chapter_snth_title.data();
    *word_count = (int) aoeu_chapter_snth_title.size();
}


void mm_aoeu_chapter_snth_subtitle(int const * * chapter_subtitle, int * word_count)
{
    *chapter_subtitle = aoeu_chapter_snth_subtitle.data();
    *word_count = (int) aoeu_chapter_snth_subtitle.size();
}


int mm_aoeu_chapter_snth_paragraph_count()
{
    return (int) aoeu_chapter_snth_paragraphs.size();
}


int mm_aoeu_chapter_snth_word_count(int paragraph_index)
{
    if (paragraph_index < 0 || paragraph_index >= (int) aoeu_chapter_snth_paragraphs.size())
        return 0;

    return (int) aoeu_chapter_snth_paragraphs[paragraph_index].size();
}


int mm_aoeu_chapter_snth_word(
    int paragraph_index,
    int word_index,
    int const * * ancestors,
    int * ancestor_count,
    int * index_within_first_ancestor,
    bool * referenced
)
{
    if (paragraph_index < 0 || paragraph_index >= (int) aoeu_chapter_snth_paragraphs.size() ||
            word_index < 0 || word_index >= (int) aoeu_chapter_snth_paragraphs[paragraph_index].size())
        return -1;

    if (nullptr != ancestors && nullptr != ancestor_count)
    {
        *ancestors = aoeu_chapter_snth_paragraphs[paragraph_index][word_index].ancestors.data();
        *ancestor_count = (int) aoeu_chapter_snth_paragraphs[paragraph_index][word_index].ancestors.size();
    }

    if (nullptr != index_within_first_ancestor)
        *index_within_first_ancestor =
                (int) aoeu_chapter_snth_paragraphs[paragraph_index][word_index].index_within_first_ancestor;

    if (nullptr != referenced)
        *referenced = aoeu_chapter_snth_paragraphs[paragraph_index][word_index].referenced;

    return (int) aoeu_chapter_snth_paragraphs[paragraph_index][word_index].word;
}
