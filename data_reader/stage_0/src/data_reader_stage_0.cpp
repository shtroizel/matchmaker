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


#include <cstring>
#include <filesystem>
#include <iostream>
#include <queue>

#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchable/MatchableMaker.h>

#include <matchmaker/parts_of_speech.h>
#include <matchmaker/data_reader_common.h>


int const MAX_WORD_LENGTH{4953};


MATCHABLE(
    word_attribute,
    invisible_ascii,
    matchable_symbols,
    unmatchable_symbols,
    name,
    male_name,
    female_name,
    place,
    compound,
    acronym,
    phrase
)


MATCHABLE(Encountered, Yes)


void print_usage();

bool has_responsibility(char letter, char prefix_element);

bool passes_prefix_filter(
    std::string const & word,
    std::string const & l0, // first letter
    std::string const & l1, // second letter
    std::string const & l2, // third letter
    std::string const & l3, // fourth letter
    std::string const & l4, // fifth letter
    std::string const & l5  // sixth letter
);

bool passes_status_filter(word_attribute::Flags const & status);

bool passes_filter(
    std::string const & word,
    word_attribute::Flags const & status,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5
);

void read_3201_default(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & base_attributes,
    matchable::MatchableMaker & mm
);

void read_3202(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    matchable::MatchableMaker & mm
);

void read_3203_mobypos(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    matchable::MatchableMaker & mm
);

bool read_crumbs(
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * book_vocab_file,
    matchable::MatchableMaker & mm,
    int & text
);

void update_word_attribute(word_attribute::Flags & flags, int & ch);

bool read_crumbs_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & attributes
);

void find_quotes(std::string const & line, std::vector<std::string> & quotes);

void cherry_pick_parens(
    char left_paren,
    char right_paren,
    std::string & line,
    std::vector<std::string> & cherries
);

bool strip_word(char ch, std::string & word);
void trim_word(char ch, std::string & word);

bool read_3201_default_line(FILE * f, std::string & word, word_attribute::Flags & status);

bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & status,
    parts_of_speech::Flags & pos
);

bool add_book_vocab_if_passes_filter(
    std::string const & word,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters,
    matchable::MatchableMaker & mm
);

bool add_book_vocab(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters,
    matchable::MatchableMaker & mm
);

void add_word(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    matchable::MatchableMaker & mm
);
void add_word(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    parts_of_speech::Flags const & pos_flags,
    matchable::MatchableMaker & mm
);

void tokenize(std::string const & str, char const delim, std::vector<std::string> & out);


bool symbols_off = false; // disables words with symbols when this is true



