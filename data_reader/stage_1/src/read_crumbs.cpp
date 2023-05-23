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



MATCHABLE(
    ParseStatus,
    Failed,
    Success,
    Finished
)

ParseStatus::Type parse_post_start(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);

ParseStatus::Type parse_linked_post(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);

ParseStatus::Type parse_image(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);

ParseStatus::Type parse_archived_link(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);

ParseStatus::Type parse_hyperlink(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);

ParseStatus::Type parse_body_line(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
);


using ParseFunc = std::function<ParseStatus::Type (std::string const & line,
                                                   bool within_linked_post,
                                                   int & post,
                                                   FILE * input_file,
                                                   Book & crumbs)>;

PROPERTYx1_MATCHABLE(
    ParseFunc, parse_func,

    LineStart,

    Post,
    LinkedPost,
    Image,
    ArchivedLink,
    Hyperlink
)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, Post, parse_func, &parse_post_start)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, LinkedPost, parse_func, &parse_linked_post)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, Image, parse_func, &parse_image)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, ArchivedLink, parse_func, &parse_archived_link)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, Hyperlink, parse_func, &parse_hyperlink)
MATCHABLE_NIL_PROPERTY_VALUE(LineStart, parse_func, &parse_body_line)



MATCHABLE(
    LineStartCode,
    esc__tld__tld__cln_,
    esc__gt__gt_,
    esc__tld__tld__tld_,
    esc__sbr__tld__tld_,
    http
)


// lookup tables for converting between LineStart and LineStartCode
matchable::MatchBox<LineStart::Type, LineStartCode::Type> const LineStart_as_code {
    { LineStart::Post::grab(), LineStartCode::esc__tld__tld__cln_::grab() },
    { LineStart::LinkedPost::grab(), LineStartCode::esc__gt__gt_::grab() },
    { LineStart::Image::grab(), LineStartCode::esc__tld__tld__tld_::grab() },
    { LineStart::ArchivedLink::grab(), LineStartCode::esc__sbr__tld__tld_::grab() },
    { LineStart::Hyperlink::grab(), LineStartCode::http::grab() },
};
matchable::MatchBox<LineStartCode::Type, LineStart::Type> const code_as_LineStart {
    { LineStartCode::esc__tld__tld__cln_::grab(), LineStart::Post::grab() },
    { LineStartCode::esc__gt__gt_::grab(), LineStart::LinkedPost::grab() },
    { LineStartCode::esc__tld__tld__tld_::grab(), LineStart::Image::grab() },
    { LineStartCode::esc__sbr__tld__tld_::grab(), LineStart::ArchivedLink::grab() },
    { LineStartCode::http::grab(), LineStart::Hyperlink::grab() },
};


bool read_crumbs(
    std::string const & DATA_DIR,
    Book & book
);

bool read_chapter(
    FILE * input_file,
    Book & crumbs,
    int & chapter
);

bool read_line(FILE * f, std::string & line);

bool read_header(std::string const & header, Chapter & chapter);

void tokenize(std::string const & str, char const delim, std::vector<std::string> & out);

bool read_link(std::string const & link_number_as_string, std::vector<std::string> & lines);

bool read_link_line(FILE * f, std::string & line);

void add_to_paragraph(
    int word,
    std::deque<int> parents,
    int parent_start,
    int index_within_parent,
    bool referenced,
    std::vector<BookWord> & paragraph
);



