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



#include <read_3202.h>

#include <filesystem>

#include <SerialTask.h>
#include <Stage0Data.h>



void read_3202(SerialTask::Type task)
{
    std::filesystem::path path_FN_3202{Stage0Data::data_dir() + "/3202/files/mthesaur.txt"};

    task.set_goal(std::filesystem::file_size(path_FN_3202));
    task.set_progress(0);

    FILE * input_file = fopen(path_FN_3202.c_str(), "r");
    if (input_file == 0)
    {
        perror(path_FN_3202.c_str());
        abort();
    }

    std::string word;
    parts_of_speech::Flags pos_flags;
    Stage0Data::word_attribute::Flags attributes;

    int ch = 0;
    while (true)
    {
        word.clear();
        pos_flags.clear();
        attributes.clear();

        while (true)
        {
            ch = fgetc(input_file);
            ++task.as_mutable_progress();
            if (ch == EOF || ch == 10 || ch == 13 || ch == ',')
                break;

            Stage0Data::update_word_attribute(attributes, ch);
            word += (char) ch;
        }

        Stage0Data::add_word(word, attributes, pos_flags);

        if (ch == EOF)
            break;

        SerialTask::check_progress(task);
    }

   fclose(input_file);
}
