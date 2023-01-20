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


bool read_link(int link_as_term, std::vector<int> & lines);

bool read_link_line(FILE * f, std::string & line);



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
    int post = 0;
    while (!q_files.empty())
    {
        ++files_processed;
        if (files_processed % (file_count / progress_steps) == 0 && printed_progress < progress_steps)
        {
            ++printed_progress;
            std::cout << "." << std::flush;
        }

        q_file = fopen(q_files.top().entry.path().c_str(), "r");
        if (q_file == 0)
        {
            perror(q_files.top().entry.path().c_str());
            exit(1);
        }

        ok = read_chapter(embedded_words, q_file, book, post);
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
    int & post
)
{
    std::string post_number = "INVALID";
    std::string line;
    bool term_exists{false};
    int line_as_term{-1};

    while (read_line(input_file, line))
    {
        if (line.length() == 0)
            continue;

        if (line.rfind("~~:", 0) == 0)
        {
            ++post;
            crumbs.chapters.push_back(Chapter());

            // first read the text number (chapter title)
            post_number = line.substr(3);
            int index = mm_lookup(post_number.c_str(), &term_exists);
            if (!term_exists)
            {
                std::cout << "FAILED!\nencountered '" << post_number << "' which is not in the dictionary!"
                          << std::endl;
                return false;
            }
            if (std::to_string(post) != mm_at(index, nullptr))
            {
                std::cout << "FAILED!\n"
                          << "post_number: " << post_number
                          << "\nencountered '" << mm_at(index, nullptr)
                          << "' but " << post << " expected" << std::endl;
                return false;
            }
            crumbs.chapters.back().title.push_back(index);

            // then read the text's header (chapter subtitle)
            if (!read_line(input_file, line))
            {
                std::cout << "FAILED!\nunexpected end of file within text " << post_number << std::endl;
                return false;
            }
            if (!read_header(line, crumbs.chapters.back()))
            {
                std::cout << "failed to read header for text " << post_number << std::endl;
                return false;
            }
        }



        // TODO stop ignoring hyperlinks without blowing up the dictionary (9 deep prefixes!?!?!)
        //
        // note this would be 12 deep if we have the entire line including the ]~~ !!!
        //
        // when the time comes for 9 deep, instead of assuming entire line exists check for two words
        // instead --> ]~~ and line.substr(3)
        else if (line.rfind("]~~", 0) == 0)
        {
            continue;
        }



        else
        {
            // work around missing chapter info within image references
            if (line.rfind("~~~", 0) == 0)
                line.insert(3, post_number + "_");

            // entire line should exist as a term
            line_as_term = mm_lookup(line.c_str(), &term_exists);
            if (!term_exists)
            {
                std::cout << "FAILED!\nword '" << line
                          << "' not in the dictionary! text: " << post_number << std::endl;
                return false;
            }

            std::function<void (int,
                                std::deque<int>,
                                int,
                                int,
                                bool,
                                IndexTable const &,
                                std::vector<BookWord> &)> add_to_paragraph =
                [&add_to_paragraph]
                (
                    int word,
                    std::deque<int> parents,
                    int parent_start,
                    int index_within_parent,
                    bool referenced,
                    IndexTable const & embedded_words,
                    std::vector<BookWord> & paragraph
                )
                {
                    if (embedded_words[word].size() == 0)
                    {
                        BookWord bw;
                        bw.word = word;
                        bw.ancestors = parents;
                        bw.first_ancestor_start_index = parent_start;
                        bw.index_within_first_ancestor = index_within_parent;
                        bw.referenced = referenced;
                        paragraph.push_back(bw);
                    }
                    else
                    {
                        parent_start = (int) paragraph.size();
                        parents.push_front(word);
                        for (size_t i = 0; i < embedded_words[word].size(); ++i)
                        {
                            add_to_paragraph(embedded_words[word][i],
                                             parents,
                                             parent_start,
                                             i,
                                             referenced,
                                             embedded_words,
                                             paragraph);
                        }
                    }
                };

            // current line as string
            int s_len = 0;
            char const * s = mm_at(line_as_term, &s_len);

            // check for linked text
            if (s_len > 3 && s[0] == '>' && s[1] == '>')
            {
                bool numbers_only = true;
                for (int si = 2; numbers_only && si < s_len; ++si)
                    numbers_only = s[si] >= '0' && s[si] <= '9';

                if (numbers_only)
                {
                    {
                        std::vector<BookWord> link_paragraph;
                        BookWord link_bw;
                        link_bw.word = line_as_term;
                        link_bw.ancestors.clear();
                        link_bw.first_ancestor_start_index = -1;
                        link_bw.index_within_first_ancestor = -1;
                        link_bw.referenced = false;
                        link_paragraph.push_back(link_bw);
                        crumbs.chapters.back().paragraphs.push_back(link_paragraph);
                    }

                    std::vector<int> link_lines;

                    if (!read_link(line_as_term, link_lines))
                    {
                        std::cout << "failed to read link for post: " << post_number << std::endl;
                        return false;
                    }

                    for (int link_line : link_lines)
                    {
                        int ll_s_len = 0;
                        char const * ll_s = mm_at(link_line, &ll_s_len);

                        bool is_link = ll_s_len > 3;
                        is_link = is_link && ll_s[0] == '>' && ll_s[1] == '>';
                        for (int i = 2; is_link && i < s_len; ++i)
                            is_link = s[i] >= '0' && s[i] <= '9';

                        // check for lines that must be added as whole paragraphs
                        if (
                            ll_s_len > 3 &&
                            (
                                is_link ||
                                (ll_s[0] == '~' && ll_s[1] == '~' && ll_s[2] == '~') ||
                                (ll_s[0] == 'h' && ll_s[1] == 't' && ll_s[2] == 't' && ll_s[3] == 'p')
                            )
                        )
                        {
                            std::vector<BookWord> image_paragraph;
                            BookWord image_bw;
                            image_bw.word = link_line;
                            image_bw.ancestors.clear();
                            image_bw.first_ancestor_start_index = -1;
                            image_bw.index_within_first_ancestor = -1;
                            image_bw.referenced = true;
                            image_paragraph.push_back(image_bw);
                            crumbs.chapters.back().paragraphs.push_back(image_paragraph);
                        }
                        else
                        {
                            std::vector<BookWord> paragraph;
                            std::deque<int> parents;
                            add_to_paragraph(link_line, parents, -1, -1, true, embedded_words, paragraph);
                            crumbs.chapters.back().paragraphs.push_back(paragraph);
                        }
                    }

                    // add blank line after linked post
                    crumbs.chapters.back().paragraphs.push_back({});

                    continue; // to next line
                }
            }

            bool is_link = s_len > 3;
            is_link = is_link && s[0] == '>' && s[1] == '>';
            for (int i = 2; is_link && i < s_len; ++i)
                is_link = s[i] >= '0' && s[i] <= '9';

            // check for lines that must be added as whole paragraphs
            if (
                s_len > 3 &&
                ((s[0] == '~' && s[1] == '~' && s[2] == '~') ||
                 is_link || (s[0] == 'h' && s[1] == 't' && s[2] == 't' && s[3] == 'p'))
            )
            {
                std::vector<BookWord> image_paragraph;
                BookWord image_bw;
                image_bw.word = line_as_term;
                image_bw.ancestors.clear();
                image_bw.first_ancestor_start_index = -1;
                image_bw.index_within_first_ancestor = -1;
                image_bw.referenced = false;
                image_paragraph.push_back(image_bw);
                crumbs.chapters.back().paragraphs.push_back(image_paragraph);
            }
            else
            {
                std::vector<BookWord> paragraph;
                std::deque<int> parents;
                add_to_paragraph(line_as_term, parents, -1, -1, false, embedded_words, paragraph);
                crumbs.chapters.back().paragraphs.push_back(paragraph);
            }
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
    if (tokenized_on_comma.size() < 2 || tokenized_on_comma.size() > 3)
    {
        std::cout << "FAILED!\nfailed to parse header line! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split date, time, AM/PM, zoneinfo (time is middle 2 tokens because of AM/PM)
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
    bool found_in_dictionary{false};
    int const dash = mm_lookup("-", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n'-' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const space = mm_lookup(" ", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n' ' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const colon = mm_lookup(":", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n':' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const comma = mm_lookup(",", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n',' is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // finally we can start creating the subtitle
    //
    // start by adding the year
    chapter.subtitle.push_back(mm_lookup(tokenized_date[2].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[2]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // then add the month
    chapter.subtitle.push_back(mm_lookup(tokenized_date[1].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // add the day
    chapter.subtitle.push_back(mm_lookup(tokenized_date[0].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[0]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(space);

    // add the hour
    //
    // but use the 24hr clock (military is the only way)
    std::string hr_as_str = tokenized_time[0];
    if (tokenized_date_time[2] == "PM")
    {
        int hr = std::stoi(hr_as_str);
        hr += 12;
        if (hr > 24)
        {
            std::cout << "FAILED\nam/pm time format but encountered hour: " << hr_as_str << std::endl;
            abort();
        }
        else if (hr == 24)
        {
            hr = 0;
        }
        hr_as_str = std::to_string(hr);
    }
    chapter.subtitle.push_back(mm_lookup(hr_as_str.c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << hr_as_str
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // add a colon
    chapter.subtitle.push_back(colon);

    // add the minute
    chapter.subtitle.push_back(mm_lookup(tokenized_time[1].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_time[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // add another colon
    chapter.subtitle.push_back(mm_lookup(":", nullptr));

    // add the second
    chapter.subtitle.push_back(mm_lookup(tokenized_time[2].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_time[2]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(space);

    // add time zone
    chapter.subtitle.push_back(mm_lookup(tokenized_date_time[3].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date_time[3]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(comma);

    // add message board
    chapter.subtitle.push_back(mm_lookup(tokenized_on_comma[1].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_on_comma[1]
                  << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    if (tokenized_on_comma.size() == 3)
    {
        chapter.subtitle.push_back(space);
        chapter.subtitle.push_back(mm_lookup(tokenized_on_comma[2].c_str(), &found_in_dictionary));
        if (!found_in_dictionary)
        {
            std::cout << "FAILED!\nencountered '" << tokenized_on_comma[2]
                    << "' which is not in the dictionary! text: " << mm_at(chapter.title[0], nullptr)
                    << std::endl;
            return false;
        }
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


bool read_link(int link_as_term, std::vector<int> & lines)
{
    int link_as_str_len = 0;
    char const * link_as_str = mm_at(link_as_term, &link_as_str_len);
    if (link_as_str_len < 3)
        return false;

    ++link_as_str;
    ++link_as_str;

    std::string const & DATA_DIR = Stage1Data::nil.as_data_dir();

    std::string fn = DATA_DIR;
    fn += "/Crumbs/linked_text/";
    fn += link_as_str;
    fn += "/post";

    FILE * link_file = fopen(fn.c_str(), "r");
    if (link_file == 0)
    {
        perror(link_as_str);
        return false;
    }

    lines.clear();
    std::string line;
    bool term_in_dictionary{false};
    int line_as_term{-1};


    while (read_link_line(link_file, line))
    {
        if (line.length() == 0)
            continue;

        // entire line should exist as a term
        line_as_term = mm_lookup(line.c_str(), &term_in_dictionary);
        if (!term_in_dictionary)
        {
            std::cout << "\nterm '" << line
                        << "' not in the dictionary! link: " << link_as_str << std::endl;
            return false;
        }

        lines.push_back(line_as_term);
    }

    fclose(link_file);

    return true;
}


bool read_link_line(FILE * f, std::string & line)
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
