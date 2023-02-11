#include "patch_matchables.h"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <queue>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <matchable/MatchableMaker.h>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



struct HeaderEntry
{
    std::filesystem::directory_entry entry;
    friend bool operator<(HeaderEntry const & l, HeaderEntry const & r);
};
inline bool operator<(HeaderEntry const & l, HeaderEntry const & r)
{
    return l.entry.file_size() < r.entry.file_size();
}


bool patch_matchable_header(
    SerialTask::Type task,
    std::vector<HeaderEntry> const & matchable_headers
);


std::mutex patch_matchable_header_mutex;


bool patch_matchables(SerialTask::Type task)
{
    std::string const STAGE_1_MATCHABLES_DIR{
        Stage1Data::nil.as_workspace_dir() + "/generated_include/matchmaker/generated_matchables"
    };

    // sort headers by file size (largest files first)
    std::priority_queue<
        HeaderEntry,
        std::vector<HeaderEntry>,
        std::less<std::vector<HeaderEntry>::value_type>
    > header_entries;

    for (auto const & entry : std::filesystem::recursive_directory_iterator(STAGE_1_MATCHABLES_DIR))
        if (entry.is_regular_file())
            header_entries.push({entry});

    task.set_goal(header_entries.size());
    task.set_progress(0);

    // create vector of header entries for each thread
    int const CPU_COUNT = std::thread::hardware_concurrency();
    std::vector<std::vector<HeaderEntry>> dealt_header_entries;
    dealt_header_entries.reserve(CPU_COUNT);
    for (int i = 0; i < CPU_COUNT; ++i)
        dealt_header_entries.push_back(std::vector<HeaderEntry>());

    // deal out headers to each thread
    int i = 0;
    while (!header_entries.empty())
    {
        dealt_header_entries[i].push_back(header_entries.top());
        header_entries.pop();
        ++i;
        if (i >= CPU_COUNT)
            i = 0;
    }

    // los gehts
    std::vector<std::thread> threads;
    bool all_ok{true};
    for (auto const & deal : dealt_header_entries)
        threads.emplace_back(
            std::thread(
                [&]()
                {
                    if (all_ok && !patch_matchable_header(task, deal))
                        all_ok = false;
                }
            )
        );
    for (auto & t : threads)
        t.join();

    return all_ok;
}



