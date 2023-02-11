#include "read_3202.h"

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

    std::string syn;
    std::string key;
    bool found = false;
    int term = -1;

    int ch = 0;
    while (true)
    {
        // read key (up to first ',')
        key.clear();
        while (true)
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == EOF)
                goto end;

            if (ch == ',')
                break;

            key += (char) ch;
        }

        term = mm_lookup(key.c_str(), &found);
        if (found && !std::binary_search(Stage1Data::nil.as_mutable_key_words().begin(),
                                         Stage1Data::nil.as_mutable_key_words().end(),
                                         term))
        {
            // read each comma separated synonym
            do
            {
                // read next synonym
                syn.clear();
                while (true)
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();
                    if (ch == EOF)
                        goto end;

                    if (ch == ',' || ch == 10 || ch == 13)
                        break;

                    syn += (char) ch;
                }

                term = mm_lookup(syn.c_str(), &found);
                if (
                    found &&
                    !std::binary_search(Stage1Data::nil.as_mutable_key_words().begin(),
                                        Stage1Data::nil.as_mutable_key_words().end(),
                                        term) &&
                    std::find(
                        Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.begin(),
                        Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end(),
                        syn
                    ) == Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end()
                )
                    Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.push_back(syn);
            }
            while (ch == ',');
        }

        // skip to beginning of next line
        do
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == EOF)
                goto end;
        }
        while (ch == 10 || ch == 13);
        ungetc(ch, f);
        --task.as_mutable_progress();

        SerialTask::check_progress(task);
    }

end:
    fclose(f);
    return true;
}
