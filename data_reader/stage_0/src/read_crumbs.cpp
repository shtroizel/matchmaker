#include <read_crumbs.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <queue>

#include <matchable/matchable.h>
#include <matchmaker/data_reader_common.h>

#include <Stage0Data.h>


MATCHABLE(
    ParseStatus,
    Failed,
    Success,
    Finished
)

ParseStatus::Type parse_post_start(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);

ParseStatus::Type parse_linked_post(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);

ParseStatus::Type parse_image(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);

ParseStatus::Type parse_link(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);

ParseStatus::Type parse_body_line(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);


using ParseFunc = std::function<ParseStatus::Type (SerialTask::Type,
                                                   bool within_linked_post,
                                                   std::string const & line,
                                                   Stage0Data::word_attribute::Flags const &,
                                                   int &,
                                                   FILE *,
                                                   FILE *,
                                                   std::map<std::string, Stage0Data::Encountered::Type> &)>;

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
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, ArchivedLink, parse_func, &parse_link)
MATCHABLE_VARIANT_PROPERTY_VALUE(LineStart, Hyperlink, parse_func, &parse_link)
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
    SerialTask::Type task,
    Stage0Data::word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
    int & post,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);

bool read_crumbs_line(
    SerialTask::Type task,
    FILE * f,
    std::string & line,
    Stage0Data::word_attribute::Flags & attributes
);

bool read_link_line(
    FILE * f,
    std::string & line,
    Stage0Data::word_attribute::Flags & attributes
);

void find_quotes(std::string const & line, std::vector<std::string> & quotes);

void cherry_pick_parens(
    char left_paren,
    char right_paren,
    std::string & line,
    std::vector<std::string> & cherries
);

bool strip_word(char ch, std::string & word);

void trim_word(char ch, std::string & word);

void tokenize(std::string const & str, char const delim, std::vector<std::string> & out);

bool add_embedded(
    SerialTask::Type task,
    int post,
    int linked_text,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & attributes,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
);



void read_crumbs(SerialTask::Type task)
{
    task.set_goal(0);
    task.set_progress(0);

    int post = 0;
    FILE * q_file = nullptr;

    std::string const crumbs_path = Stage0Data::book_vocab_dir() + "/Crumbs";
    std::filesystem::create_directory(crumbs_path);
    std::string const crumbs_vocab = Stage0Data::book_vocab_dir() + "/Crumbs/vocabulary";
    FILE * vocab_file = fopen(crumbs_vocab.c_str(), "w");
    if (nullptr == vocab_file)
    {
        perror(crumbs_vocab.c_str());
        exit(1);
    }

    std::map<std::string, Stage0Data::Encountered::Type> encounters;

    // add all line start codes...
    for (auto code : LineStartCode::variants())
        Stage0Data::add_book_vocab(
            code.as_string(),
            Stage0Data::word_attribute::Flags(),
            task,
            vocab_file,
            encounters
        );

    // the recursive_directory_iterator provides the files in random order, so first sort
    // them by filename using the priority q
    std::priority_queue<
        QFile,
        std::vector<QFile>,
        std::less<std::vector<QFile>::value_type>
    > q_files;
    for (
        auto const & entry :
        std::filesystem::recursive_directory_iterator(Stage0Data::data_dir() + "/Crumbs/text")
    )
    {
        if (entry.is_regular_file())
        {
            q_files.push({entry});
            task.as_mutable_goal() += std::filesystem::file_size(entry);
        }
    }

    while (!q_files.empty())
    {
        // std::cout << "processing file: " << q_files.top().entry.path() << std::endl;
        q_file = fopen(q_files.top().entry.path().c_str(), "r");
        if (q_file == 0)
        {
            perror(q_files.top().entry.path().c_str());
            abort();
        }
        Stage0Data::word_attribute::Flags base_attributes;
        bool ok = read_crumbs(task, base_attributes, q_file, vocab_file, post, encounters);
        fclose(q_file);
        if (!ok)
        {
            std::cout << "failed to parse file: " << q_files.top().entry.path() << std::endl;
            fclose(vocab_file);
            abort();
        }
        q_file = nullptr;
        q_files.pop();
    }

    fclose(vocab_file);
    vocab_file = nullptr;
}



