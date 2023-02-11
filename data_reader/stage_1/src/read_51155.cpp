#include "read_51155.h"

#include <filesystem>
#include <iostream>
#include <string>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

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



bool read_51155(SerialTask::Type task)
{
    std::string const FN_51155{Stage1Data::nil.as_data_dir() + "/51155/51155-0.txt"};
    FILE * f = fopen(FN_51155.c_str(), "r");
    if (nullptr == f)
    {
        perror(FN_51155.c_str());
        return false;
    }

    std::filesystem::path p{FN_51155.c_str()};
    task.set_goal(std::filesystem::file_size(p));
    task.set_progress(0);

    std::string term;
    int term_as_index = -1;
    bool found = false;
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
        ++task.as_mutable_progress();
        if (ch == 'K')
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == 'E')
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == 'Y')
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++task.as_mutable_progress();
                        if (ch == ' ')
                            key = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'S')
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == 'Y')
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == 'N')
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++task.as_mutable_progress();
                        if (ch == ' ')
                            syn = true;
                    }
                }
            }
        }
        else if (cur_key != "-1" && ch == 'A')
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == 'N')
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == 'T')
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();
                    if (ch == ':')
                    {
                        ch = fgetc(f);
                        ++task.as_mutable_progress();
                        if (ch == ' ')
                            ant = true;
                    }
                }
            }
        }

        if (key)
        {
            term.clear();
            status.clear();
            while (true)
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();

                if (ch == EOF)
                    goto err;

                if (ch == ' ' || ch == '_' || ch == '.' || ch == 10 || ch == 13)
                    break;

                if (ch >= 'A' && ch <= 'Z')
                    ch += 32;

                update_word_status(status, ch);
                term += (char) ch;
            }

            term_as_index = mm_lookup(term.c_str(), &found);
            if (term.empty() || status.is_set(word_status::has_other_symbols::grab()) || !found)
                cur_key = "-1";
            else if (!std::binary_search(Stage1Data::nil.as_mutable_key_words().begin(),
                                         Stage1Data::nil.as_mutable_key_words().end(),
                                         term_as_index))
                cur_key = term;
        }
        else if (syn || ant)
        {
            while (ch != '.')
            {
                term.clear();
                status.clear();

                // read word
                while (true)
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();

                    if (ch == EOF)
                        goto err;

                    if (ch == ' ' || ch == '.' || ch == ',' || ch == 10 || ch == 13)
                        break;

                    if (ch >= 'A' && ch <= 'Z')
                        ch += 32;

                    update_word_status(status, ch);
                    term += (char) ch;
                }

                term_as_index = mm_lookup(term.c_str(), &found);

                // maybe add word
                if (
                    term.size() > 0 &&
                    found &&
                    !std::binary_search(Stage1Data::nil.as_mutable_key_words().begin(),
                                        Stage1Data::nil.as_mutable_key_words().end(),
                                        term_as_index)
                )
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
                            term
                        ) == Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.end()
                    )
                        Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].syn.push_back(term);

                    if (
                        ant &&
                        std::find(
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.begin(),
                            Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.end(),
                            term
                        ) == Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.end()
                    )
                        Stage1Data::nil.as_mutable_syn_ant_table()[cur_key].ant.push_back(term);
                }
            }
        }

        // skip to end of line
        while (ch != 10 && ch != 13)
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == EOF)
                goto end;
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

err:
    fclose(f);
    return false;
}
