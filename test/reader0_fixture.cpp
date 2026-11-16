// reader0_fixture <data dir> <parent prefix file> <none|reduced|all> <work dir> <task>...
// Runs the named stage 0 reader tasks ("reading 3202", ...) over a data directory, in the order
// data_reader_stage_0 runs them, and prints each word kept, in term order, with its leaf and properties,
// then each book vocabulary written. With the task "saving matchables" it also saves, as the reader does
// last, and checks what was written against the words kept.
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <matchable/matchable.h>
#include <matchable/MatchableMaker.h>

#include <matchmaker/data_reader_common.h>
#include <matchmaker/parts_of_speech.h>

#include <SerialTask.h>
#include <Stage0Data.h>



namespace
{
    void run(SerialTask::Type task)
    {
        int const magic_number = 58;
        task.set_progress_steps(magic_number - task.as_string().length());
        if (!task.as_run())
        {
            std::cerr << "run function is null: " << task << std::endl;
            exit(1);
        }
        task.as_run()(task);
    }


    struct Word
    {
        std::string term;
        std::string leaf;
        std::string line;
    };


    std::string describe(matchable::MatchableVariant const & v)
    {
        std::string pos;
        std::string ordinal_summation;
        std::string attributes;
        for (auto const & [name, value, values] : v.properties)
        {
            if (name == "pos")
            {
                auto const & pos_variants = parts_of_speech::variants_by_string();
                for (size_t i = 0; i < values.size() && i < pos_variants.size(); ++i)
                    if (values[i] == "1")
                        pos += pos_variants[i].as_string();
            }
            else if (name == "ordinal_summation")
            {
                ordinal_summation = value;
            }
            else if (name.rfind("is_", 0) == 0 && value == "1")
            {
                attributes += " " + name.substr(3);
            }
            else if (!value.empty() || !values.empty())
            {
                attributes += " " + name + "=" + value;
            }
        }
        return "pos=" + pos + " os=" + ordinal_summation + attributes;
    }


    std::vector<std::string> split_arguments(std::string const & text)
    {
        std::vector<std::string> arguments;
        size_t start = 0;
        while (true)
        {
            size_t const comma = text.find(',', start);
            std::string argument = text.substr(start, comma == std::string::npos ? std::string::npos
                                                                                  : comma - start);
            size_t const first = argument.find_first_not_of(" \n");
            size_t const last = argument.find_last_not_of(" \n");
            arguments.push_back(first == std::string::npos ? "" : argument.substr(first, last - first + 1));
            if (comma == std::string::npos)
                return arguments;
            start = comma + 1;
        }
    }


    // a saved leaf header: the variant names it declares (PROPERTYx19_MATCHABLE, GROW_MATCHABLE) and each
    // property value it sets (MATCHABLE_VARIANT_PROPERTY_VALUE and _VALUES, keyed by variant and property,
    // "[]" marking a vector property); statements may span lines
    struct SavedLeaf
    {
        std::vector<std::string> variants;
        std::map<std::pair<std::string, std::string>, std::string> values;
        std::vector<std::string> problems;
    };


