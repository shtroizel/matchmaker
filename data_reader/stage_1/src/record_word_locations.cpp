#include "record_word_locations.h"

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
                    if ((len != 1 || s[0] == 'Q' || s[0] == 'q') &&
                            strcmp(s, "the") != 0 && strcmp(s, "The") != 0 &&
                            strcmp(s, "an") != 0 && strcmp(s, "An") != 0)
                    {
                        book_indexes[bw.word].push_back(b);
                        chapter_indexes[bw.word].push_back(ch);
                        paragraph_indexes[bw.word].push_back(p);
                        word_indexes[bw.word].push_back(w);
                    }

                    // record links
                    if (w == 0 && len > 5 && s[0] == '>' && s[1] == '>')
                    {
                        bool all_numbers = true;
                        for (int i = 2; all_numbers && i < len; ++i)
                            all_numbers = s[i] >= '0' && s[i] <= '9';

                        if (all_numbers)
                        {
                            std::function<void (int)> add_term =
                                [&](int term)
                                {
                                    // add term
                                    if ((len != 1 || s[0] == 'Q' || s[0] == 'q') &&
                                            strcmp(s, "the") != 0 && strcmp(s, "The") != 0 &&
                                            strcmp(s, "an") != 0 && strcmp(s, "An") != 0)
                                    {
                                        book_indexes[term].push_back(b);
                                        chapter_indexes[term].push_back(ch);
                                        paragraph_indexes[term].push_back(p);
                                        word_indexes[term].push_back(w);

                                        int s_len = 0;
                                        char const * s = mm_at(term, &s_len);
                                        if (s_len > 2 && s[0] == '>' && s[1] == '>')
                                            return;

                                        // add embedded
                                        std::vector<int> embedded =
                                                Stage1Data::nil.as_embedded_words()[term];
                                        for (int e : embedded)
                                            add_term(e);
                                    }
                                };

                            IndexTable const & defs = Stage1Data::nil.as_definitions();
                            for (auto d : defs[bw.word])
                                add_term(d);
                        }
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

                    ++locations_processed;
                    if (locations_processed % (location_count / progress_steps) == 0)
                        std::cout << "." << std::flush;
                }
            }
        }
    }

    return true;
}
