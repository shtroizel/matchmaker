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
    read_line_status,
    eof,
    not_printable_ascii,
    has_spaces,
    has_hyphens,
    has_other_symbols
)



void print_usage();
bool passes_prefix_filter(
    std::string const & word,
    std::string const & fl, // first letter
    std::string const & sl, // second letter
    std::string const & tl  // third letter
);
void read_simple(
    FILE * input_file,
    std::string const & fl,
    std::string const & sl,
    std::string const & tl,
    std::string const & prefix,
    matchable::MatchableMaker & mm,
    std::vector<std::string> & encountered_words
);
void read_pos(
    FILE * input_file,
    std::string const & fl,
    std::string const & sl,
    std::string const & tl,
    std::string const & prefix,
    matchable::MatchableMaker & mm,
    std::vector<std::string> & encountered_words
);
void update_read_line_status(read_line_status::Flags & flags, int & ch);
read_line_status::Flags read_simple_line(FILE * f, std::string & word);
read_line_status::Flags read_pos_line(FILE * f, std::string & word, pos::Flags & parts_of_speech);


int main(int argc, char ** argv)
{
    if (argc != 6)
    {
        print_usage();
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const OUTPUT_DIR{argv[2]};

    std::string fl{argv[3]};
    if (fl.size() != 1)
    {
        print_usage();
        return 2;
    }
    if (fl[0] < 'A' || (fl[0] > 'Z' && fl[0] < 'a') || fl[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string sl{argv[4]};
    if (sl.size() != 1 && sl != "nil")
    {
        print_usage();
        return 2;
    }
    if (sl[0] < 'A' || (sl[0] > 'Z' && sl[0] < 'a') || sl[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string tl{argv[5]};
    if (tl.size() != 1 && tl != "nil")
    {
        print_usage();
        return 2;
    }
    if (tl[0] < 'A' || (tl[0] > 'Z' && tl[0] < 'a') || tl[0] > 'z')
    {
        print_usage();
        return 2;
    }

    std::string prefix{"_" + fl};
    if (sl != "nil")
    {
        prefix += "_" + sl;
        if (tl != "nil")
            prefix += "_" + tl;
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

    std::vector<std::string> encountered_words;


    std::string const SINGLE_FN{DATA_DIR + "/3201/files/SINGLE.TXT"};
    FILE * single_file = fopen(SINGLE_FN.c_str(), "r");
    if (single_file == 0)
    {
        perror(SINGLE_FN.c_str());
        exit(1);
    }
    read_simple(single_file, fl, sl, tl, prefix, mm, encountered_words);
    fclose(single_file);

    std::string const MOBYPOS_FN{DATA_DIR + "/3203/files/mobypos.txt"};
    FILE * mobypos_file = fopen(MOBYPOS_FN.c_str(), "r");
    if (mobypos_file == 0)
    {
        perror(MOBYPOS_FN.c_str());
        exit(1);
    }
    read_pos(mobypos_file, fl, sl, tl, prefix, mm, encountered_words);
    fclose(mobypos_file);


    // remove nonsense
    //
    // there are a huge amount of words starting with "non" - so many that to properly support we need
    // to go 4 deep with the prefix...
    //
    // until then any "non" word that exists without the "non" prefix gets removed!
    // until then encourage ppl skilz (practice positive language!) who needs non words anyway?
    if (fl == "n" && sl == "o" && tl == "n")
    {
        std::string const nonword_prefix{"esc_non"};
        auto word_matchable = mm.grab("word" + prefix);
        std::vector<std::string> nonwords;
        std::vector<std::string> nonword_pos_vect;
        std::string nonword_with_non_removed;
        word_matchable->variants_starting_with(nonword_prefix, nonwords);
        for (auto const & nonword : nonwords)
        {
            nonword_with_non_removed = nonword.substr(nonword_prefix.size());
            if (
                std::find(
                    encountered_words.begin(),
                    encountered_words.end(),
                    nonword_with_non_removed
                ) != encountered_words.end()
            )
            {
                nonword_pos_vect.clear();
                word_matchable->get_propertyvect(nonword, "pos" + prefix, nonword_pos_vect);
                for (auto p : nonword_pos_vect)
                    mm.grab(p.substr(11, 7))->del_variant(nonword);

                word_matchable->del_variant(nonword);
            }
        }
    }

    // remove leading underscore
    prefix.erase(0, 1);

    {
        auto sa_status = mm.save_as(
            OUTPUT_DIR + "/" + prefix + ".h",
            {matchable::save_as__content::matchables::grab()},
            matchable::save_as__spread_mode::wrap::grab()
        );

        std::cout << fl << " ";
        if (sl == "nil")
            std::cout << "--";
        else
            std::cout << sl << " ";
        if (tl == "nil")
            std::cout << "--";
        else
            std::cout << tl << " ";
        std::cout << "---------------> " << sa_status << std::endl;
    }

    return 0;
}



void print_usage()
{
    std::cout << "program expects 5 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  output directory\n"
              << "\n"
              << "    * letter arguments form an inclusive prefix filter\n"
              << "    * letters are case sensitive\n"
              << "\n"
              << "    [3]  first letter\n"
              << "         - include words starting with 'first letter'\n"
              << "         - single letter word of 'first letter' is included when second letter is 'a'\n"
              << "           and 'third letter' is either 'a' or 'nil'\n"
              << "    [4]  second letter\n"
              << "         - include words seconding with 'second letter'\n"
              << "         - two letter word of 'first letter' + 'second letter' is included when \n"
              << "           'third letter' is either 'a' or 'nil'\n"
              << "         - can be disabled for single letter prefix by setting to 'nil'\n"
              << "    [5]  third letter\n"
              << "         - include words thirding with 'third letter'\n"
              << "         - can be disabled for two letter prefix by setting to 'nil'\n"
              << "         - ignored when second letter is 'nil'\n"
              << std::flush;
}



bool passes_prefix_filter(
    std::string const & word,
    std::string const & fl,
    std::string const & sl,
    std::string const & tl
)
{
    if (word.size() == 0)
        return false;

    // if word does not start with first letter then fail
    if (word[0] != fl[0])
        return false;

    if (sl != "nil")
    {
        if (word.size() > 1)
        {
            // if word does not second with second letter then fail
            if (word[1] != sl[0])
                return false;

            if (tl != "nil")
            {
                if (word.size() > 2)
                {
                    // if 3+ letter word does not third with third letter then fail
                    if (word[2] != tl[0])
                        return false;
                }
                else
                {
                    // include two letter word of 'first letter' + 'second letter' when
                    // third letter is 'a', otherwise fail
                    if (tl[0] != 'a')
                        return false;
                }
            }
        }
        else
        {
            // fail one letter word unless second letter is 'a' or 'nil'
            if (sl[0] != 'a')
                return false;
        }
    }

    return true;
}



void read_simple(
    FILE * input_file,
    std::string const & fl,
    std::string const & sl,
    std::string const & tl,
    std::string const & prefix,
    matchable::MatchableMaker & mm,
    std::vector<std::string> & encountered_words
)
{
    std::string word;
    read_line_status::Flags status;

    while (true)
    {
        status = read_simple_line(input_file, word);
        if (status.is_set(read_line_status::eof::grab()))
            break;

        if (word.size() == 0)
            continue;

        encountered_words.push_back(word);

        if (!passes_prefix_filter(word, fl, sl, tl))
            continue;

        if (status.is_set(read_line_status::not_printable_ascii::grab()))
            continue;

        if (status.is_set(read_line_status::has_spaces::grab()))
            continue;

        if (status.is_set(read_line_status::has_hyphens::grab()))
            continue;

        if (status.is_set(read_line_status::has_other_symbols::grab()))
            continue;

        std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
        mm.grab("word" + prefix)->add_variant(escaped);
    }
}



void read_pos(
    FILE * input_file,
    std::string const & fl,
    std::string const & sl,
    std::string const & tl,
    std::string const & prefix,
    matchable::MatchableMaker & mm,
    std::vector<std::string> & encountered_words
)
{
    std::string word;
    pos::Flags pos_flags;
    read_line_status::Flags status;

    while (true)
    {
        status = read_pos_line(input_file, word, pos_flags);
        if (status.is_set(read_line_status::eof::grab()))
            break;

        if (word.size() == 0)
            continue;

        encountered_words.push_back(word);

        if (!passes_prefix_filter(word, fl, sl, tl))
            continue;

        if (status.is_set(read_line_status::not_printable_ascii::grab()))
            continue;

        if (status.is_set(read_line_status::has_spaces::grab()))
            continue;

        if (status.is_set(read_line_status::has_hyphens::grab()))
            continue;

        if (status.is_set(read_line_status::has_other_symbols::grab()))
            continue;

        std::string const escaped = "esc_" + matchable::escapable::escape_all(word);

        mm.grab("word" + prefix)->add_variant(escaped);
        std::vector<std::string> svect;
        for (auto p : pos_flags.currently_set())
            svect.push_back("pos" + prefix + "::" + p.as_identifier_string() + prefix + "::grab()");
        mm.grab("word" + prefix)->set_propertyvect(escaped, "pos" + prefix, svect);
    }
}



void update_read_line_status(read_line_status::Flags & flags, int & ch)
{
    if (ch < 32 || ch > 126)
    {
        flags.set(read_line_status::not_printable_ascii::grab());
        ch = '?';
    }
    else if (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z')
    {
        if (ch == ' ')
            flags.set(read_line_status::has_spaces::grab());
        else if (ch == '-')
            flags.set(read_line_status::has_hyphens::grab());
        else
            flags.set(read_line_status::has_other_symbols::grab());
    }
}



read_line_status::Flags read_simple_line(FILE * f, std::string & word)
{
    word.clear();
    read_line_status::Flags ret;

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
        {
            ret.set(read_line_status::eof::grab());
            break;
        }

        if (ch == 10 || ch == 13)
        {
            while (true)
            {
                ch = fgetc(f);

                if (ch == EOF)
                    break;

                if (ch != 10 && ch != 13)
                {
                    ungetc(ch, f);
                    break;
                }
            }
            break;
        }

        update_read_line_status(ret, ch);
        word += (char) ch;
    }

    return ret;
}



read_line_status::Flags read_pos_line(FILE * f, std::string & word, pos::Flags & parts_of_speech)
{
    word.clear();
    parts_of_speech.clear();
    read_line_status::Flags ret;

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
        {
            ret.set(read_line_status::eof::grab());
            return ret;
        }

        if (ch == 10 || ch == 13)
            continue;

        if (ch == (int) '\\')
            break;

        update_read_line_status(ret, ch);

        word += (char) ch;
    }

    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
        {
            ret.set(read_line_status::eof::grab());
            break;
        }

        if (ch == 10 || ch == 13)
            break;

        if (ch < 32 || ch > 126)
            ret.set(read_line_status::not_printable_ascii::grab());

        if (ch == (int) '!')
            ch = (int) 'n';

        auto ch_str = std::string(1, (char) ch);
        auto pos_flag = pos::from_string(ch_str);
        if (!pos_flag.is_nil())
            parts_of_speech.set(pos_flag);
    }

    return ret;
}
