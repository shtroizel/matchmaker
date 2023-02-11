#include <read_keys.h>

#include <filesystem>

#include <SerialTask.h>
#include <Stage0Data.h>



void read_keys(SerialTask::Type task)
{
    std::filesystem::path path_keys{Stage0Data::data_dir() + "/Crumbs/keys/keys"};

    task.set_goal(std::filesystem::file_size(path_keys));
    task.set_progress(0);

    FILE * input_file = fopen(path_keys.c_str(), "r");
    if (input_file == 0)
    {
        perror(path_keys.c_str());
        abort();
    }

    std::string line;
    parts_of_speech::Flags pos_flags;
    Stage0Data::word_attribute::Flags attributes;

    int ch = 0;
    while (true)
    {
        line.clear();
        pos_flags.clear();
        attributes.clear();

        while (true)
        {
            ch = fgetc(input_file);
            ++task.as_mutable_progress();

            if (ch == EOF || ch == 10 || ch == 13)
                break;

            Stage0Data::update_word_attribute(attributes, ch);
            line += (char) ch;
        }

        if (line == "_spc_")
            continue;

        if (
            line.size() > 4
            && line[0] == '~'
            && line[1] == ']'
            && line[2] == '['
            && line[3] == '~'
        )
            continue;

        Stage0Data::add_word(line, attributes, task, pos_flags);

        if (ch == EOF)
            break;

        SerialTask::check_progress(task);
    }

   fclose(input_file);
}
