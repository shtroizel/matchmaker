#include "record_word_locations.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



bool record_word_locations(int progress_steps)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    IndexTable & book_indexes = Stage1Data::nil.as_mutable_book_indexes();
    IndexTable & chapter_indexes = Stage1Data::nil.as_mutable_chapter_indexes();
    IndexTable & paragraph_indexes = Stage1Data::nil.as_mutable_paragraph_indexes();
    IndexTable & word_indexes = Stage1Data::nil.as_mutable_word_indexes();

    int location_count{0};
    for (int b = 0; b < (int) books.size(); ++b)
        for (int ch = 0; ch < (int) books[b].chapters.size(); ++ch)
            for (int p = 0; p < (int) books[b].chapters[ch].paragraphs.size(); ++p)
                location_count += (int) books[b].chapters[ch].paragraphs[p].size();

    int locations_processed{0};

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
                    if (len != 1 || s[0] == 'Q' || s[0] == 'q')
                    {
                        book_indexes[bw.word].push_back(b);
                        chapter_indexes[bw.word].push_back(ch);
                        paragraph_indexes[bw.word].push_back(p);
                        word_indexes[bw.word].push_back(w);
                    }

                    if (bw.parent_phrase >= 0 && bw.parent_phrase_start_index >= 0)
                    {
                        bool parent_added = false;
                        for (size_t i = 0; !parent_added && i < book_indexes[bw.parent_phrase].size(); ++i)
                        {
                            if (word_indexes[bw.parent_phrase][i] == bw.parent_phrase_start_index &&
                                    paragraph_indexes[bw.parent_phrase][i] == p &&
                                    chapter_indexes[bw.parent_phrase][i] == ch &&
                                    book_indexes[bw.parent_phrase][i] == b)
                                parent_added = true;
                        }

                        if (!parent_added)
                        {
                            book_indexes[bw.parent_phrase].push_back(b);
                            chapter_indexes[bw.parent_phrase].push_back(ch);
                            paragraph_indexes[bw.parent_phrase].push_back(p);
                            word_indexes[bw.parent_phrase].push_back(bw.parent_phrase_start_index);
                        }
                    }

                    ++locations_processed;
                    if (locations_processed % (location_count / progress_steps) == 0)
                        std::cout << "." << std::flush;
                }
            }
        }
    }

    return true;
}
