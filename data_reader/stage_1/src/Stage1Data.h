#pragma once
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


#ifdef MATCHABLE_OMIT_BY_INDEX
matchable with by_index required for stage 1 reader
#endif
#include <matchable/matchable.h>

#include <functional>
#include <map>


struct BookWord
{
    int word{-1};
    int parent_phrase{-1};
    int parent_phrase_start_index{-1};
};

struct Chapter
{
    std::vector<int> title;
    std::vector<int> subtitle;
    std::vector<std::vector<BookWord>> paragraphs;
};

struct Book
{
    std::vector<int> title;
    std::vector<int> author;
    std::vector<Chapter> chapters;
};

struct SynAnt
{
    std::vector<std::string> syn;
    std::vector<std::string> ant;
};


MATCHABLE(
    word_status,
    not_printable_ascii,
    has_spaces,
    has_hyphens,
    has_other_symbols
)


MATCHABLE(
    Buch,

    Crumbs
)


using SynAntTable = std::map<std::string, SynAnt>;
using IndexTable = std::vector<std::vector<int>>;
using UsedInBookTable = std::vector<Buch::Flags>;
using LongestOffsetsMap = std::map<int, std::pair<int, int>>;

PROPERTYx12_MATCHABLE(
    std::string, workspace_dir,
    std::string, data_dir,
    SynAntTable, syn_ant_table,
    IndexTable, embedded_words,
    UsedInBookTable, used_in_book,
    std::vector<int>, by_longest,
    LongestOffsetsMap, longest_offsets,
    std::vector<Book>, books,
    IndexTable, book_indexes,
    IndexTable, chapter_indexes,
    IndexTable, paragraph_indexes,
    IndexTable, word_indexes,

    Stage1Data
)
