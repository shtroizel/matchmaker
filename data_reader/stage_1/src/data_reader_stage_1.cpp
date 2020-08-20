/*
Copyright (c) 2020, Eric Hyer
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


#include <iostream>
#include <filesystem>
#include <regex>

#include <matchable/matchable.h>

#include <matchable/MatchableMaker.h>
#include <matchmaker/matchmaker.h>


MATCHABLE(
    word_status,
    not_printable_ascii,
    has_spaces,
    has_hyphens,
    has_other_symbols
)


struct entry_51155
{
    std::vector<std::string> syn;
    std::vector<std::string> ant;
};



void print_usage();

void read_51155(
    FILE * input_file,
    std::map<std::string, entry_51155> & contents_51155
);

void update_word_status(word_status::Flags & flags, int & ch);

bool create_stage_1_prefix(
    std::filesystem::directory_entry const & stage_0_header_entry,
    std::map<std::string, entry_51155> const & contents_51155
);


int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        print_usage();
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const STAGE_0_MATCHABLES_DIR{argv[2]};

    std::cout << "creating matchables for stage 1:\n" << std::endl;
    std::cout << "       ------> reading 51155 ..............: " << std::flush;

    std::map<std::string, entry_51155> contents_51155;

    {
        std::string const FN_51155{DATA_DIR + "/51155/51155-0.txt"};
        FILE * f = fopen(FN_51155.c_str(), "r");
        if (f == 0)
        {
            perror(FN_51155.c_str());
            return 1;
        }
        read_51155(f, contents_51155);
        fclose(f);
    }

    std::cout << "done" << std::endl;

    for (auto const & entry : std::filesystem::recursive_directory_iterator(STAGE_0_MATCHABLES_DIR))
        if (entry.is_regular_file())
            if (!create_stage_1_prefix(entry, contents_51155))
                return 1;

    std::cout << "\nstage 1 matchables ready!\n" << std::endl;

    return 0;
}



void print_usage()
{
    std::cout << "program expects 2 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  stage 0 matchables directory\n"
              << std::flush;
}


void read_51155(FILE * input_file, std::map<std::string, entry_51155> & contents_51155)
{
    std::string word;
    std::string cur_key{"-1"};
    word_status::Flags status;
    bool key{false};
    bool syn{false};
    bool ant{false};
    int ch{0};

    while (true)
    {
        key = false;
        syn = false;
        ant = false;

        ch = fgetc(input_file);
        if (ch == 'K')
        {
            ch = fgetc(input_file);
            if (ch == 'E')
            {
                ch = fgetc(input_file);
                if (ch == 'Y')
                {
                    ch = fgetc(input_file);
                    if (ch == ':')
                    {
                        ch = fgetc(input_file);
                        if (ch == ' ')
                            key = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'S')
        {
            ch = fgetc(input_file);
            if (ch == 'Y')
            {
                ch = fgetc(input_file);
                if (ch == 'N')
                {
                    ch = fgetc(input_file);
                    if (ch == ':')
                    {
                        ch = fgetc(input_file);
                        if (ch == ' ')
                            syn = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'A')
        {
            ch = fgetc(input_file);
            if (ch == 'N')
            {
                ch = fgetc(input_file);
                if (ch == 'T')
                {
                    ch = fgetc(input_file);
                    if (ch == ':')
                    {
                        ch = fgetc(input_file);
                        if (ch == ' ')
                            ant = true;
                    }
                }
            }
        }

        if (key)
        {
            word.clear();
            status.clear();
            while (true)
            {
                ch = fgetc(input_file);

                if (ch == EOF)
                    return;

                if (ch == ' ' || ch == '_' || ch == '.' || ch == 10 || ch == 13)
                    break;

                if (ch >= 'A' && ch <= 'Z')
                    ch += 32;

                update_word_status(status, ch);
                word += (char) ch;
            }
            if (word.size() > 0 && !status.is_set(word_status::has_other_symbols::grab()))
                cur_key = word;
            else
                cur_key = "-1";
        }
        else if (syn || ant)
        {
            while (ch != '.')
            {
                word.clear();
                status.clear();

                // read word
                while (true)
                {
                    ch = fgetc(input_file);

                    if (ch == EOF)
                        return;

                    if (ch == ' ' || ch == '.' || ch == ',' || ch == 10 || ch == 13)
                        break;

                    if (ch >= 'A' && ch <= 'Z')
                        ch += 32;

                    update_word_status(status, ch);
                    word += (char) ch;
                }

                // maybe add word
                if (word.size() > 0)
                {
                    if (status.is_set(word_status::not_printable_ascii::grab()))
                        continue;

                    if (status.is_set(word_status::has_spaces::grab()))
                        continue;

                    if (status.is_set(word_status::has_hyphens::grab()))
                        continue;

                    if (status.is_set(word_status::has_other_symbols::grab()))
                        continue;

                    if (syn)
                        contents_51155[cur_key].syn.push_back(word);

                    if (ant)
                        contents_51155[cur_key].ant.push_back(word);
                }
            }
        }

        // skip to end of line
        while (ch != 10 && ch != 13)
        {
            ch = fgetc(input_file);
            if (ch == EOF)
                return;
        }

        ungetc(ch, input_file);

        // skip to beginning of next line
        while (ch == 10 || ch == 13)
        {
            ch = fgetc(input_file);
            if (ch == EOF)
                return;

            if (ch != 10 && ch != 13)
                ungetc(ch, input_file);
        }
    }
}



void update_word_status(word_status::Flags & flags, int & ch)
{
    if (ch < 32 || ch > 126)
    {
        flags.set(word_status::not_printable_ascii::grab());
        ch = '?';
    }
    else if (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z')
    {
        if (ch == ' ')
            flags.set(word_status::has_spaces::grab());
        else if (ch == '-')
            flags.set(word_status::has_hyphens::grab());
        else
            flags.set(word_status::has_other_symbols::grab());
    }
}



bool create_stage_1_prefix(
    std::filesystem::directory_entry const & stage_0_header_entry,
    std::map<std::string, entry_51155> const & contents_51155
)
{
    matchable::MatchableMaker mm;
    matchable::load__status::Type load_status = mm.load(stage_0_header_entry.path());
    if (load_status != matchable::load__status::success::grab())
    {
        std::cout << "attempt to load " << stage_0_header_entry << " failed with: "
                  << load_status << std::endl;
        return false;
    }

    std::string existing_word;
    bool found{false};
    int index{-1};

    for (auto const & [str, m] : mm.matchables)
    {
        if (str.rfind("word", 0) != std::string::npos)
        {
            std::string prefix = str.substr(5);
            prefix = std::regex_replace(
                prefix,
                std::regex("_"),
                " "
            );
            std::cout << "       ------> patching " << prefix;
            for (int i = prefix.size(); i < 5; ++i)
                std::cout << ".";
            std::cout << "..............: " << std::flush;

            std::vector<std::string> syn_vect;
            std::vector<std::string> ant_vect;
            for (auto const & v : m->variants)
            {
                syn_vect.clear();
                ant_vect.clear();

                existing_word = matchable::escapable::unescape_all(v.variant_name);

                auto contents_51155_iter = contents_51155.find(existing_word);
                if (contents_51155_iter != contents_51155.end())
                {
                    for (auto const & s : contents_51155_iter->second.syn)
                    {
                        index = matchmaker::lookup(s, &found);
                        if (found)
                            syn_vect.push_back(std::to_string(index));
                    }
                    for (auto const & a : contents_51155_iter->second.ant)
                    {
                        index = matchmaker::lookup(a, &found);
                        if (found)
                            ant_vect.push_back(std::to_string(index));
                    }
                }

                m->set_propertyvect(v.variant_name, "syn", syn_vect);
                m->set_propertyvect(v.variant_name, "ant", ant_vect);
            }

            break;
        }
    }

    std::string output_fn = stage_0_header_entry.path();
    output_fn = std::regex_replace(
        output_fn,
        std::regex("generated_matchables_stage_0"),
        "generated_matchables_stage_1"
    );
    std::filesystem::path output_path{output_fn};
    std::filesystem::create_directories(output_path.parent_path());

    auto sa_status = mm.save_as(
        output_fn,
        {matchable::save_as__content::matchables::grab()},
        matchable::save_as__spread_mode::wrap::grab()
    );

    std::cout << sa_status << std::endl;

    if (sa_status != matchable::save_as__status::success::grab())
        return false;
    return true;
}
