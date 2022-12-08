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



#include <read_3201.h>

#include <filesystem>

#include <bits/types/FILE.h>

#include <SerialTask.h>
#include <Stage0Data.h>



bool read_3201_default_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
);



void read_3201_default(
    std::filesystem::path const file_path,
    Stage0Data::word_attribute::Flags const & base_attributes,
    SerialTask::Type task
)
{
    task.set_goal(std::filesystem::file_size(file_path));
    task.set_progress(0);

    FILE * input_file = fopen(file_path.c_str(), "r");
    if (input_file == 0)
    {
        perror(file_path.c_str());
        abort();
    }

    std::string word;
    Stage0Data::word_attribute::Flags attributes;

    while (true)
    {
        attributes = base_attributes;

        if (!read_3201_default_line(input_file, word, attributes, task))
            break;

        if (word.size() == 0)
            continue;

        if (attributes.is_set(Stage0Data::word_attribute::compound::grab()) &&
                word.find('-') == std::string::npos && word.find(' ') == std::string::npos)
            attributes.unset(Stage0Data::word_attribute::compound::grab());

        Stage0Data::add_word(word, attributes);

        SerialTask::check_progress(task);
    }

    fclose(input_file);
}



bool read_3201_default_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
)
{
    word.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();
        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
        {
            while (true)
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == EOF)
                    return false;

                if (ch != 10 && ch != 13)
                {
                    ungetc(ch, f);
                    break;
                }
            }
            break;
        }

        Stage0Data::update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    return true;
}
