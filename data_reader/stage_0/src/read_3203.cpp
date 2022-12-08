#include <read_3203.h>

#include <filesystem>
#include <iostream>

#include <Stage0Data.h>



bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    parts_of_speech::Flags & pos_flags,
    SerialTask::Type task
);

void read_3203(SerialTask::Type task)
{
    std::filesystem::path path_FN_3203_MOBYPOS{Stage0Data::data_dir() + "/3203/files/mobypos.txt"};

    task.set_goal(std::filesystem::file_size(path_FN_3203_MOBYPOS));
    task.set_progress(0);

    FILE * mobypos_file = fopen(path_FN_3203_MOBYPOS.c_str(), "r");
    if (mobypos_file == 0)
    {
            perror(path_FN_3203_MOBYPOS.c_str());
            abort();
    }

    std::string word;
    parts_of_speech::Flags pos_flags;
    Stage0Data::word_attribute::Flags attributes;

    while (true)
    {
        attributes.clear();
        pos_flags.clear();

        if (attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
        {
            std::cout << "read_3203_mobypos() --> invisible_ascii !!!" << std::endl;
            abort();
        }

        if (!read_3203_mobypos_line(mobypos_file, word, attributes, pos_flags, task))
            break;

        if (word.size() == 0)
            continue;

        Stage0Data::add_word(word, attributes, pos_flags);
        SerialTask::check_progress(task);
    }

   fclose(mobypos_file);
}



bool read_3203_mobypos_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    parts_of_speech::Flags & pos_flags,
    SerialTask::Type task
)
{
    word.clear();
    attributes.clear();
    pos_flags.clear();

    while (true)
    {
        int ch = fgetc(f);
        if (ch == EOF)
            return false;

        ++task.as_mutable_progress();

        if (ch == 10 || ch == 13)
            continue;

        if (ch == (int) '\\')
            break;

        Stage0Data::update_word_attribute(attributes, ch);
        word += (char) ch;
    }

    while (true)
    {
        int ch = fgetc(f);
        if (ch == EOF)
            return false;

        ++task.as_mutable_progress();

        if (ch == 10 || ch == 13)
            break;

        if (ch < 32 || ch > 126)
            attributes.set(Stage0Data::word_attribute::invisible_ascii::grab());

        if (ch == (int) '!')
            ch = (int) 'n';

        std::string ch_str = std::string(1, (char) ch);
        auto pos_flag = parts_of_speech::from_string(ch_str);
        if (!pos_flag.is_nil())
            pos_flags.set(pos_flag);
    }

    return true;
}
