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



MATCHABLE(
    pos_desc,
    Noun,
    Plural,
    Noun_Phrase,
    Verb_spc__pl_usu_spc_participle_pr_,
    Verb_spc__pl_transitive_pr_,
    Verb_spc__pl_intransitive_pr_,
    Adjective,
    Adverb,
    Conjunction,
    Preposition,
    Interjection,
    Pronoun,
    Definite_spc_Article,
    Indefinite_spc_Article,
    Nominative
);

PROPERTYx1_MATCHABLE(
    pos_desc::Type,
    pos_desc,
    pos,
    N,
    p,
    h,
    V,
    t,
    i,
    A,
    v,
    C,
    P,
    n, // !
    r,
    D,
    I,
    o
)

SET_PROPERTY(pos, N, pos_desc, pos_desc::Noun::grab())
SET_PROPERTY(pos, p, pos_desc, pos_desc::Plural::grab())
SET_PROPERTY(pos, h, pos_desc, pos_desc::Noun_Phrase::grab())
SET_PROPERTY(pos, V, pos_desc, pos_desc::Verb_spc__pl_usu_spc_participle_pr_::grab())
SET_PROPERTY(pos, t, pos_desc, pos_desc::Verb_spc__pl_transitive_pr_::grab())
SET_PROPERTY(pos, i, pos_desc, pos_desc::Verb_spc__pl_intransitive_pr_::grab())
SET_PROPERTY(pos, A, pos_desc, pos_desc::Adjective::grab())
SET_PROPERTY(pos, v, pos_desc, pos_desc::Adverb::grab())
SET_PROPERTY(pos, C, pos_desc, pos_desc::Conjunction::grab())
SET_PROPERTY(pos, P, pos_desc, pos_desc::Preposition::grab())
SET_PROPERTY(pos, n, pos_desc, pos_desc::Interjection::grab())
SET_PROPERTY(pos, r, pos_desc, pos_desc::Pronoun::grab())
SET_PROPERTY(pos, D, pos_desc, pos_desc::Definite_spc_Article::grab())
SET_PROPERTY(pos, I, pos_desc, pos_desc::Indefinite_spc_Article::grab())
SET_PROPERTY(pos, o, pos_desc, pos_desc::Nominative::grab())


MATCHABLE(
    word_status,
    not_printable_ascii,
    has_spaces,
    has_hyphens,
    has_other_symbols
)



void print_usage();

bool passes_prefix_filter(
    std::string const & word,
    std::string const & l0, // first letter
    std::string const & l1, // second letter
    std::string const & l2, // third letter
    std::string const & l3, // fourth letter
    std::string const & l4, // fifth letter
    std::string const & l5  // sixth letter
);

