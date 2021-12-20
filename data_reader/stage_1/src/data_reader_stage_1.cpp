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


// #include <cstring>
//
// #include <chrono>
// #include <filesystem>
#include <iostream>
// #include <mutex>
// #include <queue>
// #include <regex>
// #include <thread>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"
#include "SerialTask.h"

// #include "discover_book_vocabulary.h"
// #include "read_51155.h"
// #include "read_3202.h"
// #include "fill_reflections.h"
// #include "calculate_longest_words.h"
// #include "discover_embedded.h"
// #include "calculate_ordinal_summations.h"
// #include "read_crumbs.h"
// #include "create_books.h"
// #include "patch_headers.h"


MATCHABLE(q_usage, included, omitted, only)


void print_usage();
bool run_task(SerialTask::Type task);



int main(int argc, char ** argv)
{
    if (argc != 4)
    {
        print_usage();
        return 2;
    }

    std::cout << "******* stage 1 data reader *******\n" << std::endl;

    Stage1Data::nil.as_mutable_data_dir() = argv[1];
    Stage1Data::nil.as_mutable_workspace_dir() = argv[2];


//     std::string const BOOKS_HEADER{
//         STAGE_1_WORKSPACE_DIR + "/generated_include/matchmaker/books.h"
//     };

//     std::string const STAGE_1_MATCHABLES_DIR{
//         STAGE_1_WORKSPACE_DIR + "/generated_include/matchmaker/generated_matchables"
//     };

    q_usage::Type q_mode = q_usage::from_string(argv[3]);
    if (q_mode.is_nil())
        q_mode = q_usage::included::grab();

    Stage1Data::nil.as_mutable_used_in_book().reserve(mm_count());
    Stage1Data::nil.as_mutable_by_longest().reserve(mm_count());
    Stage1Data::nil.as_mutable_embedded_words().reserve(mm_count());
    Stage1Data::nil.as_mutable_book_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_chapter_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_paragraph_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_word_indexes().reserve(mm_count());
    for (int i = 0; i < mm_count(); ++i)
    {
        Stage1Data::nil.as_mutable_used_in_book().push_back(Buch::Flags{});
        Stage1Data::nil.as_mutable_embedded_words().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_book_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_chapter_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_paragraph_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_word_indexes().push_back(std::vector<int>());
    }

    SerialTask::Flags tasks;
    for (auto const & task : SerialTask::variants_by_index())
        tasks.set(task);

    q_mode.match({
        {q_usage::only::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_51155::grab());
                tasks.unset(SerialTask::reading_spc_3202::grab());
                tasks.unset(SerialTask::fill_spc_reflections::grab());
            }},
        {q_usage::omitted::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_crumbs::grab());
            }}
    });

    for (auto const & task : tasks.currently_set())
    {
        std::cout << "       ------> " << task << std::flush;
        bool ok = task.as_run()(36 - task.as_string().length());
        if (ok)
        {
            std::cout << ": SUCCESS" << std::endl;
        }
        else
        {
            std::cout << ": FAILED" << std::endl;
            return 1;
        }
    }

    return 0;
}



bool run_task(SerialTask::Type task)
{
    std::cout << "       ------> " << task << std::flush;
    bool ok = task.as_run()(36 - task.as_string().length());
    if (!ok)
    {
        std::cout << ": FAILED" << std::endl;
        return false;
    }
    std::cout << ": SUCCESS" << std::endl;
    return true;
}



void print_usage()
{
    std::cout << "program expects 3 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  stage 1 workspace directory\n"
              << "    [3]  Q mode (one of \"included\", \"omitted\", or \"only\")\n"
              << std::flush;
}
