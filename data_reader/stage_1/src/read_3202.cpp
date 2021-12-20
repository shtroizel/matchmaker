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


#include "read_3202.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "Stage1Data.h"


bool read_3202(int progress_steps)
{
    std::string const FN_3202{Stage1Data::nil.as_data_dir() + "/3202/files/mthesaur.txt"};
    FILE * f = fopen(FN_3202.c_str(), "r");
    if (nullptr == f)
    {
        perror(FN_3202.c_str());
        return false;
    }

    std::filesystem::path p{FN_3202.c_str()};
    unsigned long goal{std::filesystem::file_size(p)};
    int progress{0};
    int mod_progress{0};
    int prev_mod_progress{0};

    std::string syn;
    std::string key;

    int ch = 0;
    while (true)
    {
        // read key (up to first ',')
        key.clear();
        while (true)
        {
            ch = fgetc(f);
            ++progress;
            if (ch == EOF)
                goto end;

            if (ch == ',')
                break;

            key += (char) ch;
        }

        // read each comma separated synonym
        do
        {
            // read next synonym
            syn.clear();
            while (true)
            {
                ch = fgetc(f);
                ++progress;
                if (ch == EOF)
                    goto end;

                if (ch == ',' || ch == 10 || ch == 13)
                    break;

                syn += (char) ch;
            }
            if (
                std::find(
                    Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.begin(),
                    Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end(),
                    syn
                ) == Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end()
            )
            Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.push_back(syn);
        }
        while (ch == ',');

        // skip to beginning of next line
        do
        {
            ch = fgetc(f);
            ++progress;
            if (ch == EOF)
                goto end;
        }
        while (ch == 10 || ch == 13);
        ungetc(ch, f);
        --progress;

        mod_progress = progress % (goal / progress_steps);
        if (mod_progress < prev_mod_progress)
            std::cout << "." << std::flush;
        prev_mod_progress = mod_progress;
    }

end:
    std::cout << "." << std::flush;
    fclose(f);
    return true;
}