bool patch_matchable_header(
    SerialTask::Type task,
    std::vector<HeaderEntry> const & matchable_headers
)
{
    // aliases
    SynAntTable const & contents_SynAnt = Stage1Data::nil.as_syn_ant_table();
    std::vector<int> const & by_longest = Stage1Data::nil.as_by_longest();
    std::map<int, std::pair<int, int>> const & longest_offsets = Stage1Data::nil.as_longest_offsets();
    IndexTable const & embedded_words = Stage1Data::nil.as_embedded_words();
    IndexTable const & defs = Stage1Data::nil.as_definitions();
    IndexTable const & book_indexes = Stage1Data::nil.as_book_indexes();
    IndexTable const & chapter_indexes = Stage1Data::nil.as_chapter_indexes();
    IndexTable const & paragraph_indexes = Stage1Data::nil.as_paragraph_indexes();
    IndexTable const & word_indexes = Stage1Data::nil.as_word_indexes();
    UsedInBookTable const & used_in_book = Stage1Data::nil.as_used_in_book();

    for (auto const & matchable_header : matchable_headers)
    {
        matchable::MatchableMaker mm;
        matchable::load__status::Type load_status = mm.load(matchable_header.entry.path());
        if (load_status != matchable::load__status::success::grab())
        {
            std::cout << "attempt to load " << matchable_header.entry << " failed with: "
                    << load_status << std::endl;
            return false;
        }

        std::string existing_word;
        bool word_in_dictionary{false};
        bool found_word_matchable{false};
        int index{-1};

        for (auto const & [str, m] : mm.matchables)
        {
            if (str.rfind("word", 0) != std::string::npos)
            {
                std::vector<std::string> syn_vect;
                std::vector<std::string> ant_vect;
                for (auto const & v : m->variants)
                {
                    syn_vect.clear();
                    ant_vect.clear();

                    existing_word = matchable::escapable::unescape_all(v.variant_name);

                    // gather synonyms and antonynms
                    auto contents_SynAnt_iter = contents_SynAnt.find(existing_word);
                    if (contents_SynAnt_iter != contents_SynAnt.end())
                    {
                        for (auto const & s : contents_SynAnt_iter->second.syn)
                        {
                            index = mm_lookup(s.c_str(), &word_in_dictionary);
                            if (word_in_dictionary)
                                syn_vect.push_back(std::to_string(index));
                        }
                        for (auto const & a : contents_SynAnt_iter->second.ant)
                        {
                            index = mm_lookup(a.c_str(), &word_in_dictionary);
                            if (word_in_dictionary)
                                ant_vect.push_back(std::to_string(index));
                        }
                    }
                    m->set_propertyvect(v.variant_name, "syn", syn_vect);
                    m->set_propertyvect(v.variant_name, "ant", ant_vect);

                    {
                        std::pair<int, int> const & window = longest_offsets.at(existing_word.size());

                        int lower = window.first;
                        int upper = window.second + 1;
                        int mid{0};

                        while (lower < upper)
                        {
                            mid = lower + (upper - lower) / 2;

                            if (existing_word == std::string(mm_at(by_longest[mid], nullptr)))
                            {
                                m->set_property(v.variant_name, "by_longest_index", std::to_string(mid));
                                break;
                            }

                            if (existing_word < std::string(mm_at(by_longest[mid], nullptr)))
                                upper = mid;
                            else
                                lower = mid;
                        }
                    }

                    bool found = false;
                    int const existing_word_index = mm_lookup(existing_word.c_str(), &found);
                    if (!found)
                    {
                        std::cout << "existing_word (" << existing_word
                                    << ") not found within dictionary!" << std::endl;
                        return false;
                    }

                    {
                        std::vector<std::string> embedded_word_vect;
                        for (int i : embedded_words[existing_word_index])
                            embedded_word_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "embedded", embedded_word_vect);
                    }

                    {
                        std::vector<std::string> def_vect;
                        for (int i : defs[existing_word_index])
                            def_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "definition", def_vect);
                    }

                    {
                        std::vector<std::string> book_index_vect;
                        for (int i : book_indexes[existing_word_index])
                            book_index_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "book_index", book_index_vect);
                    }

                    {
                        std::vector<std::string> chapter_index_vect;
                        for (int i : chapter_indexes[existing_word_index])
                            chapter_index_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "chapter_index", chapter_index_vect);
                    }

                    {
                        std::vector<std::string> paragraph_index_vect;
                        for (int i : paragraph_indexes[existing_word_index])
                            paragraph_index_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "paragraph_index", paragraph_index_vect);
                    }

                    {
                        std::vector<std::string> word_index_vect;
                        for (int i : word_indexes[existing_word_index])
                            word_index_vect.push_back(std::to_string(i));
                        m->set_propertyvect(v.variant_name, "word_index", word_index_vect);
                    }

                    {
                        bool in_at_least_one_book{false};
                        std::vector<std::string> used_in_book_vect;
                        for (auto const & book : Buch::variants_by_index())
                        {
                            if (used_in_book[existing_word_index].is_set(book))
                            {
                                in_at_least_one_book = true;
                                used_in_book_vect.push_back("1");
                            }
                            else
                            {
                                used_in_book_vect.push_back("0");
                            }
                        }
                        if (in_at_least_one_book)
                            m->set_propertyvect(v.variant_name, "is_used_in_book", used_in_book_vect);
                    }
                }

                found_word_matchable = true;
                break;
            }
        }

        std::lock_guard<std::mutex> const lock(patch_matchable_header_mutex);

        if (!found_word_matchable)
        {
            std::cout << "failed to find \"word\" matchable" << std::endl;
            return false;
        }

        auto sa_status = mm.save_as(
            matchable_header.entry.path(),
            {matchable::save__content::matchables::grab()},
            matchable::save__grow_mode::wrap::grab()
        );

        ++task.as_mutable_progress();
        SerialTask::check_progress(task);

        if (sa_status != matchable::save__status::success::grab())
            return false;

    }

    return true;
}
