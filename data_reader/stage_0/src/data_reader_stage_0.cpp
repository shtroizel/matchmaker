#include <cstring>
#include <filesystem>
#include <iostream>
#include <queue>

#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>
#include <matchable/MatchableMaker.h>

#include <matchmaker/parts_of_speech.h>
#include <matchmaker/data_reader_common.h>

#include <SerialTask.h>
#include <Stage0Data.h>
#include <prepare_prefixes.h>
#include <read_3201.h>
#include <read_3201_acronyms.h>
#include <read_3201_common.h>
#include <read_3201_compound.h>
#include <read_3201_crosswd.h>
#include <read_3201_fnames.h>
#include <read_3201_mnames.h>
#include <read_3201_names.h>
#include <read_3201_places.h>
#include <read_3201_single.h>
#include <read_3202.h>
#include <read_3203.h>
#include <read_crumbs.h>
#include <read_keys.h>
#include <save_matchables.h>



void print_usage();



int main(int argc, char ** argv)
{
    if (argc < 5 || argc > 7)
    {
        std::cout << "invalid usage!" << std::endl;
        return 2;
    }

    Stage0Data::data_dir() = argv[1];
    Stage0Data::book_vocab_dir() = argv[2];
    Stage0Data::output_dir() = argv[3];
    Stage0Data::prefixes_filename() = argv[4];

    Stage0Data::dictionary_mode() = dictionary_usage::all::grab();
    if (argc >= 6)
    {
        Stage0Data::dictionary_mode() = dictionary_usage::from_string(argv[5]);
        if (Stage0Data::dictionary_mode().is_nil())
        {
            print_usage();
            return 2;
        }
    }
    std::cout << "dictionary usage: " << Stage0Data::dictionary_mode() << std::endl;

    if (!select_books(argc == 7 ? argv[6] : "all"))
    {
        print_usage();
        return 2;
    }
    std::cout << "books:";
    for (auto const & book : selected_books().currently_set())
        std::cout << " " << book;
    std::cout << std::endl;


    SerialTask::Flags tasks;
    for (auto const & task : SerialTask::variants_by_index())
        if (SerialTask::book_of(task).is_nil() || selected_books().is_set(SerialTask::book_of(task)))
            tasks.set(task);

    SerialTask::reset_progress();
    int const magic_number = 58;
    // std::cout << "         1         2         3         4         5         6         7         8\n"
    //           << "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    //           << std::endl;
    for (SerialTask::Type task : tasks.currently_set())
    {
        std::cout << "       ------> " << task << std::flush;
        task.set_progress_steps(magic_number - task.as_string().length());
        if (nullptr == task.as_run())
        {
            std::cout << "\nrun function is null!" << std::endl;
            abort();
        }

        task.as_run()(task);

        for (int i = task.as_printed_progress(); i < task.as_progress_steps(); ++i)
            ++task.as_mutable_progress();
        SerialTask::check_progress(task);
        std::cout << std::endl;
    }

    return 0;
}



void print_usage()
{
    std::cout << "program expects 4 to 6 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  book vocabulary directory\n"
              << "    [3]  output directory\n"
              << "    [4]  prefix filename\n"
              << "    [5]  dictionary usage (optional, defaults to all), one of:";
    for (auto v : dictionary_usage::variants_by_string())
        std::cout << " " << v;
    std::cout << "\n    [6]  books (optional, defaults to all), all or a comma separated list of:";
    for (auto v : Buch::variants_by_index())
        std::cout << " " << v;
    std::cout << std::endl;
}
