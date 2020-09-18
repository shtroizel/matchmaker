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


#include <chrono>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <queue>
#include <regex>
#include <thread>

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
    return matchmaker::at(r.index) < matchmaker::at(l.index);
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


void print_usage();

void read_51155(
    FILE * input_file,
    std::map<std::string, entry_51155> & contents_51155
);

void update_word_status(word_status::Flags & flags, int & ch);

bool patch_matchable_header(
    std::vector<HeaderEntry> const & matchable_headers,
    std::map<std::string, entry_51155> const & contents_51155,
    std::vector<int> const & by_longest,
    std::map<int, std::pair<int, int>> const & longest_offsets
);


std::mutex patch_matchable_header_mutex;
int processed_headers{0};
int header_count{0};


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
    std::map<int, std::pair<int, int>> offsets;
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

        // use calculation to create and write header file content
        std::string index_to_print;
        FILE * f = fopen(LONGEST_WORDS_HEADER.c_str(), "w");
        if (nullptr == f)
        {
            std::cout << "failed to open " << LONGEST_WORDS_HEADER << std::endl;
            std::cout << "unable to save longest word info, aborting..." << std::endl;
            return 1;
        }

        if (fputs("#pragma once\n#include <vector>\n#include <map>\n"
                  "inline std::vector<int> const LONGEST_WORDS{\n", f) == EOF)
            goto err;

        // write initialization code for LONGEST_WORDS
        while (!q.empty())
        {
            if (fputs("    ", f) == EOF)
                goto err;

            index_to_print = std::to_string(q.top().index);
            if (fputs(index_to_print.c_str(), f) == EOF)
                goto err;

            if (fputs(",\n", f) == EOF)
                goto err;

            by_longest.push_back(q.top().index);
            q.pop();
        }
        if (fputs("};\n\n", f) == EOF)
            goto err;

        // calculate offsets
        {
            int cur_end{(int) by_longest.size() - 1};
            int cur_start{cur_end};
            int cur_length{0};
            for (int i = (int) by_longest.size() - 1; i-- >= 0;)
            {
                if (matchmaker::at(by_longest[i]).size() == matchmaker::at(by_longest[cur_end]).size())
                {
                    cur_start = i;
                }
                else
                {
                    cur_length = matchmaker::at(by_longest[cur_start]).size();
                    offsets[cur_length] = std::make_pair(cur_start, cur_end);
                    cur_start = cur_end = i;
                }
            }
        }

        // write offsets
        {
            std::string index;

            if (fputs("inline std::map<int, std::pair<int, int>> const LONGEST_WORDS_OFFSETS{\n", f) == EOF)
                goto err;

            for (auto const & [l, o] : offsets)
            {
                if (fputs("    { ", f) == EOF)
                    goto err;

                index = std::to_string(l);
                if (fputs(index.c_str(), f) == EOF)
                    goto err;

                if (fputs(", { ", f) == EOF)
                    goto err;

                index = std::to_string(o.first);
                if (fputs(index.c_str(), f) == EOF)
                    goto err;

                if (fputs(", ", f) == EOF)
                    goto err;

                index = std::to_string(o.second);
                if (fputs(index.c_str(), f) == EOF)
                    goto err;

                if (fputs("} },\n", f) == EOF)
                    goto err;
            }

            if (fputs("};\n\n", f) == EOF)
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


    {
        std::priority_queue<
            HeaderEntry,
            std::vector<HeaderEntry>,
            std::less<std::vector<HeaderEntry>::value_type>
        > header_entries;

        for (auto const & entry : std::filesystem::recursive_directory_iterator(STAGE_1_MATCHABLES_DIR))
            if (entry.is_regular_file())
                header_entries.push({entry});

        header_count = (int) header_entries.size();

        int const CPU_COUNT = std::thread::hardware_concurrency();
        std::vector<std::vector<HeaderEntry>> dealt_header_entries;
        dealt_header_entries.reserve(CPU_COUNT);
        for (int i = 0; i < CPU_COUNT; ++i)
            dealt_header_entries.push_back(std::vector<HeaderEntry>());

        int i = 0;
        while (!header_entries.empty())
        {
            dealt_header_entries[i].push_back(header_entries.top());
            header_entries.pop();
            ++i;
            if (i >= CPU_COUNT)
                i = 0;
        }

        std::vector<std::thread> threads;
        bool all_ok{true};
        for (auto const & deal : dealt_header_entries)
            threads.emplace_back(
                std::thread(
                    [&](){
                        if (all_ok && !patch_matchable_header(deal, contents_51155, by_longest, offsets))
                            all_ok = false;
                    }
                )
            );
        for (auto & t : threads)
            t.join();

        if (!all_ok)
            return 1;
    }

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
    std::vector<HeaderEntry> const & matchable_headers,
    std::map<std::string, entry_51155> const & contents_51155,
    std::vector<int> const & by_longest,
    std::map<int, std::pair<int, int>> const & longest_offsets
)
{
    for (auto const & matchable_header : matchable_headers)
    {
        matchable::MatchableMaker mm;
        matchable::load__status::Type load_status = mm.load(matchable_header.entry.path());
        if (load_status != matchable::load__status::success::grab())
        {
            std::cout << "attempt to load " << matchable_header.entry << " failed with: "
                    << load_status << std::endl;
            return false;
        }

        std::string existing_word;
        std::string prefix;
        bool found{false};
        int index{-1};

        for (auto const & [str, m] : mm.matchables)
        {
            if (str.rfind("word", 0) != std::string::npos)
            {
                prefix = str.substr(5);
                prefix = std::regex_replace(
                    prefix,
                    std::regex("_"),
                    " "
                );

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
                        std::pair<int, int> const & window = longest_offsets.at(existing_word.size());

                        int lower = window.first;
                        int upper = window.second + 1;
                        int mid{0};

                        while (lower < upper)
                        {
                            mid = lower + (upper - lower) / 2;

                            if (existing_word == matchmaker::at(by_longest[mid]))
                            {
                                m->set_property(v.variant_name, "by_longest_index", std::to_string(mid));
                                break;
                            }

                            if (existing_word < matchmaker::at(by_longest[mid]))
                                upper = mid;
                            else
                                lower = mid;
                        }
                    }
                }

                found = true;
                break;
            }
        }

        std::lock_guard<std::mutex> const lock(patch_matchable_header_mutex);

        std::cout << "       ------> patching " << prefix;
        for (int i = prefix.size(); i < 11; ++i)
            std::cout << ".";
        std::cout << "........: " << std::flush;
        if (!found)
        {
            std::cout << "failed to find \"word\" matchable" << std::endl;
            return false;
        }

        auto sa_status = mm.save_as(
            matchable_header.entry.path(),
            {matchable::save_as__content::matchables::grab()},
            matchable::save_as__spread_mode::wrap::grab()
        );

        ++processed_headers;
        std::cout << sa_status << " (" << processed_headers << " / " << header_count << ")" << std::endl;

        if (sa_status != matchable::save_as__status::success::grab())
            return false;

    }

    return true;
}
