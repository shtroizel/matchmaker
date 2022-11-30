/*
Copyright (c) 2022, shtroizel
All rights reserved without prejudice.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "read_crumbs.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>


#ifdef MATCHABLE_OMIT_BY_INDEX
    matchable with by_index required for stage 1 reader
#endif
#include <matchable/matchable.h>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include <matchmaker/data_reader_common.h>

#include "Stage1Data.h"



bool read_crumbs(
    std::string const & DATA_DIR,
    IndexTable const & embedded_words,
    Book & book
);

bool read_chapter(
    IndexTable const & embedded_words,
    FILE * input_file,
    Book & crumbs,
    int & chapter
);

bool read_line(FILE * f, std::string & line);

bool read_header(std::string const & header, Chapter & chapter);

void tokenize(std::string const & str, char const delim, std::vector<std::string> & out);





bool read_crumbs(int progress_steps)
{
    IndexTable const & embedded_words = Stage1Data::nil.as_embedded_words();
    std::string const & DATA_DIR = Stage1Data::nil.as_data_dir();

    Stage1Data::nil.as_mutable_books().push_back(Book());
    Book & book = Stage1Data::nil.as_mutable_books().back();

    bool ok{true};
    int i = mm_lookup("Crumbs", &ok);
    if (!ok)
    {
        std::cout << "FAILED!\nBook title 'Crumbs' not found in dictionary!" << std::endl;
        return false;
    }
    book.title.push_back(i);

    i = mm_lookup("Q", &ok);
    if (!ok)
    {
        std::cout << "FAILED!\nBook author 'Q' not found in dictionary!" << std::endl;
        return false;
    }
    book.author.push_back(i);

    // the recursive_directory_iterator provides the files in random order, so first sort
    // them by filename using the priority q
    std::priority_queue<
        QFile,
        std::vector<QFile>,
        std::less<std::vector<QFile>::value_type>
    > q_files;
    for (auto const & entry : std::filesystem::recursive_directory_iterator(DATA_DIR + "/Crumbs/text"))
        if (entry.is_regular_file())
            q_files.push({entry});

    FILE * q_file{nullptr};
    int files_processed{0};
    int printed_progress{0};
    int const file_count = (int) q_files.size();
    int chapter = 0;
    while (!q_files.empty())
    {
        ++files_processed;
        if (files_processed % (file_count / progress_steps) == 0 && printed_progress < progress_steps)
        {
            ++printed_progress;
            std::cout << "." << std::flush;
        }

        // TODO FIXME REMOVEME!!!!!!!!!!!!!!!!!!!!!!!!
        if (files_processed > 28)
        {
            q_files.pop();
            continue;
        }
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        q_file = fopen(q_files.top().entry.path().c_str(), "r");
        if (q_file == 0)
        {
            perror(q_files.top().entry.path().c_str());
            exit(1);
        }
        ok = read_chapter(embedded_words, q_file, book, chapter);
        fclose(q_file);
        if (!ok)
            return false;
        q_file = nullptr;
        q_files.pop();
    }
    return true;
}


bool read_chapter(
    IndexTable const & embedded_words,
    FILE * input_file,
    Book & crumbs,
    int & chapter
)
{
    std::string text_number = "INVALID";
    std::string line;
    bool word_in_dictionary{false};
    int word{-1};

    while (read_line(input_file, line))
    {
        if (line.length() == 0)
            continue;

        if (line.rfind("~~:", 0) == 0)
        {
            ++chapter;
            crumbs.chapters.push_back(Chapter());

            // first read the text number (chapter title)
            text_number = line.substr(3);
            int index = mm_lookup(text_number.c_str(), &word_in_dictionary);
            if (!word_in_dictionary)
            {
                std::cout << "FAILED!\nencountered '" << text_number << "' which is not in the dictionary!"
                          << std::endl;
                return false;
            }
            if (std::to_string(chapter) != mm_at(index, nullptr))
            {
                std::cout << "FAILED!\n"
                          << "text_number: " << text_number
                          << "\nencountered '" << mm_at(index, nullptr) << "' but " << chapter << " expected" << std::endl;
                return false;
            }
            crumbs.chapters.back().title.push_back(index);

            // then read the text's header (chapter subtitle)
            if (!read_line(input_file, line))
            {
                std::cout << "FAILED!\nunexpected end of file within text " << text_number << std::endl;
                return false;
            }
            if (!read_header(line, crumbs.chapters.back()))
            {
                std::cout << "failed to read header for text " << text_number << std::endl;
                return false;
            }
        }
        else
        {
            // entire line should exist as a word
            word = mm_lookup(line.c_str(), &word_in_dictionary);
            if (!word_in_dictionary)
            {
                std::cout << "FAILED!\nword '" << line
                          << "' not in the dictionary! text: " << text_number << std::endl;
                return false;
            }

            std::function<void (int,
                                int,
                                int,
                                IndexTable const &,
                                std::vector<BookWord> &)> add_to_paragraph =
                [&add_to_paragraph]
                (
                    int word,
                    int parent,
                    int parent_start,
                    IndexTable const & embedded_words,
                    std::vector<BookWord> & paragraph
                )
                {
                    if (embedded_words[word].size() == 0)
                    {
                        paragraph.push_back({word, parent, parent_start});
                    }
                    else
                    {
                        parent_start = (int) paragraph.size();
                        for (size_t i = 0; i < embedded_words[word].size(); ++i)
                        {
                            add_to_paragraph(embedded_words[word][i],
                                             word,
                                             parent_start,
                                             embedded_words,
                                             paragraph);
                        }
                    }
                };

            std::vector<BookWord> paragraph;
            add_to_paragraph(word, -1, -1, embedded_words, paragraph);
            crumbs.chapters.back().paragraphs.push_back(paragraph);
        }
    }
    return true;
}


bool read_line(FILE * f, std::string & line)
{
    line.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        line += (char) ch;
    }

    return true;
}


bool read_header(std::string const & header, Chapter & chapter)
{
    // first split date/time from message board
    std::vector<std::string> tokenized_on_comma;
    tokenize(header, ',', tokenized_on_comma);
    if (tokenized_on_comma.size() != 2)
    {
        std::cout << "FAILED!\nfailed to parse header line! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split date, time, zoneinfo (time is middle 2 tokens because of AM/PM)
    std::vector<std::string> tokenized_date_time;
    tokenize(tokenized_on_comma[0], ' ', tokenized_date_time);
    if (tokenized_date_time.size() != 4)
    {
        std::cout << "FAILED!\nfailed to parse date/time stamp! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split date into day month year
    std::vector<std::string> tokenized_date;
    tokenize(tokenized_date_time[0], '-', tokenized_date);
    if (tokenized_date.size() != 3)
    {
        std::cout << "FAILED!\nfailed to parse date stamp! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split time into hour min sec
    std::vector<std::string> tokenized_time;
    tokenize(tokenized_date_time[1], ':', tokenized_time);
    if (tokenized_time.size() != 3)
    {
        std::cout << "FAILED!\nfailed to parse time stamp! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // cache repeatedly used indexes
    bool word_in_dictionary{false};
    int const dash = mm_lookup("-", &word_in_dictionary);
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\n'-' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const space = mm_lookup(" ", &word_in_dictionary);
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\n' ' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const colon = mm_lookup(":", &word_in_dictionary);
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\n':' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const comma = mm_lookup(",", &word_in_dictionary);
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\n',' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // finally we can start creating the subtitle
    //
    // start by adding the year
    chapter.subtitle.push_back(mm_lookup(tokenized_date[2].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[2]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // then add the month
    chapter.subtitle.push_back(mm_lookup(tokenized_date[1].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // add the day
    chapter.subtitle.push_back(mm_lookup(tokenized_date[0].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[0]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(space);

    // add the hour
    chapter.subtitle.push_back(mm_lookup(tokenized_time[0].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_time[0]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // add a colon
    chapter.subtitle.push_back(colon);

    // add the minute
    chapter.subtitle.push_back(mm_lookup(tokenized_time[1].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_time[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // add another colon
    chapter.subtitle.push_back(mm_lookup(":", nullptr));

    // add the second
    chapter.subtitle.push_back(mm_lookup(tokenized_time[2].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_time[2]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(space);

    // add AM/PM
    chapter.subtitle.push_back(mm_lookup(tokenized_date_time[3].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date_time[3]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(comma);

    // add message board
    chapter.subtitle.push_back(mm_lookup(tokenized_on_comma[1].c_str(), &word_in_dictionary));
    if (!word_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_on_comma[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    return true;
}


void tokenize(std::string const & str, char const delim, std::vector<std::string> & out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}