bool read_crumbs(
    SerialTask::Type task,
    Stage0Data::word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
    int & post,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    Stage0Data::word_attribute::Flags attributes;
    std::string line;
    std::string code;

    while (true)
    {
        attributes = base_attributes;

        if (!read_crumbs_line(task, input_file, line, attributes))
            break;

        if (line.empty())
            continue;

        if (attributes.is_set(Stage0Data::word_attribute::unmatchable_symbols::grab()))
        {
            std::cout << "encountered unmatchable symbols! text: " << post << " line: " << line << std::endl;
            return false;
        }
        if (attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
        {
            std::cout << "encountered invisible_ascii! text: " << post << " line: " << line << std::endl;
            return false;
        }
        attributes = base_attributes;

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
                                    task,
                                    false,
                                    line,
                                    attributes,
                                    post,
                                    input_file,
                                    vocab_file,
                                    encounters
                                );

        if (ret == ParseStatus::Failed::grab())
            return false;

        if (ret == ParseStatus::Finished::grab())
            break;

        SerialTask::check_progress(task);
    }
    return true;
}


ParseStatus::Type parse_post_start(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & const_line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    ++post;

    if (within_linked_post)
    {
        std::cout << "parse_post_start() called within linked content! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    std::string line = const_line;
    std::string post_as_str = line.substr(3);
    if (std::to_string(post) != post_as_str)
    {
        std::cout << "encountered q post " << post_as_str << " but "
                    << post << " expected! aborting...\n";
        return ParseStatus::Failed::grab();
    }

    Stage0Data::word_attribute::Flags attributes = base_attributes;
    Stage0Data::add_book_vocab(post_as_str, attributes, task, vocab_file, encounters);

    // read date/time line
    if (!read_crumbs_line(task, input_file, line, attributes))
        return ParseStatus::Finished::grab();

    if (attributes.is_set(Stage0Data::word_attribute::unmatchable_symbols::grab()))
    {
        std::cout << "encountered unmatchable symbols! post: " << post << " line: " << line << std::endl;
        return ParseStatus::Failed::grab();
    }
    if (attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
    {
        std::cout << "encountered invisible_ascii! post: " << post << " line: " << line << std::endl;
        return ParseStatus::Failed::grab();
    }

    // first split date/time from message board
    std::vector<std::string> tokenized_on_comma;
    tokenize(line, ',', tokenized_on_comma);
    if (tokenized_on_comma.size() != 2 && tokenized_on_comma.size() != 3)
    {
        std::cout << "failed to parse header line! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // then split date, time, zoneinfo (time is middle 2 tokens because of AM/PM)
    std::vector<std::string> tokenized_date_time;
    tokenize(tokenized_on_comma[0], ' ', tokenized_date_time);
    if (tokenized_date_time.size() != 4)
    {
        std::cout << "failed to parse date/time stamp! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // then split date into day month year
    std::vector<std::string> tokenized_date;
    tokenize(tokenized_date_time[0], '-', tokenized_date);
    if (tokenized_date.size() != 3)
    {
        std::cout << "failed to parse date stamp! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // then split time into hour min sec
    std::vector<std::string> tokenized_time;
    tokenize(tokenized_date_time[1], ':', tokenized_time);
    if (tokenized_time.size() != 3)
    {
        std::cout << "failed to parse time stamp! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // guarantee 2 digit hour
    if (tokenized_time[0].length() == 1)
    {
        tokenized_time[0].insert(0, 1, '0');
    }

    // use 24 hr clock (military is the only way)
    else if (tokenized_date_time[2] == "PM")
    {
        int hr = std::stoi(tokenized_time[0]);
        hr += 12;
        tokenized_time[0] = std::to_string(hr);
    }

    // now add them all including all tokens & symbols
    std::vector<std::string> terms{
        " ", "-", ":", ",", "=", "/", "%", "$", "\"", "|", "^", "!", "?", ".", "(", ")",
        "[", "]", "#", ";", tokenized_date_time[2], tokenized_date_time[3], tokenized_on_comma[1]
    };
    if (tokenized_on_comma.size() > 2)
        terms.push_back(tokenized_on_comma[2]);

    terms.insert(terms.end(), tokenized_date.begin(), tokenized_date.end());
    terms.insert(terms.end(), tokenized_time.begin(), tokenized_time.end());
    for (auto const & word : terms)
        Stage0Data::add_book_vocab(word, attributes, task, vocab_file, encounters);

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_linked_post(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    // first add the link's number and extra chars if any (the start code is already added)
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
    Stage0Data::add_book_vocab(link_number_as_string, base_attributes, task, vocab_file, encounters);
    std::string extra_chars;
    for (; i < line_without_start_code.size(); ++i)
        extra_chars += line_without_start_code[i];

    // line starts with link's start code but is not a link! (parse entire line as normal body line)
    if (link_number_as_string.empty() || (extra_chars.size() > 0 && line_without_start_code[i] != ' '))
        return parse_body_line(task, within_linked_post, line, base_attributes,
                               post, input_file, vocab_file, encounters);

    // add the link's extra chars but do not try to add any of the link's content (invalid link)
    if (extra_chars.size())
        return parse_body_line(task, within_linked_post, extra_chars, base_attributes,
                               post, input_file, vocab_file, encounters);

    // recursively linked posts are without content!
    if (within_linked_post)
        return ParseStatus::Success::grab();

    // now add the link's content
    std::string const link_dir = Stage0Data::data_dir() + "/Crumbs/linked_text/" + link_number_as_string;
    std::string const link_path = link_dir + "/post";

    FILE * link_file = fopen(link_path.c_str(), "r");
    if (nullptr == link_file)
    {
        std::cout << "post: " << post << " --> failed to open file: " << link_path << std::endl;
        return ParseStatus::Failed::grab();
    }

    Stage0Data::word_attribute::Flags content_attributes;
    std::string content_line;
    while (true)
    {
        content_attributes = base_attributes;
        if (!read_link_line(link_file, content_line, content_attributes))
            break;

        if (content_line.empty())
            continue;

        if (content_attributes.is_set(Stage0Data::word_attribute::unmatchable_symbols::grab()))
        {
            std::cout << "encountered unmatchable symbols in linked post: " << link_path << std::endl;
            fclose(link_file);
            return ParseStatus::Failed::grab();
        }
        if (content_attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
        {
            std::cout << "encountered invisible_ascii in linked post: " << link_path << std::endl;
            fclose(link_file);
            return ParseStatus::Failed::grab();
        }

        // get line start code
        LineStart::Type line_start;
        for (auto code : LineStartCode::variants())
        {
            if (content_line.rfind(code.as_string(), 0) == 0)
            {
                line_start = code_as_LineStart.at(code);
                break;
            }
        }

        ParseStatus::Type ret = line_start.as_parse_func()(
                                    task,
                                    true, // within linked post!
                                    content_line,
                                    content_attributes,
                                    post,
                                    input_file,
                                    vocab_file,
                                    encounters
                                );

        if (ret == ParseStatus::Finished::grab())
        {
            std::cout << "unexpected \"Finished\" status when parsing linked post: "
                      << link_path << std::endl;
            fclose(link_file);
            return ParseStatus::Failed::grab();
        }

        if (ret == ParseStatus::Failed::grab())
        {
            std::cout << "failed to parse linked post: " << link_path << std::endl;
            fclose(link_file);
            return ParseStatus::Failed::grab();
        }
    }
    fclose(link_file);

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_image(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    (void) input_file;

    LineStartCode::Type start_code = LineStart_as_code.at(LineStart::Image::grab());
    if (line.size() <= start_code.as_string().size())
    {
        std::cout << "FAILED\nImage start code but missing name! post: " << post << std::endl;
        return ParseStatus::Failed::grab();
    }

    // work around missing chapter info within image references
    std::string l = line;
    if (!within_linked_post)
        l.insert(start_code.as_string().size(), std::to_string(post) + "_");

    Stage0Data::add_book_vocab(l, base_attributes, task, vocab_file, encounters);

    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_link(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    (void) post;
    (void) input_file;
    (void) within_linked_post;

    for (char ch : line)
        Stage0Data::add_book_vocab(std::string(1, ch), base_attributes, task, vocab_file, encounters);
    return ParseStatus::Success::grab();
}


ParseStatus::Type parse_body_line(
    SerialTask::Type task,
    bool within_linked_post,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & base_attributes,
    int & post,
    FILE * input_file,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    (void) input_file;
    (void) within_linked_post;

    // add entire line as a phrase
    Stage0Data::word_attribute::Flags att = base_attributes;
    att.set(Stage0Data::word_attribute::phrase::grab());
    Stage0Data::add_book_vocab(line, att, task, vocab_file, encounters);

    // add all of the line's embedded words
    if (!add_embedded(task, post, -1, line, base_attributes, vocab_file, encounters))
        return ParseStatus::Failed::grab();

    return ParseStatus::Success::grab();
}


bool read_crumbs_line(
    SerialTask::Type task,
    FILE * f,
    std::string & line,
    Stage0Data::word_attribute::Flags & attributes
)
{
    line.clear();

    int ch;
    while (true)
    {
        ch = fgetc(f);
        ++task.as_mutable_progress();
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        Stage0Data::update_word_attribute(attributes, ch);
        line += (char) ch;
    }

    return true;
}



bool read_link_line(
    FILE * f,
    std::string & line,
    Stage0Data::word_attribute::Flags & attributes
)
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

        Stage0Data::update_word_attribute(attributes, ch);
        line += (char) ch;
    }

    return true;
}



void find_quotes(std::string const & line, std::vector<std::string> & quotes)
{
    bool within_quote = false;
    std::vector<std::pair<int, int>> quote_locations;
    for (int i = 0; i < (int) line.length(); ++i)
    {
        if (line[i] == '"')
        {
            if (!within_quote) // opening quote
                quote_locations.push_back(std::make_pair(i, i));
            else // closing quote
                quote_locations.back().second = i;
            within_quote = !within_quote;
        }
    }

    for (int i = (int) quote_locations.size(); i-- > 0;)
    {
        quotes.insert(quotes.begin(), std::string());
        for (int j = quote_locations[i].first + 1; j < quote_locations[i].second; ++j)
            quotes.front() += line[j];
    }
}



void cherry_pick_parens(
    char left_paren,
    char right_paren,
    std::string & line,
    std::vector<std::string> & cherries
)
{
    std::vector<std::pair<int, int>> cherry_locations;
    int level = 0;
    for (int i = 0; i < (int) line.length(); ++i)
    {
        if (line[i] == left_paren)
        {
            ++level;
            if (level == 1)
                cherry_locations.push_back(std::make_pair(i, i));
        }
        else if (line[i] == right_paren)
        {
            --level;
            if (level == 0)
                cherry_locations.back().second = i;
        }
    }

    for (int i = (int) cherry_locations.size(); i-- > 0;)
    {
        cherries.insert(cherries.begin(), std::string());
        for (int j = cherry_locations[i].first + 1; j < cherry_locations[i].second; ++j)
            cherries.front() += line[j];

        line.erase(cherry_locations[i].first, cherry_locations[i].second - cherry_locations[i].first + 1);
    }
}



bool strip_word(char ch, std::string & word)
{
    std::string stripped;
    stripped.reserve(word.size());
    for (size_t i = 0; i < word.length(); ++i)
        if (word[i] != ch)
            stripped += word[i];

    if (word.length() != stripped.length())
    {
        word = stripped;
        return true;
    }
    return false;
}



void trim_word(char ch, std::string & word)
{
    int count = 0;
    for (int i = 0; i < (int) word.length(); ++i)
        if (word[i] == ch)
            ++count;

    int end_count = 0;
    for (int i = (int) word.length(); i-- > 0;)
        if (word[i] == ch)
            ++end_count;
        else
            break;

    int begin_count = 0;
    for (int i = 0; i < (int) word.length(); ++i)
        if (word[i] == ch)
            ++begin_count;
        else
            break;

    if (count == end_count + begin_count)
    {
        word.erase(word.length() - end_count, end_count);
        word.erase(0, begin_count);
    }
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



bool add_embedded(
    SerialTask::Type task,
    int post,
    int linked_text,
    std::string const & line,
    Stage0Data::word_attribute::Flags const & attributes,
    FILE * vocab_file,
    std::map<std::string, Stage0Data::Encountered::Type> & encounters
)
{
    {
        std::vector<std::string> tokenized_on_space;
        tokenize(line, ' ', tokenized_on_space);
        for (auto & token : tokenized_on_space)
        {
            // check for use of 'single quotes' (apostrophes (') misused as quotes (") are banned)
            if (token != "'Tis" && token != "o'" && token != "d'etat" &&
                    token.rfind("'90s") != 0 && token != "'em" &&
                    (token[0] == '\'' ||
                    (token.length() >= 2 &&
                        token[token.length() - 1] == '\'' &&
                        token[token.length() - 2] != 's' &&
                        token[token.length() - 2] != 'S' &&
                        token[token.length() - 2] != 'n' &&
                        token[token.length() - 2] != 'N')))
            {
                std::cout << "\nencountered apostrophe misused as \""
                          << "\n           text: " << post
                          << "\n    linked text: " << linked_text
                          << "\n           line: " << line << std::endl;
                return false;
            }

            if (token.rfind("http", 0) == 0)
            {
                Stage0Data::add_book_vocab(token, attributes, task, vocab_file, encounters);
            }
            else
            {
                strip_word('"', token);
                strip_word('!', token);
                strip_word('?', token);
                strip_word(',', token);
                strip_word(';', token);
                strip_word('(', token);
                strip_word(')', token);
                strip_word('=', token);
                trim_word('.', token);
                trim_word(':', token);

                // strip '#', '+' and '/' when they are used as quotes
                if (token.length() > 2)
                {
                    if ((token[0] == '#' && token[token.length() - 1] == '#') ||
                            (token[0] == '+' && token[token.length() - 1] == '+') ||
                            (token[0] == '/' && token[token.length() - 1] == '/'))
                        token = token.substr(1, token.length() - 2);
                }

                // remove '-->' from the front of words
                if (token.length() > 3 && token.rfind("-->", 0) == 0)
                    token = token.substr(3);

                // trim leading '/' chars
                while (token.length() > 1 && token[0] == '/')
                    token = token.substr(1);

                // split on '/'
                std::vector<std::string> tokenized_on_slash;
                tokenize(token, '/', tokenized_on_slash);

                // use original token when just one entry (preserve words like w/)
                if (tokenized_on_slash.size() == 1)
                {
                    tokenized_on_slash.clear();
                    tokenized_on_slash.push_back(token);
                }

                for (auto & slash_token : tokenized_on_slash)
                {
                    // strip words with only left or only right brackets (must have both)
                    bool has_left_bracket = false;
                    bool has_right_bracket = false;
                    for (int i = 0; i < (int) slash_token.length(); ++i)
                    {
                        if (slash_token[i] == '[')
                        {
                            has_left_bracket = true;
                            if (has_right_bracket)
                                break;
                        }
                        else if (slash_token[i] == ']')
                        {
                            has_right_bracket = true;
                            if (has_left_bracket)
                                break;
                        }
                    }
                    if (has_left_bracket && !has_right_bracket)
                        strip_word('[', slash_token);
                    else if (!has_left_bracket && has_right_bracket)
                        strip_word(']', slash_token);

                    Stage0Data::add_book_vocab(slash_token, attributes, task, vocab_file, encounters);

                    // split on '='
                    std::vector<std::string> tokenized_on_eq;
                    tokenize(slash_token, '=', tokenized_on_eq);
                    for (auto & eq_token : tokenized_on_eq)
                        Stage0Data::add_book_vocab(eq_token, attributes, task, vocab_file, encounters);

                    // for words with brackets, add version of word without brackets
                    bool had_brackets = strip_word('[', slash_token);
                    had_brackets = strip_word(']', slash_token) || had_brackets;
                    if (had_brackets)
                        Stage0Data::add_book_vocab(slash_token, attributes, task, vocab_file, encounters);
                }
            }
//                         std::cout << "adding word: " << token << std::endl;
        }
    }

    // process groups of words (phrases)
    auto process_phrases =
        [&](std::vector<std::string> const & phrases)
        {
            for (auto const & phrase : phrases)
            {
                // first add entire quotation (without quotes but could be multiple words)
                bool has_space = false;
                for (int i = 0; !has_space && i < (int) phrase.length(); ++i)
                    if (phrase[i] == ' ')
                        has_space = true;

                Stage0Data::word_attribute::Flags att = attributes;
                if (has_space)
                    att.set(Stage0Data::word_attribute::phrase::grab());

                Stage0Data::add_book_vocab(phrase, att, task, vocab_file, encounters);
                // std::cout << "adding phrase: " << phrase << std::endl;
            }
        };

    // phrases created by quotes
    {
        std::vector<std::string> quotes;
        find_quotes(line, quotes);
        process_phrases(quotes);
    }

    // phrases created by parens
    std::function<void (std::string, char, char)> process_paren_phrases =
        [&process_phrases, &process_paren_phrases](
            std::string line,
            char left_paren,
            char right_paren
        )
        {
            std::vector<std::string> phrases;
            cherry_pick_parens(left_paren, right_paren, line, phrases);
            process_phrases(phrases);
            for (auto phrase : phrases)
                process_paren_phrases(phrase, left_paren, right_paren);
        };
    process_paren_phrases(line, '(', ')');
    process_paren_phrases(line, '[', ']');

    return true;
}
