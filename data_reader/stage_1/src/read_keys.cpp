#include "read_keys.h"

#include <filesystem>
#include <iostream>
#include <string>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



PROPERTYx1_MATCHABLE(
    std::string, cur_term,

    KeysData
)


using ModeFunc = std::function<bool (SerialTask::Type task, FILE * f)>;

PROPERTYx1_MATCHABLE(
    ModeFunc, parse_func,

    Mode,

    Term,
    Syn,
    Ant,
    Def
)

bool parse_nil(SerialTask::Type task, FILE * f);
MATCHABLE_NIL_PROPERTY_VALUE(Mode, parse_func, &parse_nil)

bool parse_Term(SerialTask::Type task, FILE * f);
MATCHABLE_VARIANT_PROPERTY_VALUE(Mode, Term, parse_func, &parse_Term)

bool parse_Syn(SerialTask::Type task, FILE * f);
MATCHABLE_VARIANT_PROPERTY_VALUE(Mode, Syn, parse_func, &parse_Syn)

bool parse_Ant(SerialTask::Type task, FILE * f);
MATCHABLE_VARIANT_PROPERTY_VALUE(Mode, Ant, parse_func, &parse_Ant)

bool parse_Def(SerialTask::Type task, FILE * f);
MATCHABLE_VARIANT_PROPERTY_VALUE(Mode, Def, parse_func, &parse_Def)



bool read_line(SerialTask::Type task, FILE * f, std::string & line);


bool read_keys(SerialTask::Type task)
{
    std::string const keys_path{Stage1Data::nil.as_data_dir() + "/Crumbs/keys/keys"};
    FILE * f = fopen(keys_path.c_str(), "r");
    if (nullptr == f)
    {
        perror(keys_path.c_str());
        return false;
    }

    std::filesystem::path p{keys_path.c_str()};
    task.set_goal(std::filesystem::file_size(p));
    task.set_progress(0);

    std::string term;
    int ch{0};
    Mode::Type mode;

    while (true)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();

        if (ch == '~')
        {
            ch = fgetc(f);
            ++task.as_mutable_progress();
            if (ch == ']')
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == '[')
                {
                    ch = fgetc(f);
                    ++task.as_mutable_progress();
                    if (ch == '~')
                    {
                        if (!read_line(task, f, term))
                            goto err;

                        mode = Mode::from_string(term);
                        if (mode.is_nil())
                        {
                            std::cout << "read_keys() --> failed to parse mode from: \""
                                      << term << "\"\nvalid modes are:" << std::endl;
                            for (auto m : Mode::variants_by_index())
                                std::cout << "    " << m << std::endl;
                        }
                    }
                    else
                    {
                        ungetc(ch, f);
                        ungetc('[', f);
                        ungetc(']', f);
                        ungetc('~', f);
                        task.as_mutable_progress() -= 4;
                    }
                }
                else
                {
                    ungetc(ch, f);
                    ungetc(']', f);
                    ungetc('~', f);
                    task.as_mutable_progress() -= 3;
                }
            }
            else
            {
                ungetc(ch, f);
                ungetc('~', f);
                task.as_mutable_progress() -= 2;
            }
        }
        else
        {
            ungetc(ch, f);
            --task.as_mutable_progress();
        }

        if (ch == EOF)
            break;

        if (!mode.as_parse_func()(task, f))
            goto err;

        SerialTask::check_progress(task);
    }

    fclose(f);
    return true;

err:
    fclose(f);
    return false;
}



bool parse_nil(SerialTask::Type, FILE *)
{
    std::cout << "read_keys() -> Mode is nil!" << std::endl;
    return false;
}



bool parse_Term(SerialTask::Type task, FILE * f)
{
    std::string term;
    if (!read_line(task, f, term))
        return false;

    KeysData::nil.set_cur_term(term);

    bool found = false;
    int term_as_index = mm_lookup(term.c_str(), &found);
    if (!found)
    {
        std::cout << "read_keys() --> parse_Term() --> term, "
                  << term
                  << ", not found in dictionary"
                  << std::endl;
        abort();
    }

    Stage1Data::nil.as_mutable_key_words().push_back(term_as_index);
    std::stable_sort(
        Stage1Data::nil.as_mutable_key_words().begin(),
        Stage1Data::nil.as_mutable_key_words().end()
    );

    return true;
}



