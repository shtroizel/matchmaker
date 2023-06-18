#include <Stage0Data.h>

#include <iostream>

#include <SerialTask.h>



namespace Stage0Data
{
    std::string & data_dir() { static std::string s; return s; }
    std::string & book_vocab_dir() { static std::string s; return s; }
    std::string & output_dir() { static std::string s; return s; }
    std::string & prefixes_filename() { static std::string s; return s; }

    q_usage::Type & q_mode() { static q_usage::Type mode; return mode; }

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
                                    for (index_t & i : d4)
                                        i = -1;
                    return t;
                }();
        return t;
    }










    void add_book_vocab(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task,
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

            add_word(word, wsf, task);
        }
    }



    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task
    )
    {
        static parts_of_speech::Flags const empty_pos_flags;
        add_word(word, wsf, task, empty_pos_flags);
    }



    void add_word(
        std::string const & word,
        word_attribute::Flags const & wsf,
        SerialTask::Type task,
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

        // new variant
        std::string const escaped = "esc_" + matchable::escapable::escape_all(word);

        // verify matchable escaped correctly
        if (matchable::escapable::unescape_all(escaped) != word)
        {
            std::cout << "matchable::escapable::escape_all() FAILED!" << std::endl;
            std::cout << "  escaped: " << escaped << std::endl;
            std::cout << "unescaped: " << matchable::escapable::unescape_all(escaped) << std::endl;
            std::cout << "     term: " << word << std::endl;
            abort();
        }

        if (
            MATCHABLE_INSTANCE_IN(SerialTask, task, reading_spc_Crumbs, reading_spc_Keys)
            || q_mode() != q_usage::only::grab()
        )
            mm.grab(matchable_name)->add_variant(escaped);

        if (!mm.grab(matchable_name)->has_variant(escaped))
            return;

        // property for parts of speech
        std::vector<std::string> property_values;
        mm.grab(matchable_name)->get_propertyvect(escaped, "pos", property_values);
        if (property_values.empty())
        {
            for (auto p : parts_of_speech::variants_by_string())
            {
                if (pos_flags.is_set(p))
                    property_values.push_back("1");
                else
                    property_values.push_back("0");
            }
            mm.grab(matchable_name)->set_propertyvect(escaped, "pos", property_values);
        }

        // property for ordinal sum
        {
            std::string ordinal_sum_as_str;
            mm.grab(matchable_name)->get_property(escaped, "ordinal_summation", ordinal_sum_as_str);
            if (ordinal_sum_as_str.empty())
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
                ordinal_sum_as_str = std::to_string(ordinal_sum);
                mm.grab(matchable_name)->set_property(escaped, "ordinal_summation", ordinal_sum_as_str);
            }
        }

        // properties from word attributes
        {
            auto set_prop =
                [&](word_attribute::Type att)
                {
                    std::string const prop_name = std::string("is_") + att.as_string();
                    std::string prop_value;
                    mm.grab(matchable_name)->get_property(escaped, prop_name, prop_value);
                    if (prop_value.empty() && wsf.is_set(att))
                        mm.grab(matchable_name)->set_property(escaped, prop_name, "1");
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


    index_t calc_index(char ch)
    {
        //   0  ' '
        //   1  '!'
        //   2  '"'
        //   3  '#'
        //   4  '$'
        //   5  '%'
        //   6  '&'
        //   7  '''
        //   8  '('
        //   9  ')'
        //  10  '*'
        //  11  '+'
        //  12  ','
        //  13  '-'
        //  14  '.'
        //  15  '/'
        //  16  '0'
        //  17  '1'
        //  18  '2'
        //  19  '3'
        //  20  '4'
        //  21  '5'
        //  22  '6'
        //  23  '7'
        //  24  '8'
        //  25  '9'
        //  26  ':'
        //  27  ';'
        //  28  '<'
        //  29  '='
        //  30  '>'
        //  31  '?'
        //  32  '@'
        //  33  '['
        //  34  '\'
        //  35  ']'
        //  36  '^'
        //  37  '_'
        //  38  '`'
        //  39  '{'
        //  40  '|'
        //  41  '}'
        //  42  '~'
        //  43 - 95 --> 'A' - 'z'

        if (ch >= ' ' && ch <= '@')
            return (int) (ch - ' ');
        else if (ch >= 'A' && ch <= 'Z')
            return (int) (ch - 'A' + 43);
        else if (ch >= '[' && ch <= '`')
            return (int) (ch - '[' + 33);
        else if (ch >= 'a' && ch <= 'z')
            return (int) (ch - 'a' + 69);
        else if (ch >= '{' && ch <= '~')
            return (int) (ch - '{' + 39);

        std::cout << "calc_index(" << ch << ") --> unhandled symbol!";
        abort();
        return -1;
    }



    Prefix * lookup(std::string const & str)
    {
        if (str.empty())
        {
            std::cout << "lookup() called with empty string!" << std::endl;
            abort();
        }

        // first six chars as a string with short strings filled with first symbol (space)
        std::string first_six = "      ";
        for (size_t str_i = 0; str_i < first_six.size() && str_i < str.size(); ++str_i)
            first_six[str_i] = str[str_i];

        // check for 6D prefix
        auto it = std::lower_bound(
                        prefixes_6d().begin(),
                        prefixes_6d().end(),
                        first_six,
                        [](Prefix const & l, std::string const & r)
                        {
                            return matchable::str_lt_str(l.as_string(), r);
                        }
                    );
        if (it != prefixes_6d().end() && first_six == it->as_string())
            return &*it;

        // 6D prefix not found so look for a shorter prefix,
        // but first we need the index for each dimension
        std::array<size_t, 5> i = { 0, 0, 0, 0, 0 };
        for (size_t str_i = 0; str_i < str.length() && str_i < 5; ++str_i)
            i[str_i] = calc_index(str[str_i]);

        int l = lookup_table_2d_to_5d()[i[0]][i[1]][i[2]][i[3]][i[4]];

        if (l == -1)
        {
            std::cout << "lookup() --> "
                      << "[" << i[0]
                      << "][" << i[1]
                      << "][" << i[2]
                      << "][" << i[3]
                      << "][" << i[4]
                      << "] --> -1 for given string: \"" << str << "\"" << std::endl;
            std::cout << "first six: '" << first_six << "'" << std::endl;
            std::cout << "6D prefixes:" << std::endl;
            for (auto const & p : prefixes_6d())
                std::cout << "    --> " << p.as_string() << std::endl;
            abort();
        }

        if (l < 0)
            goto err;
        if (l >= (int) prefixes_2d_to_5d().size())
            goto err;

        return &prefixes_2d_to_5d()[l];

    err:
        std::cout << "lookup() failed given string: \"" << str << "\"" << std::endl;
        std::cout << "    --> l: " << l << std::endl;
        std::cout << "    --> prefixes_2d_to_5d().size(): " << prefixes_2d_to_5d().size() << std::endl;
        abort();

        return nullptr;
    }



    void foreach_prefix(std::function<void (Prefix & p)> func)
    {
        for (auto & p : prefixes_2d_to_5d())
            func(p);

        for (auto & p : prefixes_6d())
            func(p);
    }
}
