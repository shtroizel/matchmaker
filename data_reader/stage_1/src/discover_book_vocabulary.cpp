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

std::mutex progress_mutex;
int progress{0};
int goal{0};
int steps{0};



bool discover_book_vocabulary(int progress_steps);
bool discover_book_vocabulary(
    std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>> const & dir_entries
);



bool discover_book_vocabulary(int progress_steps)
{
    steps = progress_steps;
    std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>> dir_entries;
    dir_entries.reserve(10000);

    for (auto const & book : Buch::variants_by_index())
    {
        std::string BOOK_VOCAB_DIR{Stage1Data::nil.as_workspace_dir()};
        BOOK_VOCAB_DIR += "/book_vocabulary/";
        BOOK_VOCAB_DIR += book.as_string();

        // create vector of all files with this book's vocabulary
        for (auto const & entry : std::filesystem::recursive_directory_iterator(BOOK_VOCAB_DIR))
            if (entry.is_regular_file())
                dir_entries.push_back(std::make_pair(book, entry));
    }

    goal = (int) dir_entries.size();

    // create vectors for each cpu
    int const CPU_COUNT = std::min((int) std::thread::hardware_concurrency(), goal);
    std::vector<std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>>> dealt_entries;
    dealt_entries.reserve(CPU_COUNT);
    for (int i = 0; i < CPU_COUNT; ++i)
        dealt_entries.push_back(std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>>());

    // deal out work to each cpu's vector
    int i = 0;
    for (auto const & book_entry_pair : dir_entries)
    {
        dealt_entries[i].push_back(book_entry_pair);
        ++i;
        if (i >= CPU_COUNT)
            i = 0;
    }

    std::vector<std::thread> threads;
    bool all_ok{true};
    for (auto const & deal : dealt_entries)
        threads.emplace_back(
            std::thread(
                [&]()
                {
                    if (all_ok && !discover_book_vocabulary(deal))
                        all_ok = false;
                }
            )
        );
    for (auto & t : threads)
        t.join();

    return all_ok;
}



bool discover_book_vocabulary(
    std::vector<std::pair<Buch::Type, std::filesystem::directory_entry>> const & book_entry_pairs
)
{
    FILE * f{nullptr};
    int ch = 0;
    std::string s;
    bool ok{false};
    int term{-1};

    for (auto const & book_entry_pair : book_entry_pairs)
    {
        auto const & book = book_entry_pair.first;
        auto const & entry = book_entry_pair.second;

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
        }

        fclose(f);

        std::lock_guard<std::mutex> const lock(progress_mutex);
        ++progress;

        if (goal < steps && progress == goal)
            for (int i = 0; i < steps; ++i)
                std::cout << "." << std::flush;
        else
            if (progress % (goal / steps) == 0)
                std::cout << "." << std::flush;
    }

    return true;
}
