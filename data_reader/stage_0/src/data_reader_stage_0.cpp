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

int const MAX_PREFIX_DEPTH{6};


class Prefix
{
public:
    Prefix() : maker{new matchable::MatchableMaker()} {}
    ~Prefix() {}
    Prefix(Prefix const &) = delete;
    Prefix(Prefix && other) : str{other.str}, maker{std::move(other.maker)}
    {
        other.str.clear();
        other.maker = nullptr;
    }
    Prefix & operator=(Prefix const &) = delete;
    Prefix & operator=(Prefix && other)
    {
        if (this != &other)
        {
            str = std::move(other.str);
            maker = std::move(other.maker);
        }
        return *this;
    }

    void initialize(std::string const & prefix)
    {
        str = prefix;
        std::string const matchable_name = "word_" + escaped_and_delimited('_');
        maker->grab(matchable_name)->add_property("int8_t", "pos");
        maker->grab(matchable_name)->add_property("int", "syn");
        maker->grab(matchable_name)->add_property("int", "ant");
        maker->grab(matchable_name)->add_property("int", "by_longest_index");
        maker->grab(matchable_name)->add_property("int", "ordinal_summation");
        maker->grab(matchable_name)->add_property("int", "embedded");
        maker->grab(matchable_name)->add_property("int", "book_index");
        maker->grab(matchable_name)->add_property("int", "chapter_index");
        maker->grab(matchable_name)->add_property("int", "paragraph_index");
        maker->grab(matchable_name)->add_property("int", "word_index");

        // "word_attribute" properties
        {
            auto add_bool_prop =
                [&](std::string const & prop)
                {
                    std::string const prop_name = "is_" + prop;
                    maker->grab(matchable_name)->add_property("int8_t", prop_name);
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
    }

    std::string const & as_string() const { return str; }
    std::string & as_mutable_string() { return str; }
    matchable::MatchableMaker const & as_maker() const { return *maker; }
    matchable::MatchableMaker & as_mutable_maker() { return *maker; }


    // bool operator<(Prefix const & other) const
    // {
    //     return str  < other.str;
    // }


    std::string escaped_and_delimited(char delim) const
    {
        std::string ret;

        if (str.empty())
        {
            std::cout << "Prefix::escaped_and_delimited('"
                      << delim << "') --> str is empty!" << std::endl;
            abort();
        }

        for (size_t i = 0; i < str.size(); ++i)
        {
            std::string escaped = matchable::escapable::escape(str.substr(i, 1));
            if (escaped.size() > 2)
            {
                if (escaped[0] != escaped[escaped.size() - 1])
                {
                    std::cout << "Prefix::escaped_and_delimited('" << delim << "')\n"
                              << "    --> escaped.size() > 2 but first and last do not match!\n"
                              << "    --> str is: '" << str << "'" << std::endl;
                    abort();
                }
            }
            else if (escaped.size() != 1)
            {
                std::cout << "Prefix::escaped_and_delimited('" << delim << "')\n"
                        << "    --> escape failed with: '" << escaped << "'\n"
                        << "    -->             str is: '" << str << "'" << std::endl;
                abort();
            }

            if (i > 0)
                ret += delim;

            // manually explicitly escape numbers since matchable escapes numbers to themselves.
            if (str[i] >= '0' && str[i] <= '9')
                ret += "esc_";
            ret += escaped;
        }

        // std::cout << "Prefix::escaped_and_delimited('"
        //           << delim << "') /* with str: '" << str << "' */ -> " << ret << "\n" << std::endl;
        return ret;
    }

    std::string save_path(std::string const OUTPUT_DIR) const
    {
        if (str.empty())
        {
            std::cout << "Prefix::save_path() called but str is empty!" << std::endl;
            abort();
        }

        std::string ret = OUTPUT_DIR;
        ret += "/";
        ret += escaped_and_delimited('/');
        ret += "/";
        ret += escaped_and_delimited('_');
        ret += ".h";
        // std::cout << "Prefix::save_path('" << OUTPUT_DIR << "') -> '" << ret << "'\n" << std::endl;
        return ret;
    }

private:
    std::string str;
    std::unique_ptr<matchable::MatchableMaker> maker;
};

// all prefixes of depth 1 are symbols, and their makers are stored here
std::array<Prefix, 23> symbols_1d_prefixes;


// matchable makers for prefixes of depth 2 to 5
std::vector<Prefix> prefixes_2d_to_5d;

// index lookup table for "prefixes_2d_to_5d"
using LookupTable2dTo5d =
        std::array<std::array<std::array<std::array<std::array<int16_t, 52>, 52>, 52>, 52>, 52>;
static LookupTable2dTo5d lookup_table_2d_to_5d =
    []()
    {
        LookupTable2dTo5d t;
        for (auto & d1 : t)
            for (auto & d2 : d1)
                for (auto & d3 : d2)
                    for (auto & d4 : d3)
                        for (int16_t & i : d4)
                            i = -1;
        return t;
    }();

// A 6D lookup table would require too much RAM,
// so the 6th dimension is handled separately
std::vector<Prefix> prefixes_6d;


int16_t calc_letter_index(char ch);

Prefix * prefix_for_d1_symbol(char sym);

bool read_prefixes(FILE * f,
                   std::string const & OUTPUT_DIR,
                   LookupTable2dTo5d & lookup_table_2d_to_5d,
                   std::vector<Prefix> & prefixes_6d);

bool read_prefix(FILE * f, std::array<std::string, MAX_PREFIX_DEPTH> & prefix, bool & ok);

bool char_is_letter(char ch);

Prefix * lookup(std::string const & str);




MATCHABLE(Encountered, Yes)


void print_usage();

void read_3201_default(
    FILE * input_file,
    word_attribute::Flags const & base_attributes
);

void read_3202(FILE * input_file);

void read_3203_mobypos(FILE * input_file);

bool read_crumbs(
    word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * book_vocab_file,
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

bool add_book_vocab(
    std::string const & word,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters
);

void add_word(
    std::string const & word,
    word_attribute::Flags const & wsf
);
void add_word(
    std::string const & word,
    word_attribute::Flags const & wsf,
    parts_of_speech::Flags const & pos_flags
);

void tokenize(std::string const & str, char const delim, std::vector<std::string> & out);


bool symbols_off = false; // disables words with symbols when this is true


void foreach_prefix(std::function<void (Prefix & p)> func)
{
    for (auto & p : symbols_1d_prefixes)
        func(p);

    for (auto & p : prefixes_2d_to_5d)
        func(p);

    for (auto & p : prefixes_6d)
        func(p);
}



int main(int argc, char ** argv)
{
    if (argc < 5 || argc > 6)
    {
        std::cout << "invalid usage!" << std::endl;
        return 2;
    }

    std::string const DATA_DIR{argv[1]};
    std::string const BOOK_VOCAB_DIR{argv[2]};
    std::string const OUTPUT_DIR{argv[3]};
    std::string const PREFIX_FILENAME{argv[4]};

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

    prefixes_2d_to_5d.reserve(1000);

    // get all prefixes from file
    {
        FILE * f = fopen(PREFIX_FILENAME.c_str(), "r");
        if (nullptr == f)
        {
            perror(PREFIX_FILENAME.c_str());
            exit(1);
        }
        bool ok = read_prefixes(f, OUTPUT_DIR, lookup_table_2d_to_5d, prefixes_6d);
        fclose(f);
        if (!ok)
        {
            std::cout << "failed to read prefix file: " << PREFIX_FILENAME << std::endl;
            exit(1);
        }
    }

    // std::stable_sort(prefixes_6d.begin(), prefixes_6d.end());





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
            read_3201_default(single_file, base_attributes);
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
            read_3201_default(compound_file, base_attributes);
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
            read_3201_default(common_file, base_attributes);
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
            read_3201_default(names_file, base_attributes);
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
            read_3201_default(names_f_file, base_attributes);
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
            read_3201_default(names_m_file, base_attributes);
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
            read_3201_default(places_file, base_attributes);
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
            read_3201_default(crosswd_file, base_attributes);
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
            read_3201_default(crswd_d_file, base_attributes);
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
            read_3201_default(acronyms_file, base_attributes);
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
            read_3202(input_file);
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
            read_3203_mobypos(mobypos_file);
            fclose(mobypos_file);
        }
    }

    if (q_mode != q_usage::omitted::grab())
    {
        int text = 0;
        FILE * q_file = nullptr;

        std::string const crumbs_path = BOOK_VOCAB_DIR + "/Crumbs";
        std::filesystem::create_directory(crumbs_path);
        std::string const crumbs_vocab = BOOK_VOCAB_DIR + "/Crumbs/vocabulary";
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
            bool ok = read_crumbs(base_attributes, q_file, vocab_file, text);
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
//         bool ok = read_crumbs(q_file, base_attributes, text);
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
            add_word(s, word_attributes, pos);
    }


    std::cout << "generating stage 0 matchables... " << std::endl;

    // save save save
    foreach_prefix(
        [&OUTPUT_DIR](Prefix & p)
        {
            std::string path = p.save_path(OUTPUT_DIR);
            // std::cout << "    --> " << path << std::endl;
            auto sa_status = p.as_mutable_maker().save_as(
                path,
                {matchable::save__content::matchables::grab()},
                matchable::save__grow_mode::wrap::grab()
            );
            if (sa_status != matchable::save__status::success::grab())
            {
                std::cout << "failed to save prefix: '" << p.as_string() << "'\n"
                          << "              to file: '" << p.save_path(OUTPUT_DIR) << "'\n"
                          << "    with save__status: '" << sa_status << "'" << std::endl;
                abort();
            }
        }
    );

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


bool read_prefixes(FILE * f,
                   std::string const & OUTPUT_DIR,
                   LookupTable2dTo5d & lookup_table_2d_to_5d,
                   std::vector<Prefix> & prefixes_6d)
{
    prefixes_6d.clear();

    if (OUTPUT_DIR.empty())
        return false;

    std::array<std::string, MAX_PREFIX_DEPTH> p;
    std::array<int16_t, 6> p_i = { -1, -1, -1, -1, -1, -1 };

    bool ok{true};
    while (read_prefix(f, p, ok))
    {
        if (!ok)
            return false;

        std::string p_as_str;
        for (int i = 0; i < MAX_PREFIX_DEPTH && p[i] != "nil"; ++i)
        {
            if (p[i].empty())
            {
                std::string prefix;
                for (auto const & sym : p)
                {
                    if (prefix.empty())
                        prefix += " ";
                    prefix += "'";
                    prefix += sym;
                    prefix += "'";
                }
                std::cout << "read_prefixes() --> invalid prefix: " << prefix << std::endl;
                abort();
            }
            p_as_str += p[i];
        }

        if (p[0] == "nil")
        {
            std::cout << "read_prefixes() --> invalid prefix: " << p_as_str << std::endl;
            abort();
        }

        // handle 1d prefix (only symbols have depth of 1)
        if (p[1] == "nil")
        {
            // std::cout << "    --> 1d prefix!" << std::endl;
            if (p[0].empty())
            {
                std::cout << "first char of prefix is null (failed to load prefix file)" << std::endl;
                return false;
            }

            Prefix * prefix = prefix_for_d1_symbol(p[0][0]);
            if (nullptr == prefix)
            {
                std::cout << "depth is 1, failed to get maker for symbol: '" << p[0][0] << "'" << std::endl;
                return false;
            }

            prefix->initialize(p_as_str);
            continue;
        }

        int16_t const maker_index{(int16_t) prefixes_2d_to_5d.size()};

        p_i[0] = calc_letter_index(p[0][0]);
        p_i[1] = calc_letter_index(p[1][0]);
        p_i[2] = calc_letter_index(p[2][0]);
        p_i[3] = calc_letter_index(p[3][0]);
        p_i[4] = calc_letter_index(p[4][0]);
        p_i[5] = calc_letter_index(p[5][0]);

        // handle 2d prefix
        if (p[2] == "nil")
        {
            std::array<std::array<std::array<int16_t, 52>, 52>, 52> & d3 = lookup_table_2d_to_5d [p_i[0]] [p_i[1]];
            for (auto & d4 : d3)
                for (auto & d5 : d4)
                    for (auto & i : d5)
                        i = maker_index;
        }

        // handle 3d prefix
        else if (p[3] == "nil")
        {
            std::array<std::array<int16_t, 52>, 52> & d4 = lookup_table_2d_to_5d [p_i[0]] [p_i[1]] [p_i[2]];
            for (auto & d5 : d4)
                for (auto & i : d5)
                    i = maker_index;
        }

        // handle 4d prefix
        else if (p[4] == "nil")
        {
            std::array<int16_t, 52> & d5 = lookup_table_2d_to_5d [p_i[0]] [p_i[1]] [p_i[2]] [p_i[3]];
            for (auto & i : d5)
                i = maker_index;
        }

        // handle 5d prefix
        else if (p[5] == "nil")
        {
            lookup_table_2d_to_5d [p_i[0]] [p_i[1]] [p_i[2]] [p_i[3]] [p_i[4]] = maker_index;
        }

        // handle 6d prefix
        else
        {
            Prefix p_6d;
            p_6d.initialize(p_as_str);
            prefixes_6d.push_back(std::move(p_6d));
            continue;
        }

        // finish prefix for depths 2 to 5
        Prefix p_2_to_5d;
        p_2_to_5d.initialize(p_as_str);
        prefixes_2d_to_5d.push_back(std::move(p_2_to_5d));
    }

    return true;
}


bool read_prefix(FILE * f, std::array<std::string, MAX_PREFIX_DEPTH> & prefix, bool & ok)
{
    for (auto & sym : prefix)
        sym.clear();

    int ch;
    int i{0};

    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        if (ch == ' ')
        {
            ++i;
            if (i < MAX_PREFIX_DEPTH)
                continue;

            ok = false;
            return false;
        }

        prefix[i] += (char) ch;
    }

    return true;
}


int16_t calc_letter_index(char ch)
{
    int leaf_index{0};

    if (ch >= 'A' && ch <= 'Z')
    {
        leaf_index = (int16_t) ch - 'A';
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        leaf_index = (int16_t) ch - 'a' + ('Z' - 'A') + 1;
    }

    if (leaf_index < 0)
    {
        std::cout << "calc_letter_index(" << ch << ") --> leaf_index < 0!";
        abort();
    }

    if (leaf_index > 51)
    {
        std::cout << "calc_letter_index(" << ch << ") --> leaf_index > 51!";
        abort();
    }

    return leaf_index;
}


Prefix * prefix_for_d1_symbol(char sym)
{
    //  0  "\"
    //  1  "#"
    //  2  "$"

    //  3  "'"
    //  4  "("

    //  5  "+"

    //  6  "-"
    //  7  "."
    //  8  "/"
    //  9  "0"
    // 10  "1"
    // 11  "2"
    // 12  "3"
    // 13  "4"
    // 14  "5"
    // 15  "6"
    // 16  "7"
    // 17  "8"
    // 18  "9"
    // 19  ":"

    // 20  ">"

    // 21  "_"

    // 22  "~"

    // check largest ranges first...


    // 7th - 20th
    if (sym >= '-' && sym <= ':')
        return &symbols_1d_prefixes[(int16_t) (sym - '-' + 6)];

    // first 3
    if (sym >= '"' && sym <= '$')
        return &symbols_1d_prefixes[(int16_t) (sym - '"')];

    // 4th and 5th
    if (sym >= '\'' && sym <= '(')
        return &symbols_1d_prefixes[(int16_t) (sym - '\'' + 3)];

    // 6th
    if (sym == '+')
        return &symbols_1d_prefixes[5];

    // 21st
    if (sym == '>')
        return &symbols_1d_prefixes[20];

    // 22nd
    if (sym == '_')
        return &symbols_1d_prefixes[21];

    // 23rd
    if (sym == '~')
        return &symbols_1d_prefixes[22];

    return nullptr;
}


bool char_is_letter(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}


Prefix * lookup(std::string const & str)
{
    if (str.empty())
    {
        std::cout << "lookup() called with empty string!" << std::endl;
        abort();
    }

    // check for d1 symbol
    {
        Prefix * p = prefix_for_d1_symbol(str[0]);
        if (nullptr != p)
            return p;

        // refer any symbols at 1d to deeper depth - but in a way that precedes all letters
        if (str[0] < 'A' || (str[0] > 'Z' && str[0] < 'a') || str[0] > 'z')
        {
            int16_t l = lookup_table_2d_to_5d[0][0][0][0][0];
            if (l < 0 || l >= (int16_t) prefixes_2d_to_5d.size())
            {
                std::cout << "lookup() failed given string: \"" << str << "\"" << std::endl;
                abort();
            }
            return &prefixes_2d_to_5d[l];
        }
    }

    // first six chars as a string with short strings filled with 'A'
    std::string first_six{"AAAAAA"};
    for
    (
        int16_t str_i = 0;
        str_i < (int16_t) first_six.size() && str_i < (int16_t) str.size();
        ++str_i
    )
        first_six[str_i] = str[str_i];

    // check for 6D prefix
    auto it = std::lower_bound(
                    prefixes_6d.begin(),
                    prefixes_6d.end(),
                    first_six,
                    [](Prefix const & l, std::string const & r) { return l.as_string() < r; }
                );
    if (it != prefixes_6d.end() && first_six == it->as_string())
        return &*it;

    // 6D prefix not found so look for a shorter prefix,
    // but first we need the index for each dimension
    std::array<int16_t, 5> i = { 0, 0, 0, 0, 0 };
    for (int16_t str_i = 0; str_i < (int16_t) str.length() && str_i < 5; ++str_i)
    {
        i[str_i] = calc_letter_index(str[str_i]);
    }

    int16_t l = lookup_table_2d_to_5d[i[0]][i[1]][i[2]][i[3]][i[4]];
    if (l < 0)
        goto err;
    if (l >= (int16_t) prefixes_2d_to_5d.size())
        goto err;

    return &prefixes_2d_to_5d[l];

err:
    std::cout << "lookup() failed given string: \"" << str << "\"" << std::endl;
    abort();

    return nullptr;
}


void read_3201_default(
    FILE * input_file,
    word_attribute::Flags const & base_attributes
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

        if (attributes.is_set(word_attribute::compound::grab()) &&
                word.find('-') == std::string::npos && word.find(' ') == std::string::npos)
            attributes.unset(word_attribute::compound::grab());

        add_word(word, attributes);
    }
}



