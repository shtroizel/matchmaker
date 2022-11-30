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



#include "calculate_longest_words.h"

#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <queue>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include <matchmaker/data_reader_common.h>

#include "Stage1Data.h"




// used for longest words calculation (priority q)
struct LengthIndex
{
    int length{0};
    int index{0};
    friend bool operator<(LengthIndex const & l, LengthIndex const & r);
};
inline bool operator<(LengthIndex const & l, LengthIndex const & r)
{
    if (l.length < r.length)
        return true;
    if (l.length > r.length)
        return false;
    return strcmp(mm_at(r.index, nullptr), mm_at(l.index, nullptr)) < 0;
}


// used for header file prioirty q (largest file size() == highest priority)
struct HeaderEntry
{
    std::filesystem::directory_entry entry;
    friend bool operator<(HeaderEntry const & l, HeaderEntry const & r);
};
inline bool operator<(HeaderEntry const & l, HeaderEntry const & r)
{
    return l.entry.file_size() < r.entry.file_size();
}



bool calculate_longest_words(int progress_steps)
{
    int progress{0};
    int const progress_steps1{progress_steps / 3};
    int const progress_steps2{progress_steps / 3};
    int const progress_steps3{progress_steps - progress_steps1 - progress_steps2};

    std::string const LONGEST_WORDS_HEADER{
        Stage1Data::nil.as_workspace_dir() + "/generated_include/matchmaker/longest_words.h"
    };

    std::vector<int> & by_longest = Stage1Data::nil.as_mutable_by_longest();
    std::map<int, std::pair<int, int>> & offsets = Stage1Data::nil.as_mutable_longest_offsets();
    std::string index;


    // calculate longest words
    std::priority_queue<
        LengthIndex,
        std::vector<LengthIndex>,
        std::less<std::vector<LengthIndex>::value_type>
    > q;
    LengthIndex cur;
    for (int i = 0; i < mm_count(); ++i)
    {
        mm_at(i, &cur.length);
        cur.index = i;
        q.push(cur);

        ++progress;
        if (progress % (mm_count() / progress_steps1) == 0)
            std::cout << "." << std::flush;
    }
    progress = 0;

    // use calculation to create and write header file content
    std::string index_to_print;
    FILE * f = fopen(LONGEST_WORDS_HEADER.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << LONGEST_WORDS_HEADER << std::endl;
        std::cout << "unable to save longest word info, aborting..." << std::endl;
        return 1;
    }

    if (fputs("#pragma once\n"
              "#include <vector>\n"
              "#include <map>\n"
              "inline std::vector<int> const LONGEST_WORDS{\n", f) == EOF)
        goto lw_err;

    // write initialization code for LONGEST_WORDS and populate by_longest
    while (!q.empty())
    {
        if (fputs("    ", f) == EOF)
            goto lw_err;

        index_to_print = std::to_string(q.top().index);
        if (fputs(index_to_print.c_str(), f) == EOF)
            goto lw_err;

        if (fputs(",\n", f) == EOF)
            goto lw_err;

        by_longest.push_back(q.top().index);
        q.pop();

        ++progress;
        if (progress % (mm_count() / progress_steps2) == 0)
            std::cout << "." << std::flush;
    }
    if (fputs("};\n\n", f) == EOF)
        goto lw_err;

    // calculate offsets (requires by_longest)
    progress = 0;
    {
        int cur_end{(int) by_longest.size() - 1};
        int cur_start{cur_end};
        int cur_length{0};
        for (int i = (int) by_longest.size() - 1; i-- >= 0;)
        {
            int by_longest_i_len{0};
            int by_longest_cur_end_len{0};

            mm_at(by_longest[i], &by_longest_i_len);
            mm_at(by_longest[cur_end], &by_longest_cur_end_len);

            if (by_longest_i_len == by_longest_cur_end_len)
            {
                cur_start = i;
            }
            else
            {
                mm_at(by_longest[cur_start], &cur_length);
                offsets[cur_length] = std::make_pair(cur_start, cur_end);
                cur_start = cur_end = i;
            }
            ++progress;
            if (progress % (by_longest.size() / progress_steps3) == 0)
                std::cout << "." << std::flush;
        }
    }

    // write offsets
    if (fputs("inline std::map<int, std::pair<int, int>> const LONGEST_WORDS_OFFSETS{\n", f) == EOF)
        goto lw_err;

    for (auto const & [l, o] : offsets)
    {
        if (fputs("    { ", f) == EOF)
            goto lw_err;

        index = std::to_string(l);
        if (fputs(index.c_str(), f) == EOF)
            goto lw_err;

        if (fputs(", { ", f) == EOF)
            goto lw_err;

        index = std::to_string(o.first);
        if (fputs(index.c_str(), f) == EOF)
            goto lw_err;

        if (fputs(", ", f) == EOF)
            goto lw_err;

        index = std::to_string(o.second);
        if (fputs(index.c_str(), f) == EOF)
            goto lw_err;

        if (fputs("} },\n", f) == EOF)
            goto lw_err;
    }

    if (fputs("};\n\n", f) == EOF)
        goto lw_err;

    fclose(f);
    return true;

lw_err:
    fclose(f);
    return false;
}
