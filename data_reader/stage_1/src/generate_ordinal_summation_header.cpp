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


#include "generate_ordinal_summation_header.h"

#include <iostream>
#include <string>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



int const MAX_ORD_SUM{21777};


bool generate_ordinal_summation_header(int progress_steps)
{
    int progress{0};

    std::array<std::vector<int>, MAX_ORD_SUM> all_ord_sums;
    {
        int word_ord_sum = 0;
        for (int word_index = 0; word_index < mm_count(); ++word_index)
        {
            word_ord_sum = mm_ordinal_summation(word_index);
            if (word_ord_sum >= 0 && word_ord_sum < MAX_ORD_SUM)
                all_ord_sums[word_ord_sum].push_back(word_index);
        }
    }

    std::string const ORDINAL_SUMMATION_HEADER{
        Stage1Data::nil.as_workspace_dir() + "/generated_include/matchmaker/ordinal_summation.h"
    };

    FILE * f = fopen(ORDINAL_SUMMATION_HEADER.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << ORDINAL_SUMMATION_HEADER << std::endl;
        std::cout << "unable to save ordinal summation info, aborting..." << std::endl;
        return 1;
    }

    std::string max_ord_sum_str = std::to_string(MAX_ORD_SUM);
    if (fputs("#pragma once\n"
                "#include <array>\n"
                "#include <vector>\n"
                "inline std::array<std::vector<int>, ", f) == EOF)
        goto ord_sum_err;


    if (fputs(max_ord_sum_str.c_str(), f) == EOF)
        goto ord_sum_err;

    if (fputs("> const ORDINAL_SUMMATIONS{{\n", f) == EOF)
        goto ord_sum_err;

    for (auto const & words : all_ord_sums)
    {
        ++progress;
        if (progress % (MAX_ORD_SUM / progress_steps) == 0)
            std::cout << "." << std::flush;

        if (fputs("    { ", f) == EOF)
            goto ord_sum_err;

        for (auto word_index : words)
        {
            std::string word_str = std::to_string(word_index);
            word_str += ", ";
            if (fputs(word_str.c_str(), f) == EOF)
                goto ord_sum_err;
        }
        if (fputs("},\n", f) == EOF)
            goto ord_sum_err;
    }
    if (fputs("}};\n\n", f) == EOF)
        goto ord_sum_err;

    fclose(f);
    return true;

ord_sum_err:
    fclose(f);
    return false;
}
