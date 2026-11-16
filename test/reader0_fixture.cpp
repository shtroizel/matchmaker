// reader0_fixture <data dir> <parent prefix file> <none|reduced|all> <work dir> <task>...
// Runs the named stage 0 reader tasks ("reading 3202", ...) over a data directory, in the order
// data_reader_stage_0 runs them, and prints each word kept, in term order, with its leaf and properties,
// then each book vocabulary written.
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
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
    for (int i = 5; i < argc; ++i)
    {
        auto task = SerialTask::from_string(argv[i]);
        if (task.is_nil())
        {
            std::cerr << "unknown task: " << argv[i] << std::endl;
            return 2;
        }
        wanted.push_back(task);
    }

    // the reader's progress output goes to stderr, leaving stdout for the result
    auto * const out = std::cout.rdbuf(std::cerr.rdbuf());
    SerialTask::reset_progress();
    run(SerialTask::preparing_spc_prefixes::grab());
    for (auto const & task : Stage0Data::reading_order())
        if (std::find(wanted.begin(), wanted.end(), task) != wanted.end())
            run(task);
    std::cout.rdbuf(out);

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
    return 0;
}
