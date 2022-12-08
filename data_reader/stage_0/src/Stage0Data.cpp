#include <Stage0Data.h>

#include <iostream>



namespace Stage0Data
{
    std::string & data_dir() { static std::string s; return s; }
    std::string & book_vocab_dir() { static std::string s; return s; }
    std::string & output_dir() { static std::string s; return s; }
    std::string & prefixes_filename() { static std::string s; return s; }

    std::array<Prefix, 23> & symbols_1d_prefixes()
    {
        static std::array<Prefix, 23> prefixes;
        return prefixes;
    }

    std::vector<Prefix> & prefixes_2d_to_5d()
    {
        static std::vector<Prefix> prefixes;
        return prefixes;
    }

    std::vector<Prefix> & prefixes_6d()
    {
        static std::vector<Prefix> prefixes;
        return prefixes;
    }

    LookupTable & lookup_table_2d_to_5d()
    {
        static LookupTable t =
                []()
                {
                    LookupTable t;
                    for (auto & d1 : t)
                        for (auto & d2 : d1)
                            for (auto & d3 : d2)
                                for (auto & d4 : d3)
                                    for (int16_t & i : d4)
                                        i = -1;
                    return t;
                }();
        return t;
    }










    void add_book_vocab(
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
                abort();
            }

            add_word(word, wsf);
        }
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
        if (word.empty())
            return;

        if (wsf.is_set(word_attribute::invisible_ascii::grab()))
        {
            // std::cout << "skipping '" << word << "' -->";
            // for (auto ch : word)
            //     std::cout << " " << (int) ch;
            // std::cout << std::endl;
            return;
        }

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


    void update_word_attribute(word_attribute::Flags & flags, int ch)
    {
        if (ch < 32 || ch > 126)
        {
            flags.set(word_attribute::invisible_ascii::grab());
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
                {
                    flags.set(word_attribute::matchable_symbols::grab());
                }
                else
                {
                    std::cout << "unmatchable symbol: " << std::string(1, (char) ch) << std::endl;
                    flags.set(word_attribute::unmatchable_symbols::grab());
                }
            }
        }
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
            return &symbols_1d_prefixes()[(int16_t) (sym - '-' + 6)];

        // first 3
        if (sym >= '"' && sym <= '$')
            return &symbols_1d_prefixes()[(int16_t) (sym - '"')];

        // 4th and 5th
        if (sym >= '\'' && sym <= '(')
            return &symbols_1d_prefixes()[(int16_t) (sym - '\'' + 3)];

        // 6th
        if (sym == '+')
            return &symbols_1d_prefixes()[5];

        // 21st
        if (sym == '>')
            return &symbols_1d_prefixes()[20];

        // 22nd
        if (sym == '_')
            return &symbols_1d_prefixes()[21];

        // 23rd
        if (sym == '~')
            return &symbols_1d_prefixes()[22];

        return nullptr;
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
                int16_t l = lookup_table_2d_to_5d()[0][0][0][0][0];
                if (l < 0 || l >= (int16_t) prefixes_2d_to_5d().size())
                {
                    std::cout << "lookup() failed given string: \"" << str << "\"" << std::endl;
                    abort();
                }
                return &prefixes_2d_to_5d()[l];
            }
        }

        // first six chars as a string with short strings filled with 'A'
        std::string first_six{"AAAAAA"};
        for
        (
            int16_t str_i = 0;
            str_i < (int16_t) first_six.size() && str_i < (int16_t) str.size() &&
                    ((str[str_i] >= 'A' && str[str_i] <= 'Z') || (str[str_i] >= 'a' && str[str_i] <= 'z'));
            ++str_i
        )
            first_six[str_i] = str[str_i];

        // check for 6D prefix
        auto it = std::lower_bound(
                        prefixes_6d().begin(),
                        prefixes_6d().end(),
                        first_six,
                        [](Prefix const & l, std::string const & r) { return l.as_string() < r; }
                    );
        if (it != prefixes_6d().end() && first_six == it->as_string())
            return &*it;

        // 6D prefix not found so look for a shorter prefix,
        // but first we need the index for each dimension
        std::array<int16_t, 5> i = { 0, 0, 0, 0, 0 };
        for (int16_t str_i = 0; str_i < (int16_t) str.length() && str_i < 5; ++str_i)
        {
            i[str_i] = calc_letter_index(str[str_i]);
        }

        int16_t l = lookup_table_2d_to_5d()[i[0]][i[1]][i[2]][i[3]][i[4]];

        if (l == -1)
        {
            std::cout << "lookup() --> -1 for given string: \"" << str << "\"" << std::endl;
            abort();
        }

        if (l < 0)
            goto err;
        if (l >= (int16_t) prefixes_2d_to_5d().size())
            goto err;

        return &prefixes_2d_to_5d()[l];

    err:
        std::cout << "lookup() failed given string: \"" << str << "\"" << std::endl;
        abort();

        return nullptr;
    }



    void foreach_prefix(std::function<void (Prefix & p)> func)
    {
        for (auto & p : symbols_1d_prefixes())
            func(p);

        for (auto & p : prefixes_2d_to_5d())
            func(p);

        for (auto & p : prefixes_6d())
            func(p);
    }
}
