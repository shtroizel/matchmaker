#include <read_3202.h>

#include <filesystem>

#include <SerialTask.h>
#include <Stage0Data.h>



void read_3202(SerialTask::Type task)
{
    std::filesystem::path path_FN_3202{Stage0Data::data_dir() + "/3202/files/mthesaur.txt"};

    task.set_goal(std::filesystem::file_size(path_FN_3202));
    task.set_progress(0);

    FILE * input_file = fopen(path_FN_3202.c_str(), "r");
    if (input_file == 0)
    {
        perror(path_FN_3202.c_str());
        abort();
    }

    std::string word;
    parts_of_speech::Flags pos_flags;
    Stage0Data::word_attribute::Flags attributes;

    int ch = 0;
    while (true)
    {
        word.clear();
        pos_flags.clear();
        attributes.clear();

        while (true)
        {
            ch = fgetc(input_file);
            ++task.as_mutable_progress();
            if (ch == EOF || ch == 10 || ch == 13 || ch == ',')
                break;

            Stage0Data::update_word_attribute(attributes, ch);
            word += (char) ch;
        }

        Stage0Data::add_word(word, attributes, task, pos_flags);

        if (ch == EOF)
            break;

        SerialTask::check_progress(task);
    }

   fclose(input_file);
}
