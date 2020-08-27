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
#include <queue>
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


// used for longest words calculation
struct LengthIndex
{
    int length{0};
    int index{0};
    friend bool operator<(LengthIndex const & l, LengthIndex const & r);
};
inline bool operator<(LengthIndex const & l, LengthIndex const & r) { return l.length < r.length; }


void print_usage();

void read_51155(
    FILE * input_file,
    std::map<std::string, entry_51155> & contents_51155
);

void update_word_status(word_status::Flags & flags, int & ch);

bool patch_matchable_header(
    std::filesystem::directory_entry const & matchable_header,
    std::map<std::string, entry_51155> const & contents_51155,
    std::vector<int> const & by_longest,
    std::map<int, int> const & longest_offsets
);



int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        print_usage();
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const STAGE_1_WORKSPACE_DIR{argv[2]};
    std::string const LONGEST_WORDS_HEADER{
        STAGE_1_WORKSPACE_DIR + "/generated_include/matchmaker/longest_words.h"
    };
    std::string const STAGE_1_MATCHABLES_DIR{
        STAGE_1_WORKSPACE_DIR + "/generated_include/matchmaker/generated_matchables"
    };

    std::map<std::string, entry_51155> contents_51155;

    std::cout << "******* stage 1 data reader *******\n" << std::endl;

    // process 51155
    std::cout << "       ------> reading 51155 ..............: " << std::flush;
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

    std::cout << "       ------> calculating longest words...: " << std::flush;
    std::vector<int> by_longest;
    {
        // calculate longest words
        std::priority_queue<
            LengthIndex,
            std::vector<LengthIndex>,
            std::less<std::vector<LengthIndex>::value_type>
        > q;
        LengthIndex cur;
        for (int i = 0; i < matchmaker::size(); ++i)
        {
            cur.length = matchmaker::at(i).size();
            cur.index = i;
            q.push(cur);
        }

        // use calculation to create and save header file content
        std::string index_to_print;
        FILE * f = fopen(LONGEST_WORDS_HEADER.c_str(), "w");
        if (nullptr == f)
        {
            std::cout << "failed to open " << LONGEST_WORDS_HEADER << std::endl;
            std::cout << "unable to save longest word info, aborting..." << std::endl;
            return 1;
        }
        if (fputs("#pragma once\ninline std::vector<int> const LONGEST_WORDS{\n", f) == EOF)
        {
            std::cout << "first write " << std::endl;
            goto err;
        }
        while (!q.empty())
        {
            if (fputs("    ", f) == EOF)
            {
                std::cout << "second write " << std::endl;
                goto err;
            }
            index_to_print = std::to_string(q.top().index);
            if (fputs(index_to_print.c_str(), f) == EOF)
            {
                std::cout << "third write " << std::endl;
                goto err;
            }
            if (fputs(",\n", f) == EOF)
            {
                std::cout << "fourth write " << std::endl;
                goto err;
            }

            by_longest.push_back(q.top().index);
            q.pop();
        }
        if (fputs("};\n", f) == EOF)
        {
            std::cout << "fifth write " << std::endl;
            goto err;
        }

        fclose(f);
        goto end;
err:
        std::cout << "failed" << std::endl;
        fclose(f);
        return 1;
end:
        ;
    }
    std::cout << "done" << std::endl;
    std::cout << "       ------> calculating offsets.........: " << std::flush;

    std::map<int, int> offsets;
    int longest_length = matchmaker::at(by_longest[0]).size();
    for (int offset = 1; offset <= longest_length; ++offset)
    {
        int start = (int) by_longest.size();
        bool found{false};
        while (start > 0)
        {
            --start;
            if ((int) matchmaker::at(by_longest[start]).size() == offset)
            {
                found = true;
                break;
            }
        }
        if (found)
            offsets[offset] = start;
    }
    std::cout << "done" << std::endl;


    for (auto const & entry : std::filesystem::recursive_directory_iterator(STAGE_1_MATCHABLES_DIR))
        if (entry.is_regular_file())
            if (!patch_matchable_header(entry, contents_51155, by_longest, offsets))
                return 1;

    std::cout << "\nstage 1 matchables ready!\n" << std::endl;
    return 0;
}



void print_usage()
{
    std::cout << "program expects 2 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  stage 1 workspace directory\n"
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



bool patch_matchable_header(
    std::filesystem::directory_entry const & matchable_header,
    std::map<std::string, entry_51155> const & contents_51155,
    std::vector<int> const & by_longest,
    std::map<int, int> const & longest_offsets
)
{
    matchable::MatchableMaker mm;
    matchable::load__status::Type load_status = mm.load(matchable_header.path());
    if (load_status != matchable::load__status::success::grab())
    {
        std::cout << "attempt to load " << matchable_header << " failed with: "
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
            for (int i = prefix.size(); i < 11; ++i)
                std::cout << ".";
            std::cout << "........: " << std::flush;

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

                {
                    for (int i = longest_offsets.at(existing_word.size()); i-- > 0;)
                    {
                        if (matchmaker::at(by_longest[i]).size() != existing_word.size())
                            break;

                        if (matchmaker::at(by_longest[i]) == existing_word)
                        {
                            m->set_property(v.variant_name, "by_longest_index", std::to_string(i));
                            break;
                        }
                    }
                }
            }

            break;
        }
    }

    auto sa_status = mm.save_as(
        matchable_header.path(),
        {matchable::save_as__content::matchables::grab()},
        matchable::save_as__spread_mode::wrap::grab()
    );

    std::cout << sa_status << std::endl;

    if (sa_status != matchable::save_as__status::success::grab())
        return false;
    return true;
}
