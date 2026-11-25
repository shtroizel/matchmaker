#include <cstdint>
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
    uint32_t * book_bits,
    std::filesystem::directory_entry const & entry
);



bool discover_book_vocabulary(SerialTask::Type task)
{
    task.set_progress(0);
    task.set_goal(0);

    // for each selected book one bit per term, in ints of 32, set when the book uses the term: here the
    // terms of its vocabulary, and the words and phrases of its text once the books are created
    size_t const ints_per_book = ((size_t) mm_count() + 31) / 32;
    std::vector<uint32_t> & used_in_book = Stage1Data::nil.as_mutable_used_in_book();
    used_in_book.assign(selected_books().currently_set().size() * ints_per_book, 0);

    std::vector<std::pair<size_t, std::filesystem::directory_entry>> dir_entries;
    dir_entries.reserve(10000);
    size_t book_index = 0;
    for (auto const & book : selected_books().currently_set())
    {
        std::string BOOK_VOCAB_DIR{Stage1Data::nil.as_workspace_dir()};
        BOOK_VOCAB_DIR += "/book_vocabulary/";
        BOOK_VOCAB_DIR += book.as_string();

        for (auto const & entry : std::filesystem::recursive_directory_iterator(BOOK_VOCAB_DIR))
        {
            if (entry.is_regular_file())
            {
                task.as_mutable_goal() += entry.file_size();
                dir_entries.push_back(std::make_pair(book_index, entry));
            }
        }
        ++book_index;
    }

    for (auto const & [index, entry] : dir_entries)
    {
        if (!discover_book_vocabulary(task, used_in_book.data() + index * ints_per_book, entry))
            abort();
    }

    return true;
}



bool discover_book_vocabulary(
    SerialTask::Type task,
    uint32_t * book_bits,
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

            book_bits[term / 32] |= uint32_t{1} << (term % 32);

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
