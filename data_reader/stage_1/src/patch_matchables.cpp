#include "patch_matchables.h"

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <set>
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
#include "write_array.h"



struct HeaderEntry
{
    std::filesystem::directory_entry entry;
    friend bool operator<(HeaderEntry const & l, HeaderEntry const & r);
};
inline bool operator<(HeaderEntry const & l, HeaderEntry const & r)
{
    return l.entry.file_size() < r.entry.file_size();
}


// the leaves in term order: each one's name and the alphabetic index of its first term
struct Leaves
{
    std::vector<std::string> names;
    std::vector<int> firsts;
};


// the matchable and the variant a leaf holds a word as
struct Word
{
    std::string type;
    std::string variant;
};


// each term's synonyms and antonyms as alphabetic indexes
struct Links
{
    IndexTable synonyms;
    IndexTable antonyms;
};


bool patch_matchable_header(
    SerialTask::Type task,
    std::vector<HeaderEntry> const & matchable_headers,
    Leaves const & leaves,
    Links & links
);


std::mutex patch_matchable_header_mutex;


static bool read_leaves(Leaves & leaves)
{
    std::string const path{Stage1Data::nil.as_workspace_dir() + "/leaf_list"};
    std::ifstream f{path, std::ios::binary};
    for (std::string line; std::getline(f, line);)
    {
        size_t const space = line.find(' ');
        bool found{false};
        int const first = space == std::string::npos ? -1 : mm_lookup(line.c_str() + space + 1, &found);
        bool const in_order = leaves.firsts.empty() ? first == 0 : first > leaves.firsts.back();
        if (!found || !in_order)
        {
            std::cout << "'" << line << "' in " << path << " does not name the next leaf and its first term"
                      << std::endl;
            return false;
        }
        leaves.names.push_back(line.substr(0, space));
        leaves.firsts.push_back(first);
    }
    if (leaves.names.empty())
    {
        std::cout << "no leaves in " << path << std::endl;
        return false;
    }
    return true;
}


static Word word_at(Leaves const & leaves, int index)
{
    size_t const leaf =
        std::upper_bound(leaves.firsts.begin(), leaves.firsts.end(), index) - leaves.firsts.begin() - 1;
    int length{0};
    char const * const term = mm_at(index, &length);
    return {
        "word_" + leaves.names[leaf],
        "esc_" + matchable::escapable::escape_all(std::string(term, length))
    };
}


// the references a leaf holds for the terms at indexes; the words of other leaves among them are added to
// extern_variants
static std::vector<std::string> references(
    Leaves const & leaves,
    std::vector<int> const & indexes,
    std::string const & own_type,
    std::map<std::string, std::set<std::string>> & extern_variants
)
{
    std::vector<std::string> refs;
    for (int index : indexes)
    {
        Word const word = word_at(leaves, index);
        if (word.type != own_type)
            extern_variants[word.type].insert(word.variant);
        refs.push_back("::matchable::variant_ref<" + word.type + "::" + word.variant + ">()");
    }
    return refs;
}


static bool write_relation(std::string const & name, IndexTable const & lists)
{
    std::vector<int> term_ints;
    term_ints.reserve(lists.size());
    for (auto const & list : lists)
        term_ints.push_back((int) list.size());
    return write_term_blocks(
        name, name + "_shift", term_ints,
        [&](int term, std::vector<int> & block)
            { block.insert(block.end(), lists[term].begin(), lists[term].end()); }
    );
}


bool patch_matchables(SerialTask::Type task)
{
    std::string const STAGE_1_MATCHABLES_DIR{
        Stage1Data::nil.as_workspace_dir() + "/generated_include/matchmaker/generated_matchables"
    };

    Leaves leaves;
    if (!read_leaves(leaves))
        return false;

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
    int const CPU_COUNT = Stage1Data::nil.as_thread_count();
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

    // each thread fills the entries of the terms in its own headers
    Links links{IndexTable(mm_count()), IndexTable(mm_count())};

    // los gehts
    std::vector<std::thread> threads;
    std::atomic<bool> all_ok{true};
    for (auto const & deal : dealt_header_entries)
        threads.emplace_back(
            std::thread(
                [&]()
                {
                    if (all_ok && !patch_matchable_header(task, deal, leaves, links))
                        all_ok = false;
                }
            )
        );
    for (auto & t : threads)
        t.join();

    if (!all_ok)
        return false;

    if (!Stage1Data::nil.as_generated_relations())
        return true;

    return write_relation("synonyms", links.synonyms) &&
           write_relation("antonyms", links.antonyms) &&
           write_relation("embedded", Stage1Data::nil.as_embedded_words()) &&
           write_relation("definitions", Stage1Data::nil.as_definitions());
}



bool patch_matchable_header(
    SerialTask::Type task,
    std::vector<HeaderEntry> const & matchable_headers,
    Leaves const & leaves,
    Links & links
)
{
    // aliases
    SynAntTable const & contents_SynAnt = Stage1Data::nil.as_syn_ant_table();
    IndexTable const & embedded_words = Stage1Data::nil.as_embedded_words();
    IndexTable const & defs = Stage1Data::nil.as_definitions();

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
        mm.extern_variants.clear();

        bool found_word_matchable{false};

        for (auto const & [type, m] : mm.matchables)
        {
            if (type.rfind("word", 0) == std::string::npos)
                continue;

            for (auto const & v : m->variants)
            {
                std::string const existing_word = matchable::escapable::unescape_all(v.variant_name);

                bool found = false;
                int const existing_word_index = mm_lookup(existing_word.c_str(), &found);
                if (!found)
                {
                    std::cout << "existing_word (" << existing_word
                                << ") not found within dictionary!" << std::endl;
                    return false;
                }

                Word const own = word_at(leaves, existing_word_index);
                if (own.type != type || own.variant != v.variant_name)
                {
                    std::cout << "the leaf list has '" << existing_word << "' as " << own.type << "::"
                              << own.variant << ", and " << matchable_header.entry << " as " << type
                              << "::" << v.variant_name << std::endl;
                    return false;
                }

                // gather synonyms and antonynms
                std::vector<int> & syn = links.synonyms[existing_word_index];
                std::vector<int> & ant = links.antonyms[existing_word_index];
                auto contents_SynAnt_iter = contents_SynAnt.find(existing_word);
                if (contents_SynAnt_iter != contents_SynAnt.end())
                {
                    bool word_in_dictionary{false};
                    for (auto const & s : contents_SynAnt_iter->second.syn)
                    {
                        int const index = mm_lookup(s.c_str(), &word_in_dictionary);
                        if (word_in_dictionary)
                            syn.push_back(index);
                    }
                    for (auto const & a : contents_SynAnt_iter->second.ant)
                    {
                        int const index = mm_lookup(a.c_str(), &word_in_dictionary);
                        if (word_in_dictionary)
                            ant.push_back(index);
                    }
                }

                auto const refs =
                    [&](std::vector<int> const & indexes)
                        { return references(leaves, indexes, type, mm.extern_variants); };
                m->set_propertyvect(v.variant_name, "syn", refs(syn));
                m->set_propertyvect(v.variant_name, "ant", refs(ant));
                m->set_propertyvect(v.variant_name, "embedded", refs(embedded_words[existing_word_index]));
                m->set_propertyvect(v.variant_name, "definition", refs(defs[existing_word_index]));
            }

            found_word_matchable = true;
            break;
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