ParseStatus::Type parse_post_start(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    if (within_linked_post)
    {
        std::cout << "FAILED!\nparse_post_start() called within linked post, post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    ++post;
    crumbs.chapters.push_back(Chapter());

    // first read the post number (chapter title)
    std::string post_number = line.substr(3);
    bool term_exists{false};
    int index = mm_lookup(post_number.c_str(), &term_exists);
    if (!term_exists)
    {
        std::cout << "FAILED!\nencountered '" << post_number << "' which is not in the dictionary!"
                    << std::endl;
        return ParseStatus::Failed::grab();
    }
    if (std::to_string(post) != mm_at(index, nullptr))
    {
        std::cout << "FAILED!\n"
                    << "post_number: " << post_number
                    << "\nencountered '" << mm_at(index, nullptr)
                    << "' but " << post << " expected" << std::endl;
        return ParseStatus::Failed::grab();
    }
    crumbs.chapters.back().title.push_back(index);

    // then read the post's header (chapter subtitle)
    std::string subtitle_line;
    if (!read_line(input_file, subtitle_line))
    {
        std::cout << "FAILED!\nunexpected end of file within post " << post_number << std::endl;
        return ParseStatus::Failed::grab();
    }
    if (!read_header(subtitle_line, crumbs.chapters.back()))
    {
        std::cout << "failed to read header for post " << post_number << std::endl;
        return ParseStatus::Failed::grab();
    }

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_linked_post(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    std::vector<BookWord> link_paragraph;

    // split up link line into start code, number, and any extra chars (if any)
    LineStartCode::Type start_code = LineStart_as_code.at(LineStart::LinkedPost::grab());
    if (line.size() <= start_code.as_string().size())
    {
        std::cout << "FAILED\nLinked post start code but missing number! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }
    std::string line_without_start_code = line.substr(start_code.as_string().size());
    std::string link_number_as_string;
    bool is_link{true};
    size_t i = 0;
    for (; is_link && i < line_without_start_code.size(); ++i)
    {
        is_link = line_without_start_code[i] >= '0' && line_without_start_code[i] <= '9';
        if (is_link)
            link_number_as_string += line_without_start_code[i];
        else
            --i;
    }
    std::string extra_chars;
    for (; i < line_without_start_code.size(); ++i)
        extra_chars += line_without_start_code[i];

    // line starts with link's start code but is not a link! (parse entire line as normal body line)
    if (link_number_as_string.empty() || (extra_chars.size() > 0 && line_without_start_code[i] != ' '))
        return parse_body_line(line, within_linked_post, post, input_file, crumbs);

    // first create a word for the start code
    bool term_exists{false};
    int code_as_term = mm_lookup(start_code.as_string().c_str(), &term_exists);
    if (!term_exists)
    {
        std::cout << "FAILED!\nstart code '" << start_code
                    << "' not in the dictionary! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }
    BookWord code_bw;
    code_bw.word = code_as_term;
    code_bw.ancestors.clear();
    code_bw.first_ancestor_start_index = -1;
    code_bw.index_within_first_ancestor = -1;
    code_bw.referenced = within_linked_post;
    link_paragraph.push_back(code_bw);

    // then create term for the number part of the link
    int number_as_term = mm_lookup(link_number_as_string.c_str(), &term_exists);
    if (!term_exists)
    {
        std::cout << "FAILED!\nlink number: '" << link_number_as_string
                    << "' not in the dictionary! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }
    BookWord number_bw;
    number_bw.word = number_as_term;
    number_bw.ancestors.clear();
    number_bw.first_ancestor_start_index = -1;
    number_bw.index_within_first_ancestor = -1;
    number_bw.referenced = within_linked_post;
    link_paragraph.push_back(number_bw);

    // then create terms for each extra char (if any)
    for (char ch : extra_chars)
    {
        int ch_as_term = mm_lookup(std::string(1, ch).c_str(), &term_exists);
        if (!term_exists)
        {
            std::cout << "FAILED!\nchar '" << ch << "' not in the dictionary! post: " << post << std::endl;
            return ParseStatus::Failed::grab();
        }
        BookWord ch_bw;
        ch_bw.word = ch_as_term;
        ch_bw.ancestors.clear();
        ch_bw.first_ancestor_start_index = -1;
        ch_bw.index_within_first_ancestor = -1;
        ch_bw.referenced = within_linked_post;
        link_paragraph.push_back(ch_bw);
    }

    // add the link line paragraph to the last chapter
    crumbs.chapters.back().paragraphs.push_back(link_paragraph);

    // no recursively linked content!
    // links within linked text exist but as single lines only (without content)
    // links with anything other than numbers following the start code are considered invalid (no content)
    if (!within_linked_post && extra_chars.empty())
    {
        std::vector<std::string> content_lines;
        if (!read_link(link_number_as_string, content_lines))
        {
            std::cout << "failed to read link content for link '" << link_number_as_string
                      << "' within post: " << post << std::endl;
            ParseStatus::Failed::grab();
        }

        for (std::string const & content_line : content_lines)
        {
            // get content line's start code
            LineStart::Type content_line_start;
            for (auto code : LineStartCode::variants())
            {
                if (content_line.rfind(code.as_string(), 0) == 0)
                {
                    content_line_start = code_as_LineStart.at(code);
                    break;
                }
            }

            ParseStatus::Type ret = content_line_start.as_parse_func()(
                                        content_line,
                                        true, // within_linked_post!
                                        post,
                                        input_file,
                                        crumbs
                                    );

            if (ret == ParseStatus::Failed::grab())
            {
                std::cout << "read_chapter() --> failed to content for link '"
                        << link_number_as_string << "' within post: " << post << std::endl;
                return ret;
            }
        }
    }

    // add blank line after linked content
    crumbs.chapters.back().paragraphs.push_back({});

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_image(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    (void) input_file;

    std::string image_line = line;
    std::string post_as_string = std::to_string(post);

    LineStartCode::Type start_code = LineStart_as_code.at(LineStart::Image::grab());
    if (image_line.size() <= start_code.as_string().size())
    {
        std::cout << "FAILED\nimage line is empty! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    if (!within_linked_post)
        image_line.insert(start_code.as_string().size(), post_as_string + "_");

    // entire line should exist as a term
    bool term_exists{false};
    int line_as_term = mm_lookup(image_line.c_str(), &term_exists);
    if (!term_exists)
    {
        std::cout << "FAILED!\nimage '" << image_line
                  << "' not in the dictionary! post: " << post_as_string << std::endl;
        return ParseStatus::Failed::grab();
    }

    std::vector<BookWord> image_paragraph;
    BookWord image_bw;
    image_bw.word = line_as_term;
    image_bw.ancestors.clear();
    image_bw.first_ancestor_start_index = -1;
    image_bw.index_within_first_ancestor = -1;
    image_bw.referenced = within_linked_post;
    image_paragraph.push_back(image_bw);
    crumbs.chapters.back().paragraphs.push_back(image_paragraph);

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_archived_link(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    (void) input_file;

    LineStartCode::Type start_code = LineStart_as_code.at(LineStart::ArchivedLink::grab());
    if (line.size() <= start_code.as_string().size())
    {
        std::cout << "FAILED\narchived link is empty! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    std::string link_line = line.substr(start_code.as_string().size());
    return parse_hyperlink(link_line, within_linked_post, post, input_file, crumbs);
}


ParseStatus::Type parse_hyperlink(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    (void) input_file;

    std::vector<BookWord> paragraph;
    bool term_exists{false};
    LineStartCode::Type start_code = LineStart_as_code.at(LineStart::Hyperlink::grab());
    if (line.size() <= start_code.as_string().size())
    {
        std::cout << "FAILED\nlink is empty! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // first create a word for the start code
    {
        int code_as_term = mm_lookup(start_code.as_string().c_str(), &term_exists);
        if (!term_exists)
        {
            std::cout << "FAILED!\nstart code '" << start_code
                        << "' not in the dictionary! post: " << post << std::endl;
            return ParseStatus::Failed::grab();
        }
        BookWord code_bw;
        code_bw.word = code_as_term;
        code_bw.ancestors.clear();
        code_bw.first_ancestor_start_index = -1;
        code_bw.index_within_first_ancestor = -1;
        code_bw.referenced = within_linked_post;
        paragraph.push_back(code_bw);
    }

    // then create words for each character in the link (links no longer exist as whole words!)
    for (auto ch : line.substr(start_code.as_string().size()))
    {
        int ch_as_term = mm_lookup(std::string(1, ch).c_str(), &term_exists);
        if (!term_exists)
        {
            std::cout << "FAILED!\ncharacter '" << ch
                      << "' not in the dictionary! post: " << post << std::endl;
            return ParseStatus::Failed::grab();
        }
        BookWord ch_bw;
        ch_bw.word = ch_as_term;
        ch_bw.ancestors.clear();
        ch_bw.first_ancestor_start_index = -1;
        ch_bw.index_within_first_ancestor = -1;
        ch_bw.referenced = within_linked_post;
        paragraph.push_back(ch_bw);
    }

    // finally add the paragraph to the last chapter
    crumbs.chapters.back().paragraphs.push_back(paragraph);

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_body_line(
    std::string const & line,
    bool within_linked_post,
    int & post,
    FILE * input_file,
    Book & crumbs
)
{
    (void) input_file;

    if (line.empty())
    {
        crumbs.chapters.back().paragraphs.push_back({});
        return ParseStatus::Success::grab();
    }

    // entire line should exist as a term
    bool term_exists{false};
    int line_as_term = mm_lookup(line.c_str(), &term_exists);
    if (!term_exists)
    {
        std::cout << "FAILED!\nword '" << line
                    << "' not in the dictionary! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    std::vector<BookWord> paragraph;
    std::deque<int> parents;
    add_to_paragraph(line_as_term, parents, -1, -1, within_linked_post, paragraph);
    crumbs.chapters.back().paragraphs.push_back(paragraph);

    return ParseStatus::Success::grab();
}


bool read_crumbs(SerialTask::Type task)
{
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

    task.set_goal(q_files.size());
    task.set_progress(0);

    FILE * q_file{nullptr};
    int post = 0;
    while (!q_files.empty())
    {
        ++task.as_mutable_progress();
        SerialTask::check_progress(task);

        q_file = fopen(q_files.top().entry.path().c_str(), "r");
        if (q_file == 0)
        {
            perror(q_files.top().entry.path().c_str());
            exit(1);
        }

        ok = read_chapter(q_file, book, post);
        fclose(q_file);
        if (!ok)
            return false;

        q_file = nullptr;
        q_files.pop();
    }
    return true;
}


bool read_chapter(
    FILE * input_file,
    Book & crumbs,
    int & post
)
{
    std::string line;
    while (read_line(input_file, line))
    {
        // get line start code
        LineStart::Type line_start;
        for (auto code : LineStartCode::variants())
        {
            if (line.rfind(code.as_string(), 0) == 0)
            {
                line_start = code_as_LineStart.at(code);
                break;
            }
        }
        ParseStatus::Type ret = line_start.as_parse_func()(
                                    line,
                                    false,
                                    post,
                                    input_file,
                                    crumbs
                                );

        if (ret == ParseStatus::Failed::grab())
        {
            std::cout << "read_chapter() --> failed to parse post: " << post << std::endl;
            return false;
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
        std::cout << "FAILED!\nfailed to parse header line! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split date, time, AM/PM, zoneinfo (time is middle 2 tokens because of AM/PM)
    std::vector<std::string> tokenized_date_time;
    tokenize(tokenized_on_comma[0], ' ', tokenized_date_time);
    if (tokenized_date_time.size() != 4)
    {
        std::cout << "FAILED!\nfailed to parse date/time stamp! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split date into day month year
    std::vector<std::string> tokenized_date;
    tokenize(tokenized_date_time[0], '-', tokenized_date);
    if (tokenized_date.size() != 3)
    {
        std::cout << "FAILED!\nfailed to parse date stamp! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // then split time into hour min sec
    std::vector<std::string> tokenized_time;
    tokenize(tokenized_date_time[1], ':', tokenized_time);
    if (tokenized_time.size() != 3)
    {
        std::cout << "FAILED!\nfailed to parse time stamp! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    // cache repeatedly used indexes
    bool found_in_dictionary{false};
    int const dash = mm_lookup("-", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n'-' is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const space = mm_lookup(" ", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n' ' is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const colon = mm_lookup(":", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n':' is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }
    int const comma = mm_lookup(",", &found_in_dictionary);
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\n',' is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // then add the month
    chapter.subtitle.push_back(mm_lookup(tokenized_date[1].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[1]
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(dash);

    // add the day
    chapter.subtitle.push_back(mm_lookup(tokenized_date[0].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date[0]
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(space);

    // add time zone
    chapter.subtitle.push_back(mm_lookup(tokenized_date_time[3].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_date_time[3]
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
                  << std::endl;
        return false;
    }

    chapter.subtitle.push_back(comma);

    // add message board
    chapter.subtitle.push_back(mm_lookup(tokenized_on_comma[1].c_str(), &found_in_dictionary));
    if (!found_in_dictionary)
    {
        std::cout << "FAILED!\nencountered '" << tokenized_on_comma[1]
                  << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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
                    << "' which is not in the dictionary! post: " << mm_at(chapter.title[0], nullptr)
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


bool read_link(std::string const & link_number_as_string, std::vector<std::string> & lines)
{
    std::string const & DATA_DIR = Stage1Data::nil.as_data_dir();

    std::string fn = DATA_DIR;
    fn += "/Crumbs/linked_text/";
    fn += link_number_as_string;
    fn += "/post";

    FILE * link_file = fopen(fn.c_str(), "r");
    if (link_file == 0)
    {
        perror(link_number_as_string.c_str());
        return false;
    }

    lines.clear();
    std::string line;
    while (read_link_line(link_file, line))
    {
        if (line.length() == 0)
            continue;

        lines.push_back(line);
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


void add_to_paragraph(
    int word,
    std::deque<int> parents,
    int parent_start,
    int index_within_parent,
    bool referenced,
    std::vector<BookWord> & paragraph
)
{
    IndexTable const & embedded_words = Stage1Data::nil.as_embedded_words();

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
            add_to_paragraph(
                embedded_words[word][i],
                parents,
                parent_start,
                i,
                referenced,
                paragraph
            );
        }
    }
};