void read_3202(FILE * input_file)
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

        add_word(word, attributes, pos_flags);

        if (ch == EOF)
            break;
    }
}



void read_3203_mobypos(FILE * input_file)
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

        add_word(word, attributes, pos_flags);
    }
}



bool read_crumbs(
    word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
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

        if (line.empty())
            continue;

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

            if (!add_book_vocab(text_line, attributes, vocab_file, encounters))
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
                if (!add_book_vocab(word, attributes, vocab_file, encounters))
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
                if (!add_book_vocab(line, att, vocab_file, encounters))
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

                                if (!add_book_vocab(slash_token, attributes, vocab_file, encounters))
                                    return false;

                                // split on '='
                                std::vector<std::string> tokenized_on_eq;
                                tokenize(slash_token, '=', tokenized_on_eq);
                                for (auto & eq_token : tokenized_on_eq)
                                    if (!add_book_vocab(eq_token, attributes, vocab_file, encounters))
                                        return false;

                                // for words with brackets, add version of word without brackets
                                bool had_brackets = strip_word('[', slash_token);
                                had_brackets = strip_word(']', slash_token) || had_brackets;
                                if (had_brackets)
                                {
                                    if (!add_book_vocab(slash_token, attributes, vocab_file,
                                                        encounters))
                                        return false;
                                }
                            }
                        }
                        else
                        {
                            if (!add_book_vocab(token, attributes, vocab_file, encounters))
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
                            bool has_space = false;
                            for (int i = 0; !has_space && i < (int) phrase.length(); ++i)
                                if (phrase[i] == ' ')
                                    has_space = true;

                            word_attribute::Flags att = attributes;
                            if (has_space)
                                att.set(word_attribute::phrase::grab());

                            add_book_vocab(phrase, att, vocab_file, encounters);
                            // std::cout << "adding phrase: " << phrase << std::endl;
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



bool add_book_vocab(
    std::string const & word,
    word_attribute::Flags const & wsf,
    FILE * vocab_file,
    std::map<std::string, Encountered::Type> & encounters
)
{
    if (!word.empty() && encounters[word].is_nil())
    {
        encounters[word] = Encountered::Yes::grab();

        if (fputs(word.c_str(), vocab_file) == EOF || fputs("\n", vocab_file) == EOF)
        {
            std::cout << "add_book_vocab() --> failed to write to vocab_file!" << std::endl;
            return false;
        }

        add_word(word, wsf);
    }

    return true;
}



void add_word(
    std::string const & word,
    word_attribute::Flags const & wsf
)
{
    static parts_of_speech::Flags const empty_pos_flags;
    add_word(word, wsf, empty_pos_flags);
}



void add_word(
    std::string const & word,
    word_attribute::Flags const & wsf,
    parts_of_speech::Flags const & pos_flags
)
{
    Prefix * p = lookup(word);
    if (nullptr == p)
    {
        std::cout << "add_word() --> lookup() failed for word: \"" << word << "\"" << std::endl;
        abort();
    }

    matchable::MatchableMaker & mm = p->as_mutable_maker();

    std::string const matchable_name = "word_" + p->escaped_and_delimited('_');

    // create new variant
    std::string const escaped = "esc_" + matchable::escapable::escape_all(word);
    mm.grab(matchable_name)->add_variant(escaped);

    // property for parts of speech
    std::vector<std::string> property_values;
    for (auto p : parts_of_speech::variants_by_string())
    {
        if (pos_flags.is_set(p))
            property_values.push_back("1");
        else
            property_values.push_back("0");
    }
    mm.grab(matchable_name)->set_propertyvect(escaped, "pos", property_values);

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
        mm.grab(matchable_name)->set_property(escaped, "ordinal_summation", std::to_string(ordinal_sum));
    }

    // properties from word attributes
    {
        auto set_prop =
            [&](word_attribute::Type att)
            {
                if (wsf.is_set(att))
                {
                    std::string const prop_name = std::string("is_") + att.as_string();
                    mm.grab(matchable_name)->set_property(escaped, prop_name, "1");
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