    SavedLeaf read_saved(std::string const & path, std::string const & matchable_name)
    {
        SavedLeaf leaf;
        std::ifstream header{path};
        std::string const text{std::istreambuf_iterator<char>{header}, std::istreambuf_iterator<char>{}};
        size_t start = 0;
        while (start < text.size())
        {
            size_t const line_end = std::min(text.find('\n', start), text.size());
            size_t const open = text.find('(', start);
            size_t next = line_end + 1;
            if (open < line_end)
            {
                std::string const macro = text.substr(start, open - start);
                size_t const close = text.find(')', open);
                std::vector<std::string> const args =
                    split_arguments(text.substr(open + 1, close - open - 1));
                if (macro == "PROPERTYx19_MATCHABLE")
                {
                    auto const name = std::find(args.begin(), args.end(), matchable_name);
                    if (name == args.end())
                        leaf.problems.push_back("PROPERTYx19_MATCHABLE without " + matchable_name);
                    else
                        leaf.variants.insert(leaf.variants.end(), name + 1, args.end());
                }
                else if (macro == "GROW_MATCHABLE" || macro == "MATCHABLE_VARIANT_PROPERTY_VALUE" ||
                         macro == "MATCHABLE_VARIANT_PROPERTY_VALUES")
                {
                    size_t const minimum = macro == "GROW_MATCHABLE" ? 2 : 4;
                    if (args.size() < minimum || args[0] != matchable_name)
                        leaf.problems.push_back(macro + " for another matchable or too short");
                    else if (macro == "GROW_MATCHABLE")
                        leaf.variants.insert(leaf.variants.end(), args.begin() + 1, args.end());
                    else
                    {
                        bool const vector = macro == "MATCHABLE_VARIANT_PROPERTY_VALUES";
                        std::string value;
                        for (size_t i = 3; i < args.size(); ++i)
                            value += (i == 3 ? "" : ",") + args[i];
                        std::pair<std::string, std::string> key{args[1], args[2] + (vector ? "[]" : "")};
                        if (!vector && args.size() != 4)
                            leaf.problems.push_back(macro + " with more than one value");
                        else if (!leaf.values.emplace(key, value).second)
                            leaf.problems.push_back("value set twice: " + args[1] + " " + args[2]);
                    }
                }
                next = std::min(text.find('\n', close), text.size()) + 1;
            }
            start = next;
        }
        return leaf;
    }


    // checks what saving wrote against the words kept: one header per leaf with words, declaring exactly
    // that leaf's words with exactly their property values, no other headers, and a leaf list naming each
    // of those leaves once
    int check_saved(std::filesystem::path const & work)
    {
        int problems = 0;
        size_t leaves = 0;
        size_t words = 0;
        size_t values = 0;
        std::set<std::string> expected_headers;
        std::multiset<std::string> expected_leaves;
        Stage0Data::foreach_prefix(
            [&](Prefix & p)
            {
                std::string const path = p.save_path(Stage0Data::output_dir());
                expected_headers.insert(std::filesystem::path(path).lexically_normal().string());
                expected_leaves.insert(p.escaped_and_delimited(' '));
                std::vector<std::string> kept;
                std::map<std::pair<std::string, std::string>, std::string> kept_values;
                for (auto const & [name, m] : p.as_maker().matchables)
                    for (auto const & v : m->variants)
                    {
                        kept.push_back(v.variant_name);
                        for (auto const & [property, value, values] : v.properties)
                        {
                            if (!value.empty())
                                kept_values[{v.variant_name, property}] = value;
                            if (!values.empty())
                            {
                                std::string joined;
                                for (auto const & element : values)
                                    joined += (joined.empty() ? "" : ",") + element;
                                kept_values[{v.variant_name, property + "[]"}] = joined;
                            }
                        }
                    }
                SavedLeaf saved = read_saved(path, "word_" + p.escaped_and_delimited('_'));
                std::sort(kept.begin(), kept.end());
                std::sort(saved.variants.begin(), saved.variants.end());
                if (kept.empty() || saved.variants != kept)
                    saved.problems.push_back(std::to_string(kept.size()) + " words kept, " +
                                             std::to_string(saved.variants.size()) + " variants saved");
                if (saved.values != kept_values)
                {
                    size_t differ = 0;
                    std::string first;
                    for (auto const & [key, value] : kept_values)
                    {
                        auto const found = saved.values.find(key);
                        if (found == saved.values.end() || found->second != value)
                        {
                            if (differ++ == 0)
                                first = key.first + " " + key.second + " kept " + value + ", saved " +
                                        (found == saved.values.end() ? "nothing" : found->second);
                        }
                    }
                    for (auto const & [key, value] : saved.values)
                        if (kept_values.count(key) == 0 && differ++ == 0)
                            first = key.first + " " + key.second + " saved " + value + ", kept nothing";
                    saved.problems.push_back(std::to_string(differ) + " values differ, first: " + first);
                }
                for (auto const & problem : saved.problems)
                    std::cerr << path << ": " << problem << std::endl;
                problems += saved.problems.size();
                ++leaves;
                words += kept.size();
                values += kept_values.size();
            }
        );

        std::set<std::string> headers;
        for (auto const & entry : std::filesystem::recursive_directory_iterator(work / "out"))
            if (entry.path().extension() == ".h")
                headers.insert(entry.path().lexically_normal().string());
        for (auto const & h : headers)
            if (expected_headers.count(h) == 0)
            {
                std::cerr << "header for no leaf with words: " << h << std::endl;
                ++problems;
            }
        for (auto const & h : expected_headers)
            if (headers.count(h) == 0)
            {
                std::cerr << "no header saved: " << h << std::endl;
                ++problems;
            }

        std::multiset<std::string> listed;
        std::ifstream leaf_list{work / "leaf_list"};
        for (std::string line; std::getline(leaf_list, line);)
            listed.insert(line);
        if (listed != expected_leaves)
        {
            std::cerr << "leaf list: " << listed.size() << " lines, " << expected_leaves.size()
                      << " leaves with words" << std::endl;
            ++problems;
        }

        std::cout << "saved " << leaves << " leaves, " << words << " words, " << values << " values: "
                  << (problems == 0 ? "ok" : "PROBLEMS") << "\n";
        return problems;
    }
}