bool parse_Syn(SerialTask::Type task, FILE * f)
{
    std::string syn;
    if (!read_line(task, f, syn))
        return false;

    std::string const & t = KeysData::nil.as_cur_term();

    if (
        std::find(
            Stage1Data::nil.as_mutable_syn_ant_table()[t].syn.begin(),
            Stage1Data::nil.as_mutable_syn_ant_table()[t].syn.end(),
            syn
        ) == Stage1Data::nil.as_mutable_syn_ant_table()[t].syn.end()
    )
        Stage1Data::nil.as_mutable_syn_ant_table()[t].syn.push_back(syn);

    bool found = false;
    int syn_as_index = mm_lookup(syn.c_str(), &found);
    if (!found)
    {
        std::cout << "read_keys() --> parse_Syn() --> syn, "
                  << syn
                  << ", not found in dictionary"
                  << std::endl;
        abort();
    }
    Stage1Data::nil.as_mutable_key_words().push_back(syn_as_index);
    std::stable_sort(
        Stage1Data::nil.as_mutable_key_words().begin(),
        Stage1Data::nil.as_mutable_key_words().end()
    );

    return true;
}



bool parse_Ant(SerialTask::Type task, FILE * f)
{
    std::string ant;
    if (!read_line(task, f, ant))
        return false;

    std::string const & t = KeysData::nil.as_cur_term();

    if (
        std::find(
            Stage1Data::nil.as_mutable_syn_ant_table()[t].ant.begin(),
            Stage1Data::nil.as_mutable_syn_ant_table()[t].ant.end(),
            ant
        ) == Stage1Data::nil.as_mutable_syn_ant_table()[t].ant.end()
    )
        Stage1Data::nil.as_mutable_syn_ant_table()[t].ant.push_back(ant);

    bool found = false;
    int ant_as_index = mm_lookup(ant.c_str(), &found);
    if (!found)
    {
        std::cout << "read_keys() --> parse_Syn() --> ant, "
                  << ant
                  << ", not found in dictionary"
                  << std::endl;
        abort();
    }
    Stage1Data::nil.as_mutable_key_words().push_back(ant_as_index);
    std::stable_sort(
        Stage1Data::nil.as_mutable_key_words().begin(),
        Stage1Data::nil.as_mutable_key_words().end()
    );

    return true;
}



bool parse_Def(SerialTask::Type task, FILE * f)
{
    bool found = false;
    int t = mm_lookup(KeysData::nil.as_cur_term().c_str(), &found);
    if (!found)
    {
        std::cout << "read_keys() --> current term, "
                    << KeysData::nil.as_cur_term()
                    << ", not found in the dictionary!"
                    << std::endl;
        return false;
    }

    std::string line;
    if (!read_line(task, f, line))
        return false;

    int line_as_term = mm_lookup(line.c_str(), &found);
    if (!found)
    {
        std::cout << "read_keys() --> current term: " << KeysData::nil.as_cur_term()
                  << "\n    --> def line not found in the dictionary: " << line << std::endl;
        return false;
    }

    IndexTable & defs = Stage1Data::nil.as_mutable_definitions();
    defs[t].push_back(line_as_term);

    return true;
}



bool read_line(SerialTask::Type task, FILE * f, std::string & line)
{
    int ch;
    line.clear();

    while (true)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();

        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
            break;

        line += (char) ch;
    }

    // skip to end of line
    while (ch != 10 && ch != 13)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();
        if (ch == EOF)
            return false;
    }

    // skip to beginning of next line
    do
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();
    }
    while (ch == 10 || ch == 13);
    ungetc(ch, f);
    --task.as_mutable_progress();

    if (line == "_spc_")
        line = " ";

    return true;
}
