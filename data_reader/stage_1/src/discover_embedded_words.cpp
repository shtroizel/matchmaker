#include "discover_embedded_words.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



inline bool is_word_separator(char ch)
{
    return ch != '\'' && (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z');
}


void discover_embedded(std::vector<int> const & word_indexes, int progress_steps)
{
    IndexTable & embedded_words = Stage1Data::nil.as_mutable_embedded_words();

    int progress{1};

    for (int w = 0; w < (int) word_indexes.size(); ++w)
    {
        int const & word_index = word_indexes[w];
        if (progress_steps > 0 && w > progress * (int) word_indexes.size() / progress_steps)
        {
            std::cout << "." << std::flush;
            ++progress;
        }

        int word_len = 0;
        char const * word = mm_at(word_index, &word_len);
        std::string word_chunk = word;
        int search_start_index = word_index;
        int char_index = 0;
        int prev_word_len = 0;
        char const * prev_word = nullptr;
        std::vector<int> found_embedded_words;

        while (char_index < word_len)
        {
            int search_index = search_start_index - 1;
            bool found = false;

            do
            {
                // find first prev word that is shorter than the remaining (unparsed) "word_chunk"
                while (search_index >= 0)
                {
                    prev_word = mm_at(search_index, &prev_word_len);
                    if (prev_word_len < (int) word_chunk.length())
                        break;
                    else
                        --search_index;
                }

                // only count words that are separated to avoid cases like
                // banana where "a" and "an" would otherwise be embedded words
                if ((word_chunk.substr(0, prev_word_len) == prev_word) &&
                        (is_word_separator(prev_word[0]) ||
                        ((char_index == 0 || is_word_separator(word[char_index - 1])) &&
                        is_word_separator(word_chunk[prev_word_len]))))
                {
                    found = true;
                    found_embedded_words.push_back(search_index);

                    char_index += prev_word_len;
                    word_chunk.erase(0, prev_word_len);
                    bool remaining_chunk_is_a_word = false;
                    search_start_index = mm_lookup(word_chunk.c_str(), &remaining_chunk_is_a_word);

                    if (remaining_chunk_is_a_word)
                    {
                        found_embedded_words.push_back(search_start_index);
                        char_index += word_chunk.length();
                    }
                }
                else
                {
                    --search_index;
                }
            }
            while (!found && search_index >= 0);

            if (!found)
                char_index += word_len;
        }

        if (char_index == word_len)
        {
            embedded_words[word_index].insert(embedded_words[word_index].end(),
                                              found_embedded_words.begin(),
                                              found_embedded_words.end());
        }

        // is this really an error?
//         else
//         {
//             std::cout << "Error calculating embedded words for: " << word << std::endl;
//         }
    }

    if (progress_steps > 0)
        std::cout << "." << std::flush;
}



bool discover_embedded_words(int progress_steps)
{
    // create vector of indexes for each thread
    int const CPU_COUNT = std::thread::hardware_concurrency();
    std::vector<std::vector<int>> dealt_word_indexes;
    dealt_word_indexes.reserve(CPU_COUNT);
    for (int i = 0; i < CPU_COUNT; ++i)
        dealt_word_indexes.push_back(std::vector<int>());

    // deal out words to each thread
    int i = 0;
    for (auto w : Stage1Data::nil.as_mutable_by_longest())
    {
        if (w != 0)
        {
            dealt_word_indexes[i].push_back(w);
            ++i;
            if (i >= CPU_COUNT)
                i = 0;
        }
    }

    // los gehts
    std::vector<std::thread> threads;
    for (auto const & deal : dealt_word_indexes)
        if (dealt_word_indexes[0][0] == deal[0])
            threads.emplace_back(std::thread([&](){ discover_embedded(deal, progress_steps); }));
        else
            threads.emplace_back(std::thread([&](){ discover_embedded(deal, -1); }));

    for (auto & t : threads)
        t.join();

    return true;
}
