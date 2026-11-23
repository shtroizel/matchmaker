#include <save_matchables.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

#include <matchmaker/word_order.h>

#include <leaf_cuts.h>
#include <Prefix.h>
#include <SerialTask.h>
#include <Stage0Data.h>



void save_matchables(SerialTask::Type task)
{
    // before saving sneak in some important words...
    {
        std::vector<std::string> contributed_by_shtroizel
        {
            "shtroizel",
            "Shtroizel",
        };
        Stage0Data::word_attribute::Flags word_attributes{Stage0Data::word_attribute::name::grab()};
        parts_of_speech::Flags pos{parts_of_speech::N::grab()};

        for (auto const & s : contributed_by_shtroizel)
            Stage0Data::add_word(s, word_attributes, task, pos);
    }

    // every word kept, in term order; the reader's buckets all declare the same properties
    std::vector<std::pair<std::string, matchable::MatchableVariant *>> words;
    std::vector<std::pair<std::string, std::string>> property_types_and_names;
    Stage0Data::foreach_prefix(
        [&](Prefix & p)
        {
            for (auto & [name, m] : p.as_mutable_maker().matchables)
            {
                if (property_types_and_names.empty())
                    property_types_and_names = m->property_types_and_names;
                for (auto & v : m->variants)
                    words.emplace_back(matchable::escapable::unescape_all(v.variant_name), &v);
            }
        }
    );
    std::sort(words.begin(), words.end(),
              [](auto const & l, auto const & r) { return mm_word_less(l.first, r.first); });
    std::vector<std::string> terms;
    terms.reserve(words.size());
    for (auto const & w : words)
        terms.push_back(w.first);
    std::vector<size_t> const starts = leaf_cuts::starts(terms, leaf_cuts::rule(Stage0Data::leaf_budget()));

    task.set_progress(0);
    task.set_goal(starts.size());

    // one header per leaf, and the leaf list for generate_code.py: "<name> <first word>" per leaf
    std::ofstream leaves_file{Stage0Data::leaves_filename(), std::ios::binary};
    std::set<std::string> names;
    for (size_t l = 0; l < starts.size(); ++l)
    {
        ++task.as_mutable_progress();
        size_t const end = l + 1 < starts.size() ? starts[l + 1] : words.size();
        std::string const name = leaf_cuts::name(terms[starts[l]]);
        if (!names.insert(name).second)
        {
            std::cout << "two leaves are named " << name << ", the second starting at '" << terms[starts[l]]
                      << "'" << std::endl;
            abort();
        }

        matchable::MatchableMaker mm;
        mm.includes.push_back("matchmaker/word_order.h");
        matchable::Matchable * m = mm.grab("word_" + name);
        m->property_types_and_names = property_types_and_names;
        m->sortkey = "mm_word_key";
        for (size_t i = starts[l]; i < end; ++i)
            m->variants.push_back(*words[i].second);

        std::string const path = Stage0Data::output_dir() + "/" + name + "/" + name + ".h";
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        auto sa_status = mm.save_as(
            path,
            {matchable::save__content::matchables::grab()},
            matchable::save__grow_mode::wrap::grab()
        );
        if (sa_status != matchable::save__status::success::grab())
        {
            std::cout << "failed to save leaf: '" << name << "'\n"
                      << "           to file: '" << path << "'\n"
                      << " with save__status: '" << sa_status << "'" << std::endl;
            abort();
        }
        leaves_file << name << ' ' << terms[starts[l]] << '\n';
        SerialTask::check_progress(task);
    }
    if (!leaves_file)
    {
        std::cout << "failed to write: '" << Stage0Data::leaves_filename() << "'" << std::endl;
        abort();
    }
}
