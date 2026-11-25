#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "array_cuts.h"
#include "Stage1Data.h"
#include "SerialTask.h"
#include "write_array.h"





void print_usage();



int main(int argc, char ** argv)
{
    if (argc < 3 || argc > 7)
    {
        print_usage();
        return 2;
    }

    std::cout << "******* stage 1 data reader *******\n" << std::endl;

    Stage1Data::nil.as_mutable_data_dir() = argv[1];
    Stage1Data::nil.as_mutable_workspace_dir() = argv[2];
    int thread_count = argc >= 4 ? std::atoi(argv[3]) : (int) std::thread::hardware_concurrency();
    if (thread_count < 1)
        thread_count = 1;
    Stage1Data::nil.as_mutable_thread_count() = thread_count;

    if (!select_books(argc >= 5 ? argv[4] : "all"))
    {
        print_usage();
        return 2;
    }
    std::cout << "books:";
    for (auto const & book : selected_books().currently_set())
        std::cout << " " << book;
    std::cout << "\n" << std::endl;

    int const array_budget = argc >= 6 ? std::atoi(argv[5]) : 1000000;
    if (array_budget < 1)
    {
        print_usage();
        return 2;
    }
    Stage1Data::nil.as_mutable_array_budget() = array_budget;

    std::string const relation_ranks{argc == 7 ? argv[6] : "at_load"};
    if (relation_ranks != "at_load" && relation_ranks != "generated")
    {
        print_usage();
        return 2;
    }
    Stage1Data::nil.as_mutable_generated_relations() = relation_ranks == "generated";

    // every task writes its arrays anew, so no unit of an earlier cut stays behind
    std::filesystem::remove_all(arrays_dir());
    if (!write_array("unit_shift", {array_cuts::unit_shift(array_budget)}))
        return 1;

    Stage1Data::nil.as_mutable_by_longest().reserve(mm_count());
    Stage1Data::nil.as_mutable_embedded_words().reserve(mm_count());
    Stage1Data::nil.as_mutable_definitions().reserve(mm_count());
    for (int i = 0; i < mm_count(); ++i)
    {
        Stage1Data::nil.as_mutable_embedded_words().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_definitions().push_back(std::vector<int>());
    }

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

        if (!task.as_run()(task))
            abort();

        for (int i = task.as_printed_progress(); i < task.as_progress_steps(); ++i)
            ++task.as_mutable_progress();
        SerialTask::check_progress(task);
        std::cout << std::endl;
    }

    return 0;
}



void print_usage()
{
    std::cout << "program expects 2 to 6 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  stage 1 workspace directory\n"
              << "    [3]  thread count (optional, defaults to the number of hardware threads)\n"
              << "    [4]  books (optional, defaults to all), all or a comma separated list of:";
    for (auto v : Buch::variants_by_index())
        std::cout << " " << v;
    std::cout << "\n    [5]  array budget (optional, defaults to 1000000), the most ints in a compiled file"
              << " of the generated arrays, at least 1\n"
              << "    [6]  relation ranks (optional, defaults to at_load): generated also writes each"
              << " term's synonyms, antonyms, embedded terms and definition as arrays" << std::endl;
}
