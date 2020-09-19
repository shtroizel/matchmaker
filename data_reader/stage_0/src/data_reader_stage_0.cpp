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

#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchable/MatchableMaker.h>

#include <matchmaker/parts_of_speech.h>


int const MAX_WORD_LENGTH{44};


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
    acronym
)



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

void update_word_attribute(word_attribute::Flags & flags, int & ch);

bool read_3201_default_line(FILE * f, std::string & word, word_attribute::Flags & status);

bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    word_attribute::Flags & status,
    parts_of_speech::Flags & pos
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



int main(int argc, char ** argv)
{
    if (argc != 9)
    {
        print_usage();
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const OUTPUT_DIR{argv[2]};

    std::string l0{argv[3]};
    if (l0.size() != 1)
    {
        print_usage();
        return 2;
    }
    if (l0[0] < 'A' || (l0[0] > 'Z' && l0[0] < 'a') || l0[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string l1{argv[4]};
    if (l1.size() != 1 && l1 != "nil")
    {
        print_usage();
        return 2;
    }
    if (l1[0] < 'A' || (l1[0] > 'Z' && l1[0] < 'a') || l1[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string l2{argv[5]};
    if (l2.size() != 1 && l2 != "nil")
    {
        print_usage();
        return 2;
    }
    if (l2[0] < 'A' || (l2[0] > 'Z' && l2[0] < 'a') || l2[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string l3{argv[6]};
    if (l3.size() != 1 && l3 != "nil")
    {
        print_usage();
        return 2;
    }
    if (l3[0] < 'A' || (l3[0] > 'Z' && l3[0] < 'a') || l3[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string l4{argv[7]};
    if (l4.size() != 1 && l4 != "nil")
    {
        print_usage();
        return 2;
    }
    if (l4[0] < 'A' || (l4[0] > 'Z' && l4[0] < 'a') || l4[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string l5{argv[8]};
    if (l5.size() != 1 && l5 != "nil")
    {
        print_usage();
        return 2;
    }
    if (l5[0] < 'A' || (l5[0] > 'Z' && l5[0] < 'a') || l5[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string prefix{"_" + l0};
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

    matchable::MatchableMaker mm;

    mm.grab("word" + prefix)->add_property("int8_t", "pos");
    mm.grab("word" + prefix)->add_property("int", "syn");
    mm.grab("word" + prefix)->add_property("int", "ant");
    mm.grab("word" + prefix)->add_property("int", "by_longest_index");

    // "word_attribute" properties
    {
    auto add_att_prop =
        [&](word_attribute::Type att)
        {
            std::string const prop_name = "is_" + att.as_string();
            mm.grab("word" + prefix)->add_property("int8_t", prop_name);
        };

        add_att_prop(word_attribute::name::grab());
        add_att_prop(word_attribute::male_name::grab());
        add_att_prop(word_attribute::female_name::grab());
        add_att_prop(word_attribute::place::grab());
        add_att_prop(word_attribute::compound::grab());
        add_att_prop(word_attribute::acronym::grab());
    }

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


    // remove leading underscore
    prefix.erase(0, 1);

    {
        auto sa_status = mm.save_as(
            OUTPUT_DIR + "/" + prefix + ".h",
            {matchable::save_as__content::matchables::grab()},
            matchable::save_as__spread_mode::wrap::grab()
        );

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
        std::cout << "---------> " << sa_status << std::endl;

        if (sa_status != matchable::save_as__status::success::grab())
            return 1;
    }

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



bool has_responsibility(char letter, char prefix_element)
{
    if (letter == prefix_element)
        return true;

    // support symbols supported by MATCHABLE
    // store them all under the left most leaf
    if (prefix_element == 'A')
    {
        for (auto const & [code, symbol] : matchable::escapable::code_symbol_pairs())
            if (symbol.length() > 0 && symbol[0] == letter)
                return true;
    }

    return false;
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
    if (!has_responsibility(word[0], l0[0]))
        return false;

    if (l1 != "nil")
    {
        if (word.size() > 1)
        {
            // if word does not second with l1 then fail
            if (!has_responsibility(word[1], l1[0]))
                return false;

            if (l2 != "nil")
            {
                if (word.size() > 2)
                {
                    // if 3+ letter word does not third with l2 then fail
                    if (!has_responsibility(word[2], l2[0]))
                        return false;

                    if (l3 != "nil")
                    {
                        if (word.size() > 3)
                        {
                            // if 4+ letter word does not fourth with l3 then fail
                            if (!has_responsibility(word[3], l3[0]))
                                return false;

                            if (l4 != "nil")
                            {
                                if (word.size() > 4)
                                {
                                    // if 5+ letter word does not fifth with l4 then fail
                                    if (!has_responsibility(word[4], l4[0]))
                                        return false;

                                    if (l5 != "nil")
                                    {
                                        if (word.size() > 5)
                                        {
                                            // if 6+ letter word does not sixth with l5 then fail
                                            if (!has_responsibility(word[5], l5[0]))
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



void update_word_attribute(word_attribute::Flags & flags, int & ch)
{
    if (ch < 32 || ch > 126)
    {
        flags.set(word_attribute::invisible_ascii::grab());
        ch = '?';
    }
    else
    {
        if (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z')
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
                flags.set(word_attribute::unmatchable_symbols::grab());
        }
    }
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
    std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
    mm.grab("word" + prefix)->add_variant(escaped);
    std::vector<std::string> property_values;
    for (auto p : parts_of_speech::variants_by_string())
    {
        if (pos_flags.is_set(p))
            property_values.push_back("1");
        else
            property_values.push_back("0");
    }
    mm.grab("word" + prefix)->set_propertyvect(escaped, "pos", property_values);

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
    }
}
