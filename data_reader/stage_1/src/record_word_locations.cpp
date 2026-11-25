#include "record_word_locations.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"
#include "write_array.h"



bool record_word_locations(SerialTask::Type task)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    IndexTable book_indexes(mm_count());
    IndexTable chapter_indexes(mm_count());
    IndexTable paragraph_indexes(mm_count());
    IndexTable word_indexes(mm_count());

    {
        int location_count{0};
        for (int b = 0; b < (int) books.size(); ++b)
            for (int ch = 0; ch < (int) books[b].chapters.size(); ++ch)
                for (int p = 0; p < (int) books[b].chapters[ch].paragraphs.size(); ++p)
                    location_count += (int) books[b].chapters[ch].paragraphs[p].size();

        task.set_goal(location_count);
        task.set_progress(0);
    }

    char const * s = nullptr;
    int len{0};
    for (int b = 0; b < (int) books.size(); ++b)
    {
        for (int ch = 0; ch < (int) books[b].chapters.size(); ++ch)
        {
            for (int p = 0; p < (int) books[b].chapters[ch].paragraphs.size(); ++p)
            {
                for (int w = 0; w < (int) books[b].chapters[ch].paragraphs[p].size(); ++w)
                {
                    BookWord const & bw = books[b].chapters[ch].paragraphs[p][w];

                    s = mm_at(bw.word, &len);
                    if (
                        (len != 1 || s[0] == 'Q' || s[0] == 'q') &&
                        strcmp(s, "the") != 0 &&
                        strcmp(s, "The") != 0 &&
                        strcmp(s, "an") != 0 &&
                        strcmp(s, "An") != 0 &&
                        strcmp(s, "http") != 0 &&
                        strcmp(s, ">>") != 0 &&
                        strcmp(s, "~~~") != 0 &&
                        strcmp(s, "~~:") != 0 &&
                        strcmp(s, "]~~") != 0
                    )
                    {
                        book_indexes[bw.word].push_back(b);
                        chapter_indexes[bw.word].push_back(ch);
                        paragraph_indexes[bw.word].push_back(p);
                        word_indexes[bw.word].push_back(w);
                    }

                    // record parents

                    int pp = -1;
                    int ppsi = -1;
                    if (bw.ancestors.size() > 0)
                    {
                        pp = bw.ancestors[0];
                        ppsi = bw.first_ancestor_start_index;
                    }

                    if (pp >= 0 && ppsi >= 0)
                    {
                        bool parent_added = false;
                        size_t const location_count = book_indexes[pp].size();
                        for (size_t i = 0; !parent_added && i < location_count; ++i)
                        {
                            if (word_indexes[pp][i] == ppsi &&
                                    paragraph_indexes[pp][i] == p &&
                                    chapter_indexes[pp][i] == ch &&
                                    book_indexes[pp][i] == b)
                                parent_added = true;
                        }

                        if (!parent_added)
                        {
                            book_indexes[pp].push_back(b);
                            chapter_indexes[pp].push_back(ch);
                            paragraph_indexes[pp].push_back(p);
                            word_indexes[pp].push_back(ppsi);
                        }
                    }

                    ++task.as_mutable_progress();
                    SerialTask::check_progress(task);
                }
            }
        }
    }

    // a term's locations are its books, then its chapters, its paragraphs and its words
    std::vector<int> term_ints;
    term_ints.reserve(mm_count());
    for (int term = 0; term < mm_count(); ++term)
        term_ints.push_back(4 * (int) book_indexes[term].size());

    return write_term_blocks(
        "locations", "location_shift", term_ints,
        [&](int term, std::vector<int> & block)
        {
            for (IndexTable const * indexes :
                     {&book_indexes, &chapter_indexes, &paragraph_indexes, &word_indexes})
                block.insert(block.end(), (*indexes)[term].begin(), (*indexes)[term].end());
        }
    );
}
