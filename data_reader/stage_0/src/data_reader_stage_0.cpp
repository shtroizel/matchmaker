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



#include <cstring>
#include <filesystem>
#include <iostream>
#include <queue>

#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchable/MatchableMaker.h>

#include <matchmaker/parts_of_speech.h>
#include <matchmaker/data_reader_common.h>

#include <SerialTask.h>
#include <Stage0Data.h>
#include <prepare_prefixes.h>
#include <read_3201.h>
#include <read_3201_acronyms.h>
#include <read_3201_common.h>
#include <read_3201_compound.h>
#include <read_3201_crosswd.h>
#include <read_3201_fnames.h>
#include <read_3201_mnames.h>
#include <read_3201_names.h>
#include <read_3201_places.h>
#include <read_3201_single.h>
#include <read_3202.h>
#include <read_3203.h>
#include <read_crumbs.h>
#include <save_matchables.h>



void print_usage();



int main(int argc, char ** argv)
{
    if (argc < 5 || argc > 6)
    {
        std::cout << "invalid usage!" << std::endl;
        return 2;
    }

    Stage0Data::data_dir() = argv[1];
    Stage0Data::book_vocab_dir() = argv[2];
    Stage0Data::output_dir() = argv[3];
    Stage0Data::prefixes_filename() = argv[4];

    q_usage::Type q_mode;
    if (argc == 6)
    {
        q_mode = q_usage::from_string(argv[5]);
        if (q_mode.is_nil())
        {
            std::cout << "\"" << argv[5] << "\"is not a variant of q_usage. valid variants are:"
                      << std::endl;
            for (auto v : q_usage::variants_by_string())
                std::cout << "  --> " << v << std::endl;

            std::cout << "\nusing default q_usage of: \"" << q_usage::included::grab() << "\"" << std::endl;
        }
    }
    else
    {
        std::cout << "\"q_usage\" not specified, defaulting to \""
                  << q_usage::included::grab() << "\"" << std::endl;
    }
    if (q_mode.is_nil())
        q_mode = q_usage::included::grab();


    SerialTask::Flags tasks;
    for (auto const & task : SerialTask::variants_by_index())
        tasks.set(task);

    q_mode.match({
        {q_usage::only::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_SINGLE_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_COMPOUND_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_COMMON_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_NAMES_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_NAMES_mns_F_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_NAMES_mns_M_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_PLACES_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_CROSSWD_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_CRSWD_spc_D_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3201_spc__pl_ACRONYMS_pr_::grab());
                tasks.unset(SerialTask::reading_spc_3202::grab());
                tasks.unset(SerialTask::reading_spc_3203::grab());
            }},
        {q_usage::omitted::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_Crumbs::grab());
            }}
    });

    SerialTask::reset_progress();
    int const magic_number = 58;
    // std::cout << "         1         2         3         4         5         6         7         8\n"
    //           << "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    //           << std::endl;
    for (SerialTask::Type task : tasks.currently_set())
    {
        std::cout << "       ------> " << task << std::flush;
        task.set_progress_steps(magic_number - task.as_string().length());
        if (nullptr == task.as_run())
        {
            std::cout << "\nrun function is null!" << std::endl;
            abort();
        }
        task.as_run()(task);
        int remaining_dots{0};
        for (int i = task.as_printed_progress(); i < task.as_progress_steps(); ++i)
        {
            ++remaining_dots;
            ++task.as_mutable_progress();
        }
        SerialTask::check_progress(task);
        std::cout << std::endl;
    }

    {
        std::vector<std::string> contributed_by_shtroizel
        {
            "shtroizel", "Shtroizel"
        };
        Stage0Data::word_attribute::Flags word_attributes{Stage0Data::word_attribute::name::grab()};
        parts_of_speech::Flags pos{parts_of_speech::N::grab()};

        for (auto const & s : contributed_by_shtroizel)
            Stage0Data::add_word(s, word_attributes, pos);
    }


    return 0;
}



void print_usage()
{
    std::cout << "program expects 4 or 5 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  book vocabulary directory\n"
              << "    [3]  output directory\n"
              << "    [4]  prefix filename\n"
              << "    [5]  one of the following q modes:";
    for (auto v : q_usage::variants_by_string())
        std::cout << " " << v;
    std::cout << std::endl;
}
