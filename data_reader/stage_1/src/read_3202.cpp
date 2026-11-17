#include "read_3202.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



bool read_3202(SerialTask::Type task)
{
    std::string const FN_3202{Stage1Data::nil.as_data_dir() + "/3202/files/mthesaur.txt"};
    FILE * f = fopen(FN_3202.c_str(), "r");
    if (nullptr == f)
    {
        perror(FN_3202.c_str());
        return false;
    }

    std::filesystem::path p{FN_3202.c_str()};
    task.set_goal(std::filesystem::file_size(p));
    task.set_progress(0);

    auto const is_key_word =
        [](int term)
        {
            return std::binary_search(Stage1Data::nil.as_mutable_key_words().begin(),
                                      Stage1Data::nil.as_mutable_key_words().end(),
                                      term);
        };

    // each line is a key followed by its comma separated synonyms; a line whose key is unknown or a key
    // word is skipped whole, and the last line needs no line end
    std::string line;
    bool found = false;
    int ch = 0;
    while (ch != EOF)
    {
        line.clear();
        while (true)
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == EOF || ch == '\n' || ch == '\r')
                break;
            line += (char) ch;
        }

        size_t comma = line.find(',');
        if (comma == std::string::npos)
            continue;
        std::string const key = line.substr(0, comma);
        int const key_term = mm_lookup(key.c_str(), &found);
        if (!found || is_key_word(key_term))
            continue;

        auto & syns = Stage1Data::nil.as_mutable_syn_ant_table()[key].syn;
        for (size_t start = comma + 1; start <= line.size(); start = comma + 1)
        {
            comma = std::min(line.find(',', start), line.size());
            std::string const syn = line.substr(start, comma - start);
            if (syn.empty())
                continue;
            int const term = mm_lookup(syn.c_str(), &found);
            if (found && !is_key_word(term) && std::find(syns.begin(), syns.end(), syn) == syns.end())
                syns.push_back(syn);
        }

        SerialTask::check_progress(task);
    }

    fclose(f);
    return true;
}