int main(int argc, char ** argv)
{
    if (argc < 10)
    {
        print_usage();
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const BOOK_VOCAB_DIR{argv[2]};
    std::string const OUTPUT_DIR{argv[3]};

    std::string l0{argv[4]};
    std::string l1{argv[5]};
    std::string l2{argv[6]};
    std::string l3{argv[7]};
    std::string l4{argv[8]};
    std::string l5{argv[9]};

    bool l0_is_symbol{false};

//     std::cout << "running stage 0 data reader for " << l0 << " " << l1 << " " << l2 << " "
//               << l3 << " " << l4 << " " << l5 << std::endl;

    if (l0.size() != 1)
    {
        print_usage();
        return 2;
    }
    else if (l0[0] < 'A' || (l0[0] > 'Z' && l0[0] < 'a') || l0[0] > 'z')
    {
        if (l1 == "nil")  // allow symbols for l0 only
        {
            l0_is_symbol = true;
        }
        else
        {
            print_usage();
            return 2;
        }
    }

    if (l1.size() != 1 && l1 != "nil")
    {
        print_usage();
        return 2;
    }
    else if (l1[0] < 'A' || (l1[0] > 'Z' && l1[0] < 'a') || l1[0] > 'z')
    {
        print_usage();
        return 2;
    }

    if (l2.size() != 1 && l2 != "nil")
    {
        print_usage();
        return 2;
    }
    else if (l2[0] < 'A' || (l2[0] > 'Z' && l2[0] < 'a') || l2[0] > 'z')
    {
        print_usage();
        return 2;
    }

    if (l3.size() != 1 && l3 != "nil")
    {
        print_usage();
        return 2;
    }
    else if (l3[0] < 'A' || (l3[0] > 'Z' && l3[0] < 'a') || l3[0] > 'z')
    {
        print_usage();
        return 2;
    }

    if (l4.size() != 1 && l4 != "nil")
    {
        print_usage();
        return 2;
    }
    else if (l4[0] < 'A' || (l4[0] > 'Z' && l4[0] < 'a') || l4[0] > 'z')
    {
        print_usage();
        return 2;
    }

    if (l5.size() != 1 && l5 != "nil")
    {
        print_usage();
        return 2;
    }
    else if (l5[0] < 'A' || (l5[0] > 'Z' && l5[0] < 'a') || l5[0] > 'z')
    {
        print_usage();
        return 2;
    }

    q_usage::Type q_mode;
    if (argc == 11)
        q_mode = q_usage::from_string(argv[10]);
    if (q_mode.is_nil())
        q_mode = q_usage::included::grab();

//     std::cout << "q_mode: " << q_mode << std::endl;

    std::string prefix{"_"};
    if (l0_is_symbol)
    {
        if (l0 == "0")
            prefix += "zero";
        else if (l0 == "1")
            prefix += "one";
        else if (l0 == "2")
            prefix += "two";
        else if (l0 == "3")
            prefix += "three";
        else if (l0 == "4")
            prefix += "four";
        else if (l0 == "5")
            prefix += "five";
        else if (l0 == "6")
            prefix += "six";
        else if (l0 == "7")
            prefix += "seven";
        else if (l0 == "8")
            prefix += "eight";
        else if (l0 == "9")
            prefix += "nine";
        else if (l0 == "-")
            prefix += "mns";
        else if (l0 == ".")
            prefix += "dot";
        else if (l0 == "/")
            prefix += "slsh";
        else if (l0 == ":")
            prefix += "cln";
        else if (l0 == "\"")
            prefix += "quot";
        else if (l0 == "#")
            prefix += "hsh";
        else if (l0 == "$")
            prefix += "dol";
        else if (l0 == "'")
            prefix += "sqt";
        else if (l0 == "(")
            prefix += "pl";
        else if (l0 == "+")
            prefix += "pls";
        else if (l0 == ">")
            prefix += "gt";
        else if (l0 == "_")
            prefix += "uscr";
        else if (l0 == "~")
            prefix += "tld";
        else
        {
            std::cout << "ERROR: unknown symbol for l0 --> '" << l0 << "'" << std::endl;
            return 2;
        }
    }
    else
    {
        prefix += l0;
        if (l1 != "nil")
        {
            prefix += "_" + l1;
            if (l2 != "nil")
            {
                prefix += "_" + l2;
                if (l3 != "nil")
                {
                    prefix += "_" + l3;
                    if (l4 != "nil")
                    {
                        prefix += "_" + l4;
                        if (l5 != "nil")
                        {
                            prefix += "_" + l5;
                        }
                    }
                }
            }
        }
    }

    matchable::MatchableMaker mm;

    mm.grab("word" + prefix)->add_property("int8_t", "pos");
    mm.grab("word" + prefix)->add_property("int", "syn");
    mm.grab("word" + prefix)->add_property("int", "ant");
    mm.grab("word" + prefix)->add_property("int", "by_longest_index");
    mm.grab("word" + prefix)->add_property("int", "ordinal_summation");
    mm.grab("word" + prefix)->add_property("int", "embedded");
    mm.grab("word" + prefix)->add_property("int", "book_index");
    mm.grab("word" + prefix)->add_property("int", "chapter_index");
    mm.grab("word" + prefix)->add_property("int", "paragraph_index");
    mm.grab("word" + prefix)->add_property("int", "word_index");

    // "word_attribute" properties
    {
        auto add_bool_prop =
            [&](std::string const & prop)
            {
                std::string const prop_name = "is_" + prop;
                mm.grab("word" + prefix)->add_property("int8_t", prop_name);
            };

        add_bool_prop(word_attribute::name::grab().as_string());
        add_bool_prop(word_attribute::male_name::grab().as_string());
        add_bool_prop(word_attribute::female_name::grab().as_string());
        add_bool_prop(word_attribute::place::grab().as_string());
        add_bool_prop(word_attribute::compound::grab().as_string());
        add_bool_prop(word_attribute::acronym::grab().as_string());
        add_bool_prop(word_attribute::phrase::grab().as_string());
        add_bool_prop("used_in_book");
    }

    if (q_mode != q_usage::only::grab())
    {
        {
            std::string const FN_3201_SINGLE{DATA_DIR + "/3201/files/SINGLE.TXT"};
            FILE * single_file = fopen(FN_3201_SINGLE.c_str(), "r");
            if (single_file == 0)
            {
                perror(FN_3201_SINGLE.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes;
            read_3201_default(single_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(single_file);
        }

        {
            std::string const FN_3201_COMPOUND{DATA_DIR + "/3201/files/COMPOUND.TXT"};
            FILE * compound_file = fopen(FN_3201_COMPOUND.c_str(), "r");
            if (compound_file == 0)
            {
                perror(FN_3201_COMPOUND.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{word_attribute::compound::grab()};
            read_3201_default(compound_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(compound_file);
        }

        {
            std::string const FN_3201_COMMON{DATA_DIR + "/3201/files/COMMON.TXT"};
            FILE * common_file = fopen(FN_3201_COMMON.c_str(), "r");
            if (common_file == 0)
            {
                perror(FN_3201_COMMON.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes;
            read_3201_default(common_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(common_file);
        }

        {
            std::string const FN_3201_NAMES{DATA_DIR + "/3201/files/NAMES.TXT"};
            FILE * names_file = fopen(FN_3201_NAMES.c_str(), "r");
            if (names_file == 0)
            {
                perror(FN_3201_NAMES.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{word_attribute::name::grab()};
            read_3201_default(names_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(names_file);
        }

        {
            std::string const FN_3201_NAMES_F{DATA_DIR + "/3201/files/NAMES-F.TXT"};
            FILE * names_f_file = fopen(FN_3201_NAMES_F.c_str(), "r");
            if (names_f_file == 0)
            {
                perror(FN_3201_NAMES_F.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{
                word_attribute::name::grab(),
                word_attribute::female_name::grab()
            };
            read_3201_default(names_f_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(names_f_file);
        }

        {
            std::string const FN_3201_NAMES_M{DATA_DIR + "/3201/files/NAMES-M.TXT"};
            FILE * names_m_file = fopen(FN_3201_NAMES_M.c_str(), "r");
            if (names_m_file == 0)
            {
                perror(FN_3201_NAMES_M.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{
                word_attribute::name::grab(),
                word_attribute::male_name::grab()
            };
            read_3201_default(names_m_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(names_m_file);
        }

        {
            std::string const FN_3201_PLACES{DATA_DIR + "/3201/files/PLACES.TXT"};
            FILE * places_file = fopen(FN_3201_PLACES.c_str(), "r");
            if (places_file == 0)
            {
                perror(FN_3201_PLACES.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{word_attribute::place::grab()};
            read_3201_default(places_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(places_file);
        }

        {
            std::string const FN_3201_CROSSWD{DATA_DIR + "/3201/files/CROSSWD.TXT"};
            FILE * crosswd_file = fopen(FN_3201_CROSSWD.c_str(), "r");
            if (crosswd_file == 0)
            {
                perror(FN_3201_CROSSWD.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes;
            read_3201_default(crosswd_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(crosswd_file);
        }

        {
            std::string const FN_3201_CRSWD_D{DATA_DIR + "/3201/files/CRSWD-D.TXT"};
            FILE * crswd_d_file = fopen(FN_3201_CRSWD_D.c_str(), "r");
            if (crswd_d_file == 0)
            {
                perror(FN_3201_CRSWD_D.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes;
            read_3201_default(crswd_d_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(crswd_d_file);
        }

        {
            std::string const FN_3201_ACRONYMS{DATA_DIR + "/3201/files/ACRONYMS.TXT"};
            FILE * acronyms_file = fopen(FN_3201_ACRONYMS.c_str(), "r");
            if (acronyms_file == 0)
            {
                perror(FN_3201_ACRONYMS.c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes{word_attribute::acronym::grab()};
            read_3201_default(acronyms_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm);
            fclose(acronyms_file);
        }

        {
            std::string const FN_3202{DATA_DIR + "/3202/files/mthesaur.txt"};
            FILE * input_file = fopen(FN_3202.c_str(), "r");
            if (input_file == 0)
            {
                perror(FN_3202.c_str());
                exit(1);
            }
            read_3202(input_file, l0, l1, l2, l3, l4, l5, prefix, mm);
            fclose(input_file);
        }

        {
            std::string const FN_3203_MOBYPOS{DATA_DIR + "/3203/files/mobypos.txt"};
            FILE * mobypos_file = fopen(FN_3203_MOBYPOS.c_str(), "r");
            if (mobypos_file == 0)
            {
                perror(FN_3203_MOBYPOS.c_str());
                exit(1);
            }
            read_3203_mobypos(mobypos_file, l0, l1, l2, l3, l4, l5, prefix, mm);
            fclose(mobypos_file);
        }
    }

    if (q_mode != q_usage::omitted::grab())
    {
        int text = 0;
        FILE * q_file = nullptr;

        std::string const crumbs_path = BOOK_VOCAB_DIR + "/Crumbs";
        std::filesystem::create_directory(crumbs_path);
        std::string const crumbs_vocab = BOOK_VOCAB_DIR + "/Crumbs/" + prefix.substr(1);
        FILE * vocab_file = fopen(crumbs_vocab.c_str(), "w");
        if (vocab_file == 0)
        {
            perror(crumbs_vocab.c_str());
            exit(1);
        }

        // the recursive_directory_iterator provides the files in random order, so first sort
        // them by filename using the priority q
        std::priority_queue<
            QFile,
            std::vector<QFile>,
            std::less<std::vector<QFile>::value_type>
        > q_files;
        for (auto const & entry : std::filesystem::recursive_directory_iterator(DATA_DIR + "/Crumbs/text"))
            if (entry.is_regular_file())
                q_files.push({entry});

        int files_processed = 0;
        while (!q_files.empty())
        {
            ++files_processed;

            q_file = fopen(q_files.top().entry.path().c_str(), "r");
            if (q_file == 0)
            {
                perror(q_files.top().entry.path().c_str());
                exit(1);
            }
            word_attribute::Flags base_attributes;
            bool ok = read_crumbs(l0, l1, l2, l3, l4, l5, prefix, base_attributes,
                                  q_file, vocab_file, mm, text);
            fclose(q_file);
            if (!ok)
            {
                std::cout << "failed to read file: " << q_files.top().entry.path() << std::endl;
                fclose(vocab_file);
                return 1;
            }
            q_file = nullptr;
            q_files.pop();

            if (files_processed >= 28)
                break;
        }

        fclose(vocab_file);
        vocab_file = nullptr;
    }

//     if (q_mode != q_usage::omitted::grab())
//     {
//         int text = 143;
//         FILE * q_file = nullptr;
//
//         (void) text;
//         (void) q_file;
//
//         std::string const test_filename = "/home/shtroizel/q/testing";
//         q_file = fopen(test_filename.c_str(), "r");
//         if (q_file == 0)
//         {
//             perror(test_filename.c_str());
//             exit(1);
//         }
//         word_attribute::Flags base_attributes;
//         bool ok = read_crumbs(q_file, l0, l1, l2, l3, l4, l5, prefix, base_attributes, mm, text);
//         fclose(q_file);
//         if (!ok)
//         {
//             std::cout << "failed to read file: " << test_filename << std::endl;
//             return 1;
//         }
//         q_file = nullptr;
//     }


    {
        std::vector<std::string> shtroizel{ "shtroizel", "Shtroizel" };
        word_attribute::Flags word_attributes{word_attribute::name::grab()};
        parts_of_speech::Flags pos{parts_of_speech::N::grab()};

        for (auto const & s : shtroizel)
            if (passes_filter(s, word_attributes, l0, l1, l2, l3, l4, l5))
                add_word(s, prefix, word_attributes, pos, mm);
    }

    // remove leading underscore
    prefix.erase(0, 1);


    std::cout << "generating stage 0 matchables: " << l0 << " ";
    if (l1 == "nil")
        std::cout << "--";
    else
        std::cout << l1 << " ";
    if (l2 == "nil")
        std::cout << "--";
    else
        std::cout << l2 << " ";
    if (l3 == "nil")
        std::cout << "--";
    else
        std::cout << l3 << " ";
    if (l4 == "nil")
        std::cout << "--";
    else
        std::cout << l4 << " ";
    if (l5 == "nil")
        std::cout << "--";
    else
        std::cout << l5 << " ";

    // save file
    std::string out_filename{OUTPUT_DIR + "/" + prefix + ".h"};
    auto sa_status = mm.save_as(
        out_filename,
        {matchable::save_as__content::matchables::grab()},
        matchable::save_as__spread_mode::wrap::grab()
    );

    std::cout << "---------> " << sa_status << std::endl;

    if (sa_status != matchable::save_as__status::success::grab())
        return 1;

    return 0;
}



void print_usage()
{
    std::cout << "program expects 8 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  output directory\n"
              << "\n"
              << "    * letter arguments form an inclusive prefix filter\n"
              << "    * letters are case sensitive\n"
              << "\n"
              << "    [3]  first letter\n"
              << "         - include words starting with <first letter>\n"
              << "         - single letter word of 'first letter' is included when second letter is 'a'\n"
              << "           and 'third letter' is either 'a' or 'nil'\n"
              << "    [4]  second letter\n"
              << "         - include words seconding with <second letter>\n"
              << "         - two letter word of 'first letter' + 'second letter' is included when \n"
              << "           'third letter' is either 'a' or 'nil'\n"
              << "         - can be disabled for single letter prefix by setting to 'nil'\n"
              << "    [5]  third letter\n"
              << "         - include words thirding with <third letter>\n"
              << "         - can be disabled for two letter prefix by setting to 'nil'\n"
              << "         - ignored when second letter is 'nil'\n"
              << "    [6]  fourth letter\n"
              << "         - include words fourthing with <fourth letter>\n"
              << "         - can be disabled for three letter prefix by setting to 'nil'\n"
              << "         - ignored when <second letter> or <third letter> is 'nil'\n"
              << "    [7]  fifth letter\n"
              << "         - include words fifthing with <fifth letter>\n"
              << "         - can be disabled for four letter prefix by setting to 'nil'\n"
              << "         - ignored when <second letter> or <third letter> or <fourth letter> is 'nil'\n"
              << "    [8]  sixth letter\n"
              << "         - include words sixthing with <sixth letter>\n"
              << "         - can be disabled for five letter prefix by setting to 'nil'\n"
              << "         - ignored when <second letter> or <third letter> or <fourth letter>\n"
              << "           or <fifth letter> is 'nil'\n"
              << std::flush;
}


bool has_responsibility(std::string const & word, std::string prefix, size_t depth)
{
    if (word.length() == 0)
    {
        std::cout << "has_responsibility() --> empty word!" << std::endl;
        return false;
    }

    if (prefix.length() == 0)
    {
        std::cout << "has_responsibility() --> empty prefix!" << std::endl;
        return false;
    }

    if (depth >= word.length() || depth >= prefix.length())
    {
        std::cout << "has_responsibility() --> depth out of bounds!" << std::endl;
        return false;
    }

    // allow some symbols for depth 0 only
    // the rest of the symbols are to be handled by left most leaf
    // these symbols at other depths are also handled by the left most leaf
    if (depth == 0 &&
            ((word[0] >= '-' && word[0] <= ':') || // number or '-', '.', '/' or ':'
             (word[0] >= '"' && word[0] <= '$') || // '"', '#' or '$'
             word[0] == '\'' ||
             word[0] == '(' ||
             word[0] == '+' ||
             word[0] == '>' ||
             word[0] == '_' ||
             word[0] == '~'))
        return word[0] == prefix[0];

    // find first occurance of symbol within prefix depth
    size_t i = 0;
    for (; i < word.length() && i < prefix.length(); ++i)
        if (word[i] < 'A' || (word[i] > 'Z' && word[i] < 'a') || word[i] > 'z')
            break;

    // if no symbols then do normal check
    if (i == word.length() || i == prefix.length())
        return word[depth] == prefix[depth];

    bool ok = prefix[i] == 'A';
    for (; ok && i < prefix.length(); ++i)
        ok = prefix[i] == 'A';

    return ok;
}



bool passes_prefix_filter(
    std::string const & word,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5
)
{
    if (word.size() == 0)
        return false;

    // if word does not start with l0 then fail
    if (!has_responsibility(word, l0, 0))
        return false;

    if (l1 != "nil")
    {
        if (word.size() > 1)
        {
            // if word does not second with l1 then fail
            if (!has_responsibility(word, l0 + l1, 1))
                return false;

            if (l2 != "nil")
            {
                if (word.size() > 2)
                {
                    // if 3+ letter word does not third with l2 then fail
                    if (!has_responsibility(word, l0 + l1 + l2, 2))
                        return false;

                    if (l3 != "nil")
                    {
                        if (word.size() > 3)
                        {
                            // if 4+ letter word does not fourth with l3 then fail
                            if (!has_responsibility(word, l0 + l1 + l2 + l3, 3))
                                return false;

                            if (l4 != "nil")
                            {
                                if (word.size() > 4)
                                {
                                    // if 5+ letter word does not fifth with l4 then fail
                                    if (!has_responsibility(word, l0 + l1 + l2 + l3 + l4, 4))
                                        return false;

                                    if (l5 != "nil")
                                    {
                                        if (word.size() > 5)
                                        {
                                            // if 6+ letter word does not sixth with l5 then fail
                                            if (!has_responsibility(word, l0 + l1 + l2 + l3 + l4 + l5, 5))
                                                return false;
                                        }
                                        else
                                        {
                                            // fail five letter word unless left leaf
                                            bool left_leaf = (l5[0] == 'A');

                                            if (!left_leaf)
                                                return false;
                                        }
                                    }
                                }
                                else
                                {
                                    // fail four letter word unless left leaf
                                    bool left_leaf =
                                        (l4[0] == 'A' && l5[0] == 'A') ||
                                        (l4[0] == 'A' && l5 == "nil");

                                    if (!left_leaf)
                                        return false;
                                }
                            }
                        }
                        else
                        {
                            // fail three letter word unless left leaf
                            bool left_leaf =
                                (l3[0] == 'A' && l4[0] == 'A' && l5[0] == 'A') ||
                                (l3[0] == 'A' && l4[0] == 'A' && l5 == "nil") ||
                                (l3[0] == 'A' && l4 == "nil");

                            if (!left_leaf)
                                return false;
                        }
                    }
                }
                else
                {
                    // fail two letter word unless left leaf
                    bool left_leaf =
                        (l2[0] == 'A' && l3[0] == 'A' && l4[0] == 'A' && l5[0] == 'A') ||
                        (l2[0] == 'A' && l3[0] == 'A' && l4[0] == 'A' && l5 == "nil") ||
                        (l2[0] == 'A' && l3[0] == 'A' && l4 == "nil") ||
                        (l2[0] == 'A' && l3 == "nil");

                    if (!left_leaf)
                        return false;
                }
            }
        }
        else
        {
            // fail one letter word unless left leaf
            bool left_leaf =
                (l1[0] == 'A' && l2[0] == 'A' && l3[0] == 'A' && l4[0] == 'A' && l5[0] == 'A') ||
                (l1[0] == 'A' && l2[0] == 'A' && l3[0] == 'A' && l4[0] == 'A' && l5 == "nil") ||
                (l1[0] == 'A' && l2[0] == 'A' && l3[0] == 'A' && l4 == "nil") ||
                (l1[0] == 'A' && l2[0] == 'A' && l3 == "nil") ||
                (l1[0] == 'A' && l2 == "nil");

            if (!left_leaf)
                return false;
        }
    }

    return true;
}


bool passes_status_filter(word_attribute::Flags const & status)
{
    if (status.is_set(word_attribute::invisible_ascii::grab()))
        return false;

    if (status.is_set(word_attribute::unmatchable_symbols::grab()))
        return false;

    if (symbols_off && status.is_set(word_attribute::matchable_symbols::grab()))
        return false;

    return true;
}


bool passes_filter(
    std::string const & word,
    word_attribute::Flags const & status,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5
)
{
    if (word.size() < 1)
        return false;

    if (word.size() > MAX_WORD_LENGTH)
        return false;

    if (!passes_prefix_filter(word, l0, l1, l2, l3, l4, l5))
        return false;

    if (!passes_status_filter(status))
        return false;

    return true;
}


void read_3201_default(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & base_attributes,
    matchable::MatchableMaker & mm
)
{
    std::string word;
    word_attribute::Flags attributes;

    while (true)
    {
        attributes = base_attributes;

        if (!read_3201_default_line(input_file, word, attributes))
            break;

        if (word.size() == 0)
            continue;

        if (!passes_filter(word, attributes, l0, l1, l2, l3, l4, l5))
            continue;

        if (attributes.is_set(word_attribute::compound::grab()) &&
                word.find('-') == std::string::npos && word.find(' ') == std::string::npos)
            attributes.unset(word_attribute::compound::grab());

        add_word(word, prefix, attributes, mm);
    }
}



void read_3202(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    matchable::MatchableMaker & mm
)
{
    std::string word;
    parts_of_speech::Flags pos_flags;
    word_attribute::Flags attributes;

    int ch = 0;
    while (true)
    {
        word.clear();
        while (true)
        {
            ch = fgetc(input_file);
            if (ch == EOF || ch == 10 || ch == 13 || ch == ',')
                break;

            word += (char) ch;
        }

        if (passes_filter(word, attributes, l0, l1, l2, l3, l4, l5) && word.size() > 0)
            add_word(word, prefix, attributes, pos_flags, mm);

        if (ch == EOF)
            break;
    }
}



void read_3203_mobypos(
    FILE * input_file,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    matchable::MatchableMaker & mm
)
{
    std::string word;
    parts_of_speech::Flags pos_flags;
    word_attribute::Flags attributes;

    while (true)
    {
        if (!read_3203_mobypos_line(input_file, word, attributes, pos_flags))
            break;

        if (word.size() == 0)
            continue;

        if (!passes_filter(word, attributes, l0, l1, l2, l3, l4, l5))
            continue;

        add_word(word, prefix, attributes, pos_flags, mm);
    }
}



bool read_crumbs(
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
    matchable::MatchableMaker & mm,
    int & text
)
{
    static std::map<std::string, Encountered::Type> encounters;
    word_attribute::Flags attributes;
    std::string line;

    while (true)
    {
        attributes = base_attributes;

        if (!read_crumbs_line(input_file, line, attributes))
            break;

        if (attributes.is_set(word_attribute::unmatchable_symbols::grab()))
        {
            std::cout << "encountered unmatchable symbols! text: " << text << " line: " << line << std::endl;
            return false;
        }
        if (attributes.is_set(word_attribute::invisible_ascii::grab()))
        {
            std::cout << "encountered invisible_ascii! text: " << text << " line: " << line << std::endl;
            return false;
        }
        attributes = base_attributes;

        if (line.rfind("~~:", 0) == 0)
        {
            ++text;
            std::string text_line = line.substr(3);
            if (std::to_string(text) != text_line)
            {
                std::cout << "encountered q text " << text_line << " but "
                            << text << " expected! aborting...\n";
                return false;
            }

//             if (passes_filter(text_line, attributes, l0, l1, l2, l3, l4, l5))
//                 add_word(text_line, prefix, attributes, mm);
            if (!add_book_vocab_if_passes_filter(text_line, l0, l1, l2, l3, l4, l5, prefix, attributes,
                                                 vocab_file, encounters, mm))
                return false;

            // read date/time line
            attributes = base_attributes;
            if (!read_crumbs_line(input_file, line, attributes))
                break;

            if (attributes.is_set(word_attribute::unmatchable_symbols::grab()))
            {
                std::cout << "encountered unmatchable symbols! text: " << text << " line: " << line << std::endl;
                return false;
            }
            if (attributes.is_set(word_attribute::invisible_ascii::grab()))
            {
                std::cout << "encountered invisible_ascii! text: " << text << " line: " << line << std::endl;
                return false;
            }

            // first split date/time from message board
            std::vector<std::string> tokenized_on_comma;
            tokenize(line, ',', tokenized_on_comma);
            if (tokenized_on_comma.size() != 2)
            {
                std::cout << "failed to parse header line! text: " << text << std::endl;
                return false;
            }

            // then split date, time, zoneinfo (time is middle 2 tokens because of AM/PM)
            std::vector<std::string> tokenized_date_time;
            tokenize(tokenized_on_comma[0], ' ', tokenized_date_time);
            if (tokenized_date_time.size() != 4)
            {
                std::cout << "failed to parse date/time stamp! text: " << text << std::endl;
                return false;
            }

            // then split date into day month year
            std::vector<std::string> tokenized_date;
            tokenize(tokenized_date_time[0], '-', tokenized_date);
            if (tokenized_date.size() != 3)
            {
                std::cout << "failed to parse date stamp! text: " << text << std::endl;
                return false;
            }

            // then split time into hour min sec
            std::vector<std::string> tokenized_time;
            tokenize(tokenized_date_time[1], ':', tokenized_time);
            if (tokenized_time.size() != 3)
            {
                std::cout << "failed to parse time stamp! text: " << text << std::endl;
                return false;
            }

            // guarantee 2 digit hour
            if (tokenized_time[0].length() == 1)
                tokenized_time[0].insert(0, 1, '0');

            // now add them all including all tokens & symbols
            std::vector<std::string> words{
                " ", "-", ":", ",", "=", "/", "%", "$", "\"", "|", "^", "!", "?", ".", "(", ")",
                "[", "]", "#", ";", tokenized_date_time[2], tokenized_date_time[3], tokenized_on_comma[1]
            };
            words.insert(words.end(), tokenized_date.begin(), tokenized_date.end());
            words.insert(words.end(), tokenized_time.begin(), tokenized_time.end());
            for (auto const & word : words)
//                 if (passes_filter(word, attributes, l0, l1, l2, l3, l4, l5))
//                     add_word(word, prefix, attributes, mm);
                if (!add_book_vocab_if_passes_filter(word, l0, l1, l2, l3, l4, l5, prefix, attributes,
                                                     vocab_file, encounters, mm))
                    return false;
        }
        else
        {
            // ********** TODO REMOVE ME ********************
            if (line.rfind("~~https", 0) == 0)
            {
                std::cout << "encountered error parsing '~~https' text: " << text << std::endl;
                return false;
            }
            else if (line.rfind("~;~", 0) == 0)
            {
                std::cout << "encountered error parsing '~;~' in text: " << text << std::endl;
                return false;
            } else
            //************************************************

            // each line is also just a "word"
            {
                word_attribute::Flags att = attributes;
                att.set(word_attribute::phrase::grab());
//                 if (passes_filter(line, att, l0, l1, l2, l3, l4, l5))
//                 {
//                     add_word(line, prefix, att, mm);
// //                     std::cout << "adding line: " << line << std::endl;
//                 }
                if (!add_book_vocab_if_passes_filter(line, l0, l1, l2, l3, l4, l5, prefix, att,
                                                     vocab_file, encounters, mm))
                    return false;
            }

            // only parse out lines for non-images and non-archives
            if (line.rfind("~~~", 0) != 0 && line.rfind("~:~", 0) != 0)
            {
                // add each word within the line stripped of punctuation
                {
                    std::vector<std::string> tokenized_on_space;
                    tokenize(line, ' ', tokenized_on_space);
                    for (auto & token : tokenized_on_space)
                    {
                        // check for use of 'single quotes' (apostrophes (') misused as quotes (") are banned)
                        if (token[0] == '\'' ||
                                (token.length() >= 2 &&
                                 token[token.length() - 1] == '\'' &&
                                 token[token.length() - 2] != 's' &&
                                 token[token.length() - 2] != 'S' &&
                                 token[token.length() - 2] != 'n' &&
                                 token[token.length() - 2] != 'N'))
                        {
                            std::cout << "encountered apostrophe misused as \" within q text "
                                      << text << " on line: " << line << std::endl;
                            return false;
                        }

                        if (token.rfind("http", 0) != 0)
                        {
                            strip_word('"', token);
                            strip_word('!', token);
                            strip_word('?', token);
                            strip_word(',', token);
                            strip_word(';', token);
                            strip_word('(', token);
                            strip_word(')', token);
                            strip_word('=', token);
                            trim_word('.', token);
                            trim_word(':', token);

                            // strip '#', '+' and '/' when they are used as quotes
                            if (token.length() > 2)
                            {
                                if ((token[0] == '#' && token[token.length() - 1] == '#') ||
                                        (token[0] == '+' && token[token.length() - 1] == '+') ||
                                        (token[0] == '/' && token[token.length() - 1] == '/'))
                                    token = token.substr(1, token.length() - 2);
                            }

                            // remove '-->' from the front of words
                            if (token.length() > 3 && token.rfind("-->", 0) == 0)
                                token = token.substr(3);

                            // trim leading '/' chars
                            while (token.length() > 1 && token[0] == '/')
                                token = token.substr(1);

                            // split on '/'
                            std::vector<std::string> tokenized_on_slash;
                            tokenize(token, '/', tokenized_on_slash);

                            // use original token when just one entry (preserve words like w/)
                            if (tokenized_on_slash.size() == 1)
                            {
                                tokenized_on_slash.clear();
                                tokenized_on_slash.push_back(token);
                            }

                            for (auto & slash_token : tokenized_on_slash)
                            {
                                // strip words with only left or only right brackets (must have both)
                                bool has_left_bracket = false;
                                bool has_right_bracket = false;
                                for (int i = 0; i < (int) slash_token.length(); ++i)
                                {
                                    if (slash_token[i] == '[')
                                    {
                                        has_left_bracket = true;
                                        if (has_right_bracket)
                                            break;
                                    }
                                    else if (slash_token[i] == ']')
                                    {
                                        has_right_bracket = true;
                                        if (has_left_bracket)
                                            break;
                                    }
                                }
                                if (has_left_bracket && !has_right_bracket)
                                    strip_word('[', slash_token);
                                else if (!has_left_bracket && has_right_bracket)
                                    strip_word(']', slash_token);

//                                 if (passes_filter(slash_token, attributes, l0, l1, l2, l3, l4, l5))
//                                 {
//                                     add_word(slash_token, prefix, attributes, mm);
//
// //                                     std::cout << "adding word: " << slash_token << std::endl;
//                                 }
                                if (!add_book_vocab_if_passes_filter(slash_token, l0, l1, l2, l3, l4, l5,
                                                                     prefix, attributes, vocab_file,
                                                                     encounters, mm))
                                    return false;

                                // split on '='
                                std::vector<std::string> tokenized_on_eq;
                                tokenize(slash_token, '=', tokenized_on_eq);
                                for (auto & eq_token : tokenized_on_eq)
//                                     if (passes_filter(eq_token, attributes, l0, l1, l2, l3, l4, l5))
//                                         add_word(eq_token, prefix, attributes, mm);
                                    if (!add_book_vocab_if_passes_filter(eq_token, l0, l1, l2, l3, l4, l5,
                                                                         prefix, attributes, vocab_file,
                                                                         encounters, mm))
                                        return false;

                                // for words with brackets, add version of word without brackets
                                bool had_brackets = strip_word('[', slash_token);
                                had_brackets = strip_word(']', slash_token) || had_brackets;
                                if (had_brackets)
                                {
//                                     if (passes_filter(slash_token, attributes, l0, l1, l2, l3, l4, l5))
//                                         add_word(slash_token, prefix, attributes, mm);
                                    if (!add_book_vocab_if_passes_filter(slash_token, l0, l1, l2, l3, l4,
                                                                         l5, prefix, attributes,
                                                                         vocab_file, encounters, mm))
                                        return false;
                                }
                            }
                        }
                        else
                        {
//                             if (passes_filter(token, attributes, l0, l1, l2, l3, l4, l5))
//                                 add_word(token, prefix, attributes, mm);
                            if (!add_book_vocab_if_passes_filter(token, l0, l1, l2, l3, l4, l5, prefix,
                                                                 attributes, vocab_file, encounters, mm))
                                return false;
                        }
//                         std::cout << "adding word: " << token << std::endl;
                    }
                }

                // process groups of words (phrases)
                auto process_phrases =
                    [&](std::vector<std::string> const & phrases)
                    {
                        for (auto const & phrase : phrases)
                        {
                            // first add entire quotation (without quotes but could be multiple words)
                            if (passes_filter(phrase, attributes, l0, l1, l2, l3, l4, l5))
                            {
                                bool has_space = false;
                                for (int i = 0; !has_space && i < (int) phrase.length(); ++i)
                                    if (phrase[i] == ' ')
                                        has_space = true;

                                word_attribute::Flags att = attributes;
                                if (has_space)
                                    att.set(word_attribute::phrase::grab());
                                add_book_vocab(phrase, prefix, att, vocab_file, encounters, mm);
//                                 add_word(phrase, prefix, attributes, mm);
//                                 std::cout << "adding phrase: " << phrase << std::endl;
                            }
//                             std::cout << "adding phrase: " << phrase << std::endl;

                        }
                    };

                // phrases created by quotes
                {
                    std::vector<std::string> quotes;
                    find_quotes(line, quotes);
                    process_phrases(quotes);
                }

                // phrases created by parens
                std::function<void (std::string, char, char)> process_paren_phrases =
                    [&process_phrases, &process_paren_phrases](
                        std::string line,
                        char left_paren,
                        char right_paren
                    )
                    {
                        std::vector<std::string> phrases;
                        cherry_pick_parens(left_paren, right_paren, line, phrases);
                        process_phrases(phrases);
                        for (auto phrase : phrases)
                            process_paren_phrases(phrase, left_paren, right_paren);
                    };
                process_paren_phrases(line, '(', ')');
                process_paren_phrases(line, '[', ']');
            }
        }
    }
    return true;
}



void find_quotes(std::string const & line, std::vector<std::string> & quotes)
{
    bool within_quote = false;
    std::vector<std::pair<int, int>> quote_locations;
    for (int i = 0; i < (int) line.length(); ++i)
    {
        if (line[i] == '"')
        {
            if (!within_quote) // opening quote
                quote_locations.push_back(std::make_pair(i, i));
            else // closing quote
                quote_locations.back().second = i;
            within_quote = !within_quote;
        }
    }

    for (int i = (int) quote_locations.size(); i-- > 0;)
    {
        quotes.insert(quotes.begin(), std::string());
        for (int j = quote_locations[i].first + 1; j < quote_locations[i].second; ++j)
            quotes.front() += line[j];
    }
}



void cherry_pick_parens(
    char left_paren,
    char right_paren,
    std::string & line,
    std::vector<std::string> & cherries
)
{
    std::vector<std::pair<int, int>> cherry_locations;
    int level = 0;
    for (int i = 0; i < (int) line.length(); ++i)
    {
        if (line[i] == left_paren)
        {
            ++level;
            if (level == 1)
                cherry_locations.push_back(std::make_pair(i, i));
        }
        else if (line[i] == right_paren)
        {
            --level;
            if (level == 0)
                cherry_locations.back().second = i;
        }
    }

    for (int i = (int) cherry_locations.size(); i-- > 0;)
    {
        cherries.insert(cherries.begin(), std::string());
        for (int j = cherry_locations[i].first + 1; j < cherry_locations[i].second; ++j)
            cherries.front() += line[j];

        line.erase(cherry_locations[i].first, cherry_locations[i].second - cherry_locations[i].first + 1);
    }
}


bool strip_word(char ch, std::string & word)
{
    std::string stripped;
    stripped.reserve(word.size());
    for (size_t i = 0; i < word.length(); ++i)
        if (word[i] != ch)
            stripped += word[i];

    if (word.length() != stripped.length())
    {
        word = stripped;
        return true;
    }
    return false;
}


void trim_word(char ch, std::string & word)
{
    int count = 0;
    for (int i = 0; i < (int) word.length(); ++i)
        if (word[i] == ch)
            ++count;

    int end_count = 0;
    for (int i = (int) word.length(); i-- > 0;)
        if (word[i] == ch)
            ++end_count;
        else
            break;

    int begin_count = 0;
    for (int i = 0; i < (int) word.length(); ++i)
        if (word[i] == ch)
            ++begin_count;
        else
            break;

    if (count == end_count + begin_count)
    {
        word.erase(word.length() - end_count, end_count);
        word.erase(0, begin_count);
    }
}



void update_word_attribute(word_attribute::Flags & flags, int & ch)
{
    if (ch < 32 || ch > 126)
    {
        flags.set(word_attribute::invisible_ascii::grab());
        ch = '@';
    }
    else
    {
        // if not letter, #, or _
        if ((ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z') && !(ch > 47 && ch < 58) && ch != 95)
        {
            bool found{false};
            for (auto const & [code, symbol] : matchable::escapable::code_symbol_pairs())
            {
                if (symbol.length() > 0 && symbol[0] == ch)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                flags.set(word_attribute::matchable_symbols::grab());
            else
            {
                std::cout << "unmatchable symbol: " << std::string(1, (char) ch) << std::endl;
                flags.set(word_attribute::unmatchable_symbols::grab());
            }
        }
    }
}



bool read_crumbs_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & attributes
)
{
    word.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    return true;
}



bool read_3201_default_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & attributes
)
{
    word.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
        {
            while (true)
            {
                ch = fgetc(f);
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

        update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    return true;
}



bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & attributes,
    parts_of_speech::Flags & pos_flags
)
{
    word.clear();
    attributes.clear();
    pos_flags.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
            continue;

        if (ch == (int) '\\')
            break;

        update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
            break;

        if (ch < 32 || ch > 126)
            attributes.set(word_attribute::invisible_ascii::grab());

        if (ch == (int) '!')
            ch = (int) 'n';

        auto ch_str = std::string(1, (char) ch);
        auto pos_flag = parts_of_speech::from_string(ch_str);
        if (!pos_flag.is_nil())
            pos_flags.set(pos_flag);
    }

    return true;
}



bool add_book_vocab_if_passes_filter(
    std::string const & word,
    std::string const & l0,
    std::string const & l1,
    std::string const & l2,
    std::string const & l3,
    std::string const & l4,
    std::string const & l5,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters,
    matchable::MatchableMaker & mm
)
{
    if (!passes_filter(word, wsf, l0, l1, l2, l3, l4, l5))
        return true;

    return add_book_vocab(word, prefix, wsf, vocab_file, encounters, mm);
}



bool add_book_vocab(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters,
    matchable::MatchableMaker & mm
)
{
    if (encounters[word].is_nil())
    {
        encounters[word] = Encountered::Yes::grab();

        if (fputs(word.c_str(), vocab_file) == EOF || fputs("\n", vocab_file) == EOF)
        {
            std::cout << "add_book_vocab() --> failed to write to vocab_file!" << std::endl;
            return false;
        }

        add_word(word, prefix, wsf, mm);
    }

    return true;
}



void add_word(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    matchable::MatchableMaker & mm
)
{
    static parts_of_speech::Flags const empty_pos_flags;
    add_word(word, prefix, wsf, empty_pos_flags, mm);
}



void add_word(
    std::string const & word,
    std::string const & prefix,
    word_attribute::Flags const & wsf,
    parts_of_speech::Flags const & pos_flags,
    matchable::MatchableMaker & mm
)
{
    // create new variant
    std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
    mm.grab("word" + prefix)->add_variant(escaped);

    // property for parts of speech
    std::vector<std::string> property_values;
    for (auto p : parts_of_speech::variants_by_string())
    {
        if (pos_flags.is_set(p))
            property_values.push_back("1");
        else
            property_values.push_back("0");
    }
    mm.grab("word" + prefix)->set_propertyvect(escaped, "pos", property_values);

    // property for ordinal sum
    {
        int ordinal_sum = 0;
        int letter = 0;
        for (int letter_index = 0; letter_index < (int) word.size(); ++letter_index)
        {
            letter = (int) word[letter_index];
            if (letter > 96)
                letter -= 96;
            else if (letter > 64)
                letter -= 64;

            if (letter >= 1 && letter <= 26)
                ordinal_sum += letter;
        }
        mm.grab("word" + prefix)->set_property(escaped, "ordinal_summation", std::to_string(ordinal_sum));
    }

    // properties from word attributes
    {
        auto set_prop =
            [&](word_attribute::Type att)
            {
                if (wsf.is_set(att))
                {
                    std::string const prop_name = std::string("is_") + att.as_string();
                    mm.grab("word" + prefix)->set_property(escaped, prop_name, "1");
                }
            };

        set_prop(word_attribute::name::grab());
        set_prop(word_attribute::male_name::grab());
        set_prop(word_attribute::female_name::grab());
        set_prop(word_attribute::place::grab());
        set_prop(word_attribute::compound::grab());
        set_prop(word_attribute::acronym::grab());
        set_prop(word_attribute::phrase::grab());
    }
}



void tokenize(std::string const & str, char const delim, std::vector<std::string> & out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}
