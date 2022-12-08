#include "read_3202.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "Stage1Data.h"



bool read_3202(int progress_steps)
{
    std::string const FN_3202{Stage1Data::nil.as_data_dir() + "/3202/files/mthesaur.txt"};
    FILE * f = fopen(FN_3202.c_str(), "r");
    if (nullptr == f)
    {
        perror(FN_3202.c_str());
        return false;
    }

    std::filesystem::path p{FN_3202.c_str()};
    unsigned long goal{std::filesystem::file_size(p)};
    int progress{0};
    int mod_progress{0};
    int prev_mod_progress{0};

    std::string syn;
    std::string key;

    int ch = 0;
    while (true)
    {
        // read key (up to first ',')
        key.clear();
        while (true)
        {
            ch = fgetc(f);
            ++progress;
            if (ch == EOF)
                goto end;

            if (ch == ',')
                break;

            key += (char) ch;
        }

        // read each comma separated synonym
        do
        {
            // read next synonym
            syn.clear();
            while (true)
            {
                ch = fgetc(f);
                ++progress;
                if (ch == EOF)
                    goto end;

                if (ch == ',' || ch == 10 || ch == 13)
                    break;

                syn += (char) ch;
            }
            if (
                std::find(
                    Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.begin(),
                    Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end(),
                    syn
                ) == Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.end()
            )
            Stage1Data::nil.as_mutable_syn_ant_table()[key].syn.push_back(syn);
        }
        while (ch == ',');

        // skip to beginning of next line
        do
        {
            ch = fgetc(f);
            ++progress;
            if (ch == EOF)
                goto end;
        }
        while (ch == 10 || ch == 13);
        ungetc(ch, f);
        --progress;

        mod_progress = progress % (goal / progress_steps);
        if (mod_progress < prev_mod_progress)
            std::cout << "." << std::flush;
        prev_mod_progress = mod_progress;
    }

end:
    std::cout << "." << std::flush;
    fclose(f);
    return true;
}
