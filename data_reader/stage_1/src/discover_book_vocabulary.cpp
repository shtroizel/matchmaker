#include <filesystem>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "discover_book_vocabulary.h"

#include "Stage1Data.h"



bool discover_book_vocabulary(SerialTask::Type task);
bool discover_book_vocabulary(
    SerialTask::Type task,
    Buch::Type,
    std::filesystem::directory_entry const & entry
);



bool discover_book_vocabulary(SerialTask::Type task)
{
    task.set_progress(0);
    task.set_goal(0);

    std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>> dir_entries;
    dir_entries.reserve(10000);
    for (auto const & book : Buch::variants_by_index())
    {
        std::string BOOK_VOCAB_DIR{Stage1Data::nil.as_workspace_dir()};
        BOOK_VOCAB_DIR += "/book_vocabulary/";
        BOOK_VOCAB_DIR += book.as_string();

        for (auto const & entry : std::filesystem::recursive_directory_iterator(BOOK_VOCAB_DIR))
        {
            if (entry.is_regular_file())
            {
                task.as_mutable_goal() += entry.file_size();
                dir_entries.push_back(std::make_pair(book, entry));
            }
        }
    }

    for (auto const & [book, entry] : dir_entries)
    {
        if (!discover_book_vocabulary(task, book, entry))
            abort();
    }

    return true;
}



bool discover_book_vocabulary(
    SerialTask::Type task,
    Buch::Type book,
    std::filesystem::directory_entry const & entry
)
{
    FILE * f{nullptr};
    int ch = 0;
    std::string s;
    bool ok{false};
    int term{-1};

    s.clear();

    f = fopen(entry.path().c_str(), "r");
    if (f == nullptr)
    {
        perror(entry.path().c_str());
        abort();
        return false;
    }

    while ((ch = fgetc(f)) != EOF)
    {
        if (ch == '\n')
        {
            term = mm_lookup(s.c_str(), &ok);
            if (!ok)
            {
                std::cout << " --> failed to lookup '" << s << "' within "
                            << entry.path() << std::endl;
                abort();
                return false;
            }

            // flag book
            Stage1Data::nil.as_mutable_used_in_book()[term].set(book);

            s.clear();
        }
        else
        {
            s += (char) ch;
        }

        ++task.as_mutable_progress();
        SerialTask::check_progress(task);
    }

    fclose(f);

    return true;
}
