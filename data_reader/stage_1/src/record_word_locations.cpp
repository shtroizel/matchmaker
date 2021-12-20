/*
Copyright (c) 2022, shtroizel
All rights reserved without prejudice.

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



#include "record_word_locations.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"


bool record_word_locations(int progress_steps)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    IndexTable & book_indexes = Stage1Data::nil.as_mutable_book_indexes();
    IndexTable & chapter_indexes = Stage1Data::nil.as_mutable_chapter_indexes();
    IndexTable & paragraph_indexes = Stage1Data::nil.as_mutable_paragraph_indexes();
    IndexTable & word_indexes = Stage1Data::nil.as_mutable_word_indexes();

    int location_count{0};
    for (int b = 0; b < (int) books.size(); ++b)
        for (int ch = 0; ch < (int) books[b].chapters.size(); ++ch)
            for (int p = 0; p < (int) books[b].chapters[ch].paragraphs.size(); ++p)
                location_count += (int) books[b].chapters[ch].paragraphs[p].size();

    int locations_processed{0};

    char const * s = nullptr;
    int len{0};
    for (int b = 0; b < (int) books.size(); ++b)
    {
        for (int ch = 0; ch < (int) books[b].chapters.size(); ++ch)
        {
            for (int p = 0; p < (int) books[b].chapters[ch].paragraphs.size(); ++p)
            {
                for (int w = 0; w < (int) books[b].chapters[ch].paragraphs[p].size(); ++w)
                {
                    BookWord const & bw = books[b].chapters[ch].paragraphs[p][w];

                    s = mm_at(bw.word, &len);
                    if (len != 1 || s[0] == 'Q' || s[0] == 'q')
                    {
                        book_indexes[bw.word].push_back(b);
                        chapter_indexes[bw.word].push_back(ch);
                        paragraph_indexes[bw.word].push_back(p);
                        word_indexes[bw.word].push_back(w);
                    }

                    if (bw.parent_phrase >= 0 && bw.parent_phrase_start_index >= 0)
                    {
                        bool parent_added = false;
                        for (size_t i = 0; !parent_added && i < book_indexes[bw.parent_phrase].size(); ++i)
                        {
                            if (word_indexes[bw.parent_phrase][i] == bw.parent_phrase_start_index &&
                                    paragraph_indexes[bw.parent_phrase][i] == p &&
                                    chapter_indexes[bw.parent_phrase][i] == ch &&
                                    book_indexes[bw.parent_phrase][i] == b)
                                parent_added = true;
                        }

                        if (!parent_added)
                        {
                            book_indexes[bw.parent_phrase].push_back(b);
                            chapter_indexes[bw.parent_phrase].push_back(ch);
                            paragraph_indexes[bw.parent_phrase].push_back(p);
                            word_indexes[bw.parent_phrase].push_back(bw.parent_phrase_start_index);
                        }
                    }

                    ++locations_processed;
                    if (locations_processed % (location_count / progress_steps) == 0)
                        std::cout << "." << std::flush;
                }
            }
        }
    }

    return true;
}
