#include <Stage0Data.h>

#include <iostream>

#include <SerialTask.h>
#include <routing.h>



namespace Stage0Data
{
    std::string & data_dir() { static std::string s; return s; }
    std::string & book_vocab_dir() { static std::string s; return s; }
    std::string & output_dir() { static std::string s; return s; }
    std::string & parents_filename() { static std::string s; return s; }
    std::string & leaves_filename() { static std::string s; return s; }

    dictionary_usage::Type & dictionary_mode() { static dictionary_usage::Type mode; return mode; }

    bool adds_words(SerialTask::Type task)
    {
        return !SerialTask::book_of(task).is_nil()
            || dictionary_mode() == dictionary_usage::all::grab()
            || (
                dictionary_mode() == dictionary_usage::reduced::grab()
                && !MATCHABLE_INSTANCE_IN(
                    SerialTask,
                    task,
                    reading_spc_3201_spc__parl_SINGLE_parr_,
                    reading_spc_3201_spc__parl_COMPOUND_parr_
                )
            );
    }

    std::vector<SerialTask::Type> reading_order()
    {
        std::vector<SerialTask::Type> adding;
        std::vector<SerialTask::Type> annotating;
        for (auto const & task : SerialTask::variants_by_index())
        {
            if (MATCHABLE_INSTANCE_IN(SerialTask, task, preparing_spc_prefixes, saving_spc_matchables))
                continue;
            if (!SerialTask::book_of(task).is_nil() && !selected_books().is_set(SerialTask::book_of(task)))
                continue;
            if (adds_words(task))
                adding.push_back(task);
            else
                annotating.push_back(task);
        }
        adding.insert(adding.end(), annotating.begin(), annotating.end());
        return adding;
    }

    std::set<std::string> & parent_prefixes()
    {
        static std::set<std::string> parents{""};
        return parents;
    }

    std::map<std::string, Prefix, TermLess> & leaves()
    {
        static std::map<std::string, Prefix, TermLess> l;
        return l;
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

        // a source that only annotates has nothing to annotate in a leaf without words
        Prefix * p = lookup(word, adds_words(task));
        if (nullptr == p)
            return;

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

        if (adds_words(task))
            mm.grab(matchable_name)->add_variant(escaped);

        if (!mm.grab(matchable_name)->has_variant(escaped))
            return;

        // property for parts of speech, merged across sources
        {
            std::vector<std::string> property_values;
            mm.grab(matchable_name)->get_propertyvect(escaped, "pos", property_values);
            auto const & pos_variants = parts_of_speech::variants_by_string();
            bool changed = property_values.empty();
            property_values.resize(pos_variants.size(), "0");
            for (size_t i = 0; i < pos_variants.size(); ++i)
            {
                if (pos_flags.is_set(pos_variants[i]) && property_values[i] != "1")
                {
                    property_values[i] = "1";
                    changed = true;
                }
            }
            if (changed)
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


    Prefix * lookup(std::string const & str, bool create)
    {
        if (str.empty())
        {
            std::cout << "lookup() called with empty string!" << std::endl;
            abort();
        }

        std::string const prefix = routing::leaf_of(parent_prefixes(), str);

        auto it = leaves().find(prefix);
        if (it != leaves().end())
            return &it->second;

        if (!create)
            return nullptr;

        Prefix & p = leaves()[prefix];
        p.initialize(prefix);
        return &p;
    }



    void foreach_prefix(std::function<void (Prefix & p)> func)
    {
        for (auto & [str, p] : leaves())
            func(p);
    }
}
