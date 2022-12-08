/*
Copyright (c) 2020-2022, shtroizel
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



#include <prepare_prefixes.h>

#include <filesystem>
#include <iostream>

#include <Stage0Data.h>



bool read_prefix(
    FILE * f,
    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> & prefix,
    SerialTask::Type type
);



void prepare_prefixes(SerialTask::Type task)
{
    std::filesystem::path prefix_file_path{Stage0Data::prefixes_filename()};
    task.set_goal(std::filesystem::file_size(prefix_file_path));
    task.set_progress(0);

    Stage0Data::prefixes_2d_to_5d().reserve(1000);
    Stage0Data::prefixes_6d().clear();
    if (Stage0Data::output_dir().empty())
        abort();

    FILE * f = fopen(prefix_file_path.c_str(), "r");
    if (nullptr == f)
    {
        perror(Stage0Data::prefixes_filename().c_str());
        abort();
    }

    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> p;
    std::array<int16_t, 6> p_i = { -1, -1, -1, -1, -1, -1 };

    while (read_prefix(f, p, task))
    {
        std::string p_as_str;
        for (int i = 0; i < Stage0Data::MAX_PREFIX_DEPTH && p[i] != "nil"; ++i)
        {
            if (p[i].empty())
            {
                std::string prefix;
                for (auto const & sym : p)
                {
                    if (prefix.empty())
                        prefix += " ";
                    prefix += "'";
                    prefix += sym;
                    prefix += "'";
                }
                std::cout << "read_prefixes() --> invalid prefix: " << prefix << std::endl;
                abort();
            }
            p_as_str += p[i];
        }

        if (p[0] == "nil")
        {
            std::cout << "read_prefixes() --> invalid prefix: " << p_as_str << std::endl;
            abort();
        }

        // handle 1d prefix (only symbols have depth of 1)
        if (p[1] == "nil")
        {
            // std::cout << "    --> 1d prefix!" << std::endl;
            if (p[0].empty())
            {
                std::cout << "first char of prefix is null (failed to load prefix file)" << std::endl;
                abort();
            }

            Prefix * prefix = Stage0Data::prefix_for_d1_symbol(p[0][0]);
            if (nullptr == prefix)
            {
                std::cout << "depth is 1, failed to get maker for symbol: '" << p[0][0] << "'" << std::endl;
                abort();
            }

            prefix->initialize(p_as_str);
            continue;
        }

        int16_t const maker_index{(int16_t) Stage0Data::prefixes_2d_to_5d().size()};

        p_i[0] = Stage0Data::calc_letter_index(p[0][0]);
        p_i[1] = Stage0Data::calc_letter_index(p[1][0]);
        p_i[2] = Stage0Data::calc_letter_index(p[2][0]);
        p_i[3] = Stage0Data::calc_letter_index(p[3][0]);
        p_i[4] = Stage0Data::calc_letter_index(p[4][0]);
        p_i[5] = Stage0Data::calc_letter_index(p[5][0]);

        // handle 2d prefix
        if (p[2] == "nil")
        {
            std::array<std::array<std::array<int16_t, 52>, 52>, 52> & d3 =
                    Stage0Data::lookup_table_2d_to_5d() [p_i[0]] [p_i[1]];
            for (auto & d4 : d3)
                for (auto & d5 : d4)
                    for (auto & i : d5)
                        i = maker_index;
        }

        // handle 3d prefix
        else if (p[3] == "nil")
        {
            std::array<std::array<int16_t, 52>, 52> & d4 = Stage0Data::lookup_table_2d_to_5d() [p_i[0]] [p_i[1]] [p_i[2]];
            for (auto & d5 : d4)
                for (auto & i : d5)
                    i = maker_index;
        }

        // handle 4d prefix
        else if (p[4] == "nil")
        {
            std::array<int16_t, 52> & d5 = Stage0Data::lookup_table_2d_to_5d() [p_i[0]] [p_i[1]] [p_i[2]] [p_i[3]];
            for (auto & i : d5)
                i = maker_index;
        }

        // handle 5d prefix
        else if (p[5] == "nil")
        {
            Stage0Data::lookup_table_2d_to_5d() [p_i[0]] [p_i[1]] [p_i[2]] [p_i[3]] [p_i[4]] = maker_index;
        }

        // handle 6d prefix
        else
        {
            Prefix p_6d;
            p_6d.initialize(p_as_str);
            Stage0Data::prefixes_6d().push_back(std::move(p_6d));
            continue;
        }

        // finish prefix for depths 2 to 5
        Prefix p_2_to_5d;
        p_2_to_5d.initialize(p_as_str);
        Stage0Data::prefixes_2d_to_5d().push_back(std::move(p_2_to_5d));

        SerialTask::check_progress(task);
    }

    if (!std::is_sorted(Stage0Data::prefixes_6d().begin(), Stage0Data::prefixes_6d().end()))
    {
        std::cout << "prefixes_6d is not sorted!" << std::endl;
        abort();
    }

    fclose(f);
}



bool read_prefix(
    FILE * f,
    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> & prefix,
    SerialTask::Type task
)
{
    for (auto & sym : prefix)
        sym.clear();

    int ch;
    int i{0};

    while (true)
    {
        ++task.as_mutable_progress();

        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        if (ch == ' ')
        {
            ++i;
            if (i < Stage0Data::MAX_PREFIX_DEPTH)
                continue;

            abort();
        }

        prefix[i] += (char) ch;
    }

    return true;
}