void read_3201_single(
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

void read_51155(
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

void update_word_status(word_status::Flags & flags, int & ch);

bool read_3201_single_line(FILE * f, std::string & word, word_status::Flags & status);

bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    word_status::Flags & status,
    pos::Flags & parts_of_speech
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

    for (auto desc : pos_desc::variants_by_index())
        mm.grab("pos_desc" + prefix)->add_variant(desc.as_identifier_string());

    mm.grab("pos" + prefix)->add_property("pos_desc" + prefix + "::Type", "pos_desc" + prefix);

    for (auto p : pos::variants_by_index())
    {
        mm.grab("pos" + prefix)->add_variant(p.as_identifier_string() + prefix);
        mm.grab("pos" + prefix)->set_property(
            p.as_identifier_string() + prefix,
            "pos_desc" + prefix,
            "pos_desc" + prefix + "::"
                       + pos_desc::from_index(p.as_index()).as_identifier_string() + "::grab()"
        );
    }

    mm.grab("word" + prefix)->add_property("pos" + prefix + "::Type", "pos" + prefix);

    {
        std::string const FN_3201_SINGLE{DATA_DIR + "/3201/files/SINGLE.TXT"};
        FILE * single_file = fopen(FN_3201_SINGLE.c_str(), "r");
        if (single_file == 0)
        {
            perror(FN_3201_SINGLE.c_str());
            exit(1);
        }
        read_3201_single(single_file, l0, l1, l2, l3, l4, l5, prefix, mm);
        fclose(single_file);
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

    {
        std::string const FN_51155{DATA_DIR + "/51155/51155-0.txt"};
        FILE * f = fopen(FN_51155.c_str(), "r");
        if (f == 0)
        {
            perror(FN_51155.c_str());
            exit(1);
        }
        read_51155(f, l0, l1, l2, l3, l4, l5, prefix, mm);
        fclose(f);
    }


    // remove leading underscore
    prefix.erase(0, 1);

    {
        auto sa_status = mm.save_as(
            OUTPUT_DIR + "/" + prefix + ".h",
            {matchable::save_as__content::matchables::grab()},
            matchable::save_as__spread_mode::wrap::grab()
        );

        std::cout << l0 << " ";
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
    if (word[0] != l0[0])
        return false;

    if (l1 != "nil")
    {
        if (word.size() > 1)
        {
            // if word does not second with l1 then fail
            if (word[1] != l1[0])
                return false;

            if (l2 != "nil")
            {
                if (word.size() > 2)
                {
                    // if 3+ letter word does not third with l2 then fail
                    if (word[2] != l2[0])
                        return false;

                    if (l3 != "nil")
                    {
                        if (word.size() > 3)
                        {
                            // if 4+ letter word does not fourth with l3 then fail
                            if (word[3] != l3[0])
                                return false;

                            if (l4 != "nil")
                            {
                                if (word.size() > 4)
                                {
                                    // if 5+ letter word does not fifth with l4 then fail
                                    if (word[4] != l4[0])
                                        return false;

                                    if (l5 != "nil")
                                    {
                                        if (word.size() > 5)
                                        {
                                            // if 6+ letter word does not sixth with l5 then fail
                                            if (word[5] != l5[0])
                                                return false;
                                        }
                                        else
                                        {
                                            // include five letter word of l0 + l1 + l2 + l3 + l4 when
                                            // l5 is 'a', otherwise fail
                                            if (l5[0] != 'a')
                                                return false;
                                        }
                                    }
                                }
                                else
                                {
                                    // include four letter word of l0 + l1 + l2 + l3 when l4 is 'a',
                                    // otherwise fail
                                    if (l4[0] != 'a')
                                        return false;
                                }
                            }
                        }
                        else
                        {
                            // include three letter word of l0 + l1 + l2 when l3 is 'a', otherwise fail
                            if (l3[0] != 'a')
                                return false;
                        }
                    }
                }
                else
                {
                    // include two letter word of l0 + l1 when l2 is 'a', otherwise fail
                    if (l2[0] != 'a')
                        return false;
                }
            }
        }
        else
        {
            // fail one letter word unless l1 is 'a' or 'nil'
            if (l1[0] != 'a')
                return false;
        }
    }

    return true;
}



void read_3201_single(
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
    word_status::Flags status;

    while (true)
    {
        if (!read_3201_single_line(input_file, word, status))
            break;

        if (word.size() == 0)
            continue;

        if (!passes_prefix_filter(word, l0, l1, l2, l3, l4, l5))
            continue;

        if (status.is_set(word_status::not_printable_ascii::grab()))
            continue;

        if (status.is_set(word_status::has_spaces::grab()))
            continue;

        if (status.is_set(word_status::has_hyphens::grab()))
            continue;

        if (status.is_set(word_status::has_other_symbols::grab()))
            continue;

        std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
        mm.grab("word" + prefix)->add_variant(escaped);
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
    pos::Flags pos_flags;
    word_status::Flags status;

    while (true)
    {
        if (!read_3203_mobypos_line(input_file, word, status, pos_flags))
            break;

        if (word.size() == 0)
            continue;

        if (!passes_prefix_filter(word, l0, l1, l2, l3, l4, l5))
            continue;

        if (status.is_set(word_status::not_printable_ascii::grab()))
            continue;

        if (status.is_set(word_status::has_spaces::grab()))
            continue;

        if (status.is_set(word_status::has_hyphens::grab()))
            continue;

        if (status.is_set(word_status::has_other_symbols::grab()))
            continue;

        std::string const escaped = "esc_" + matchable::escapable::escape_all(word);

        mm.grab("word" + prefix)->add_variant(escaped);
        std::vector<std::string> svect;
        for (auto p : pos_flags.currently_set())
            svect.push_back("pos" + prefix + "::" + p.as_identifier_string() + prefix + "::grab()");
        mm.grab("word" + prefix)->set_propertyvect(escaped, "pos" + prefix, svect);
    }
}



void read_51155(
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
    word_status::Flags status;
    bool ok{false};
    int ch{0};

    while (true)
    {
        ok = false;

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
                            ok = true;
                    }
                }
            }
        }
        else if (ch == 'S')
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
                            ok = true;
                    }
                }
            }
        }
        else if (ch == 'A')
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
                            ok = true;
                    }
                }
            }
        }

        if (ok)
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
                    if (!passes_prefix_filter(word, l0, l1, l2, l3, l4, l5))
                        continue;

                    if (status.is_set(word_status::not_printable_ascii::grab()))
                        continue;

                    if (status.is_set(word_status::has_spaces::grab()))
                        continue;

                    if (status.is_set(word_status::has_hyphens::grab()))
                        continue;

                    if (status.is_set(word_status::has_other_symbols::grab()))
                        continue;

                    std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
                    mm.grab("word" + prefix)->add_variant(escaped);
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



bool read_3201_single_line(
    FILE * f,
    std::string & word,
    word_status::Flags & status
)
{
    word.clear();
    status.clear();

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

        update_word_status(status, ch);
        word += (char) ch;
    }

    return true;
}



bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    word_status::Flags & status,
    pos::Flags & parts_of_speech
)
{
    word.clear();
    status.clear();
    parts_of_speech.clear();

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

        update_word_status(status, ch);
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
            status.set(word_status::not_printable_ascii::grab());

        if (ch == (int) '!')
            ch = (int) 'n';

        auto ch_str = std::string(1, (char) ch);
        auto pos_flag = pos::from_string(ch_str);
        if (!pos_flag.is_nil())
            parts_of_speech.set(pos_flag);
    }

    return true;
}
