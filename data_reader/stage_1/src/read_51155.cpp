#include "read_51155.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "Stage1Data.h"



void update_word_status(word_status::Flags & flags, int & ch)
{
    if (ch < 32 || ch > 126)
    {
        flags.set(word_status::not_printable_ascii::grab());
        ch = '?';
    }
    else if (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z')
    {
        if (ch == ' ')
            flags.set(word_status::has_spaces::grab());
        else if (ch == '-')
            flags.set(word_status::has_hyphens::grab());
        else
            flags.set(word_status::has_other_symbols::grab());
    }
}



bool read_51155(int progress_steps)
{
    std::string const FN_51155{Stage1Data::nil.as_data_dir() + "/51155/51155-0.txt"};
    FILE * f = fopen(FN_51155.c_str(), "r");
    if (nullptr == f)
    {
        perror(FN_51155.c_str());
        return false;
    }

    std::filesystem::path p{FN_51155.c_str()};
    unsigned long goal{std::filesystem::file_size(p)};
    int progress{0};
    int mod_progress{0};
    int prev_mod_progress{0};

    std::string word;
    std::string cur_key{"-1"};
    word_status::Flags status;
    bool key{false};
    bool syn{false};
    bool ant{false};
    int ch{0};

    while (true)
    {
        key = false;
        syn = false;
        ant = false;

        ch = fgetc(f);
        ++progress;
        if (ch == 'K')
        {
            ch = fgetc(f);
            ++progress;
            if (ch == 'E')
            {
                ch = fgetc(f);
                ++progress;
                if (ch == 'Y')
                {
                    ch = fgetc(f);
                    ++progress;
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++progress;
                        if (ch == ' ')
                            key = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'S')
        {
            ch = fgetc(f);
            ++progress;
            if (ch == 'Y')
            {
                ch = fgetc(f);
                ++progress;
                if (ch == 'N')
                {
                    ch = fgetc(f);
                    ++progress;
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++progress;
                        if (ch == ' ')
                            syn = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'A')
        {
            ch = fgetc(f);
            ++progress;
            if (ch == 'N')
            {
                ch = fgetc(f);
                ++progress;
                if (ch == 'T')
                {
                    ch = fgetc(f);
                    ++progress;
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++progress;
                        if (ch == ' ')
                            ant = true;
                    }
                }
            }
        }

        if (key)
        {
            word.clear();
            status.clear();
            while (true)
            {
                ch = fgetc(f);
                ++progress;

                if (ch == EOF)
                    goto err;

                if (ch == ' ' || ch == '_' || ch == '.' || ch == 10 || ch == 13)
                    break;

                if (ch >= 'A' && ch <= 'Z')
                    ch += 32;

                update_word_status(status, ch);
                word += (char) ch;
            }
            if (word.size() > 0 && !status.is_set(word_status::has_other_symbols::grab()))
                cur_key = word;
            else
                cur_key = "-1";
        }
        else if (syn || ant)
        {
            while (ch != '.')
            {
                word.clear();
                status.clear();

                // read word
                while (true)
                {
                    ch = fgetc(f);
                    ++progress;

                    if (ch == EOF)
                        goto err;

                    if (ch == ' ' || ch == '.' || ch == ',' || ch == 10 || ch == 13)
                        break;

                    if (ch >= 'A' && ch <= 'Z')
                        ch += 32;

                    update_word_status(status, ch);
                    word += (char) ch;
                }

                // maybe add word
                if (word.size() > 0)
                {
                    if (status.is_set(word_status::not_printable_ascii::grab()))
                        continue;

                    if (status.is_set(word_status::has_other_symbols::grab()))
                        continue;

                    if (
                        syn &&
                        std::find(
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.begin(),
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.end(),
                            word
                        ) == Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.end()
                    )
                        Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.push_back(word);

                    if (
                        ant &&
                        std::find(
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.begin(),
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.end(),
                            word
                        ) == Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.end()
                    )
                        Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.push_back(word);
                }
            }
        }

        // skip to end of line
        while (ch != 10 && ch != 13)
        {
            ch = fgetc(f);
            ++progress;
            if (ch == EOF)
                goto end;
        }

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

err:
    fclose(f);
    return false;
}