int main(int argc, char ** argv)
{
    if (argc < 6)
    {
        std::cerr << "usage: " << argv[0]
                  << " <data dir> <parent prefix file> <none|reduced|all> <work dir> <task>..."
                  << std::endl;
        return 2;
    }

    std::filesystem::path const work{argv[4]};
    std::filesystem::remove_all(work);
    std::filesystem::create_directories(work / "book_vocabulary");
    std::filesystem::create_directories(work / "out");

    Stage0Data::data_dir() = argv[1];
    Stage0Data::parents_filename() = argv[2];
    Stage0Data::book_vocab_dir() = (work / "book_vocabulary").string();
    Stage0Data::output_dir() = (work / "out").string();
    Stage0Data::leaves_filename() = (work / "leaf_list").string();
    Stage0Data::dictionary_mode() = dictionary_usage::from_string(argv[3]);
    if (Stage0Data::dictionary_mode().is_nil())
    {
        std::cerr << "unknown dictionary usage: " << argv[3] << std::endl;
        return 2;
    }
    select_books("all");

    std::vector<SerialTask::Type> wanted;
    bool save = false;
    for (int i = 5; i < argc; ++i)
    {
        auto task = SerialTask::from_string(argv[i]);
        if (task.is_nil())
        {
            std::cerr << "unknown task: " << argv[i] << std::endl;
            return 2;
        }
        if (task == SerialTask::saving_spc_matchables::grab())
            save = true;
        else
            wanted.push_back(task);
    }

    // the reader's progress output goes to stderr, leaving stdout for the result
    auto * const out = std::cout.rdbuf(std::cerr.rdbuf());
    SerialTask::reset_progress();
    run(SerialTask::preparing_spc_prefixes::grab());
    for (auto const & task : Stage0Data::reading_order())
        if (std::find(wanted.begin(), wanted.end(), task) != wanted.end())
            run(task);
    if (save)
        run(SerialTask::saving_spc_matchables::grab());
    std::cout.rdbuf(out);

    int const save_problems = save ? check_saved(work) : 0;

    std::vector<Word> words;
    Stage0Data::foreach_prefix(
        [&words](Prefix & p)
        {
            for (auto const & [name, m] : p.as_maker().matchables)
                for (auto const & v : m->variants)
                {
                    std::string const term = matchable::escapable::unescape_all(v.variant_name);
                    words.push_back({term, p.escaped_and_delimited(' '), describe(v)});
                }
        }
    );
    std::sort(words.begin(), words.end(),
              [](Word const & l, Word const & r) { return matchable::str_lt_str(l.term, r.term); });
    for (auto const & w : words)
        std::cout << "[" << w.term << "] " << w.leaf << " | " << w.line << "\n";

    for (auto const & book : Buch::variants_by_index())
    {
        std::ifstream vocabulary{work / "book_vocabulary" / book.as_string() / "vocabulary"};
        if (!vocabulary)
            continue;
        std::cout << "vocabulary " << book << ":\n";
        for (std::string line; std::getline(vocabulary, line);)
            std::cout << "    [" << line << "]\n";
    }
    return save_problems == 0 ? 0 : 1;
}
