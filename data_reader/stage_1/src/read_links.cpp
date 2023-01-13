#include "read_links.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>



#ifdef MATCHABLE_OMIT_BY_INDEX
    matchable with by_index required for stage 1 reader
#endif
#include <matchable/matchable.h>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include <matchmaker/data_reader_common.h>

#include "Stage1Data.h"




struct LinkEntry
{
    std::filesystem::directory_entry entry;
    friend bool operator<(LinkEntry const & l, LinkEntry const & r);
};
inline bool operator<(LinkEntry const & l, LinkEntry const & r)
{
    return l.entry.file_size() < r.entry.file_size();
}



bool read_link(FILE * f, std::string const & link);
bool read_link_line(FILE * f, std::string & line);



bool read_links(int progress_steps)
{
    std::string const & DATA_DIR = Stage1Data::nil.as_data_dir();

    std::priority_queue<
        LinkEntry,
        std::vector<LinkEntry>,
        std::less<std::vector<LinkEntry>::value_type>
    > links;
    for (auto const & entry : std::filesystem::recursive_directory_iterator(DATA_DIR + "/Crumbs/linked_text"))
        if (entry.is_regular_file() && entry.path().stem() == "post")
            links.push({entry});

    FILE * link_file{nullptr};
    int files_processed{0};
    int printed_progress{0};
    int const file_count = (int) links.size();
    bool ok = false;
    while (!links.empty())
    {
        ++files_processed;
        if (files_processed % (file_count / progress_steps) == 0 && printed_progress < progress_steps)
        {
            ++printed_progress;
            std::cout << "." << std::flush;
        }

        link_file = fopen(links.top().entry.path().c_str(), "r");
        if (link_file == 0)
        {
            perror(links.top().entry.path().c_str());
            exit(1);
        }
        std::string link = ">>";
        link += links.top().entry.path().parent_path().stem();
        ok = read_link(link_file, link);
        fclose(link_file);
        if (!ok)
        {
            std::cout << "failed to read link file: " << links.top().entry.path() << std::endl;
            return false;
        }
        link_file = nullptr;
        links.pop();
    }
    return true;
}


bool read_link(FILE * f, std::string const & link)
{
    IndexTable & def = Stage1Data::nil.as_mutable_definitions();

    std::string line;
    bool term_in_dictionary{false};
    int term{-1};

    bool ok;
    int link_as_term = mm_lookup(link.c_str(), &ok);
    if (!ok)
    {
        // std::cout << "\nmm_lookup() failed given: '" << link << "'" << std::endl;
        // return false;
        return true;
    }

    while (read_link_line(f, line))
    {
        if (line.length() == 0)
            continue;

        // entire line should exist as a term
        term = mm_lookup(line.c_str(), &term_in_dictionary);
        if (!term_in_dictionary)
        {
            // std::cout << "\nterm '" << line
            //             << "' not in the dictionary! link: " << link << std::endl;
            // return false;
            continue;
        }

        def[link_as_term].push_back(term);
    }

    return true;
}


bool read_link_line(FILE * f, std::string & line)
{
    line.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        line += (char) ch;
    }

    return true;
}
