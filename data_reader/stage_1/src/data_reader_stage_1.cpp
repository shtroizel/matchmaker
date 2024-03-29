#include <iostream>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"
#include "SerialTask.h"



MATCHABLE(q_usage, included, omitted, only)


void print_usage();



int main(int argc, char ** argv)
{
    if (argc != 5)
    {
        print_usage();
        return 2;
    }

    std::cout << "******* stage 1 data reader *******\n" << std::endl;

    Stage1Data::nil.as_mutable_data_dir() = argv[1];
    Stage1Data::nil.as_mutable_workspace_dir() = argv[2];
    Stage1Data::nil.as_mutable_template_dir() = argv[3];
    q_usage::Type q_mode = q_usage::from_string(argv[4]);
    if (q_mode.is_nil())
        q_mode = q_usage::included::grab();

    Stage1Data::nil.as_mutable_used_in_book().reserve(mm_count());
    Stage1Data::nil.as_mutable_by_longest().reserve(mm_count());
    Stage1Data::nil.as_mutable_embedded_words().reserve(mm_count());
    Stage1Data::nil.as_mutable_definitions().reserve(mm_count());
    Stage1Data::nil.as_mutable_book_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_chapter_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_paragraph_indexes().reserve(mm_count());
    Stage1Data::nil.as_mutable_word_indexes().reserve(mm_count());
    for (int i = 0; i < mm_count(); ++i)
    {
        Stage1Data::nil.as_mutable_used_in_book().push_back(Buch::Flags{});
        Stage1Data::nil.as_mutable_embedded_words().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_definitions().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_book_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_chapter_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_paragraph_indexes().push_back(std::vector<int>());
        Stage1Data::nil.as_mutable_word_indexes().push_back(std::vector<int>());
    }

    SerialTask::Flags tasks;
    for (auto const & task : SerialTask::variants_by_index())
        tasks.set(task);

    if (q_mode == q_usage::omitted::grab())
        tasks.unset(SerialTask::reading_spc_crumbs::grab());

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
    std::cout << "program expects 4 arguments:\n"
              << "    [1]  data directory\n"
              << "    [2]  stage 1 workspace directory\n"
              << "    [3]  template directory\n"
              << "    [4]  Q mode (one of \"included\", \"omitted\", or \"only\")\n"
              << std::flush;
}
