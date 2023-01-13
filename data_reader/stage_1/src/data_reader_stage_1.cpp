#include <iostream>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"
#include "SerialTask.h"

// #include "discover_book_vocabulary.h"
// #include "read_51155.h"
// #include "read_3202.h"
// #include "fill_reflections.h"
// #include "calculate_longest_words.h"
// #include "discover_embedded.h"
// #include "calculate_ordinal_summations.h"
// #include "read_crumbs.h"
// #include "create_books.h"
// #include "patch_headers.h"


MATCHABLE(q_usage, included, omitted, only)


void print_usage();
bool run_task(SerialTask::Type task);



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

    q_mode.match({
        {q_usage::only::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_51155::grab());
                tasks.unset(SerialTask::reading_spc_3202::grab());
                tasks.unset(SerialTask::fill_spc_reflections::grab());
            }},
        {q_usage::omitted::grab(),
            [&tasks]()
            {
                tasks.unset(SerialTask::reading_spc_crumbs::grab());
                tasks.unset(SerialTask::reading_spc_links::grab());
            }}
    });

    for (auto const & task : tasks.currently_set())
    {
        std::cout << "       ------> " << task << std::flush;
        bool ok = task.as_run()(36 - task.as_string().length());
        if (ok)
        {
            std::cout << ": SUCCESS" << std::endl;
        }
        else
        {
            std::cout << ": FAILED" << std::endl;
            return 1;
        }
    }

    return 0;
}



bool run_task(SerialTask::Type task)
{
    std::cout << "       ------> " << task << std::flush;
    bool ok = task.as_run()(36 - task.as_string().length());
    if (!ok)
    {
        std::cout << ": FAILED" << std::endl;
        return false;
    }
    std::cout << ": SUCCESS" << std::endl;
    return true;
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
