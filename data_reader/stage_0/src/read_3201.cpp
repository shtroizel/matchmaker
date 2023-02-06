#include <read_3201.h>

#include <filesystem>

#include <bits/types/FILE.h>

#include <SerialTask.h>
#include <Stage0Data.h>



bool read_3201_default_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
);



void read_3201_default(
    std::filesystem::path const file_path,
    Stage0Data::word_attribute::Flags const & base_attributes,
    SerialTask::Type task
)
{
    task.set_goal(std::filesystem::file_size(file_path));
    task.set_progress(0);

    FILE * input_file = fopen(file_path.c_str(), "r");
    if (input_file == 0)
    {
        perror(file_path.c_str());
        abort();
    }

    std::string word;
    Stage0Data::word_attribute::Flags attributes;

    while (true)
    {
        attributes = base_attributes;

        if (!read_3201_default_line(input_file, word, attributes, task))
            break;

        if (word.size() == 0)
            continue;

        if (attributes.is_set(Stage0Data::word_attribute::compound::grab()) &&
                word.find('-') == std::string::npos && word.find(' ') == std::string::npos)
            attributes.unset(Stage0Data::word_attribute::compound::grab());

        Stage0Data::add_word(word, attributes, task);

        SerialTask::check_progress(task);
    }

    fclose(input_file);
}



bool read_3201_default_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
)
{
    word.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();
        if (ch == EOF)
            return false;

        if (ch == 10 || ch == 13)
        {
            while (true)
            {
                ch = fgetc(f);
                ++task.as_mutable_progress();
                if (ch == EOF)
                    return false;

                if (ch != 10 && ch != 13)
                {
                    ungetc(ch, f);
                    break;
                }
            }
            break;
        }

        Stage0Data::update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    return true;
}
