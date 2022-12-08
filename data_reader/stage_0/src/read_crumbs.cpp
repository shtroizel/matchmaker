/*
Copyright (c) 2020-2022, shtroizel
All rights reserved.

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



#include <read_crumbs.h>

#include <filesystem>
#include <iostream>
#include <queue>

#include <matchmaker/data_reader_common.h>

#include <Stage0Data.h>



bool read_crumbs(
    Stage0Data::word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
    int & text,
    SerialTask::Type task
);

bool read_crumbs_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
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



void read_crumbs(SerialTask::Type task)
{
    task.set_goal(0);
    task.set_progress(0);

    int text = 0;
    FILE * q_file = nullptr;

    std::string const crumbs_path = Stage0Data::book_vocab_dir() + "/Crumbs";
    std::filesystem::create_directory(crumbs_path);
    std::string const crumbs_vocab = Stage0Data::book_vocab_dir() + "/Crumbs/vocabulary";
    FILE * vocab_file = fopen(crumbs_vocab.c_str(), "w");
    if (vocab_file == 0)
    {
        perror(crumbs_vocab.c_str());
        exit(1);
    }

    // the recursive_directory_iterator provides the files in random order, so first sort
    // them by filename using the priority q
    std::priority_queue<
        QFile,
        std::vector<QFile>,
        std::less<std::vector<QFile>::value_type>
    > q_files;
    for (auto const & entry :
            std::filesystem::recursive_directory_iterator(Stage0Data::data_dir() + "/Crumbs/text"))
    {
        if (entry.is_regular_file())
        {
            q_files.push({entry});
            task.as_mutable_goal() += std::filesystem::file_size(entry);
        }
    }

    int files_processed = 0;
    while (!q_files.empty())
    {
        ++files_processed;

        q_file = fopen(q_files.top().entry.path().c_str(), "r");
        if (q_file == 0)
        {
            perror(q_files.top().entry.path().c_str());
            abort();
        }
        Stage0Data::word_attribute::Flags base_attributes;
        bool ok = read_crumbs(base_attributes, q_file, vocab_file, text, task);
        fclose(q_file);
        if (!ok)
        {
            std::cout << "failed to read file: " << q_files.top().entry.path() << std::endl;
            fclose(vocab_file);
            abort();
        }
        q_file = nullptr;
        q_files.pop();

        if (files_processed >= 28)
            break;
    }

    fclose(vocab_file);
    vocab_file = nullptr;
}



bool read_crumbs(
    Stage0Data::word_attribute::Flags const & base_attributes,
    FILE * input_file,
    FILE * vocab_file,
    int & text,
    SerialTask::Type task
)
{
    static std::map<std::string, Stage0Data::Encountered::Type> encounters;
    Stage0Data::word_attribute::Flags attributes;
    std::string line;

    while (true)
    {
        attributes = base_attributes;

        if (!read_crumbs_line(input_file, line, attributes, task))
            break;

        if (line.empty())
            continue;

        if (attributes.is_set(Stage0Data::word_attribute::unmatchable_symbols::grab()))
        {
            std::cout << "encountered unmatchable symbols! text: " << text << " line: " << line << std::endl;
            return false;
        }
        if (attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
        {
            std::cout << "encountered invisible_ascii! text: " << text << " line: " << line << std::endl;
            return false;
        }
        attributes = base_attributes;

        if (line.rfind("~~:", 0) == 0)
        {
            ++text;
            std::string text_line = line.substr(3);
            if (std::to_string(text) != text_line)
            {
                std::cout << "encountered q text " << text_line << " but "
                            << text << " expected! aborting...\n";
                return false;
            }

            Stage0Data::add_book_vocab(text_line, attributes, vocab_file, encounters);

            // read date/time line
            attributes = base_attributes;
            if (!read_crumbs_line(input_file, line, attributes, task))
                break;

            if (attributes.is_set(Stage0Data::word_attribute::unmatchable_symbols::grab()))
            {
                std::cout << "encountered unmatchable symbols! text: " << text << " line: " << line << std::endl;
                return false;
            }
            if (attributes.is_set(Stage0Data::word_attribute::invisible_ascii::grab()))
            {
                std::cout << "encountered invisible_ascii! text: " << text << " line: " << line << std::endl;
                return false;
            }

            // first split date/time from message board
            std::vector<std::string> tokenized_on_comma;
            tokenize(line, ',', tokenized_on_comma);
            if (tokenized_on_comma.size() != 2)
            {
                std::cout << "failed to parse header line! text: " << text << std::endl;
                return false;
            }

            // then split date, time, zoneinfo (time is middle 2 tokens because of AM/PM)
            std::vector<std::string> tokenized_date_time;
            tokenize(tokenized_on_comma[0], ' ', tokenized_date_time);
            if (tokenized_date_time.size() != 4)
            {
                std::cout << "failed to parse date/time stamp! text: " << text << std::endl;
                return false;
            }

            // then split date into day month year
            std::vector<std::string> tokenized_date;
            tokenize(tokenized_date_time[0], '-', tokenized_date);
            if (tokenized_date.size() != 3)
            {
                std::cout << "failed to parse date stamp! text: " << text << std::endl;
                return false;
            }

            // then split time into hour min sec
            std::vector<std::string> tokenized_time;
            tokenize(tokenized_date_time[1], ':', tokenized_time);
            if (tokenized_time.size() != 3)
            {
                std::cout << "failed to parse time stamp! text: " << text << std::endl;
                return false;
            }

            // guarantee 2 digit hour
            if (tokenized_time[0].length() == 1)
                tokenized_time[0].insert(0, 1, '0');

            // now add them all including all tokens & symbols
            std::vector<std::string> words{
                " ", "-", ":", ",", "=", "/", "%", "$", "\"", "|", "^", "!", "?", ".", "(", ")",
                "[", "]", "#", ";", tokenized_date_time[2], tokenized_date_time[3], tokenized_on_comma[1]
            };
            words.insert(words.end(), tokenized_date.begin(), tokenized_date.end());
            words.insert(words.end(), tokenized_time.begin(), tokenized_time.end());
            for (auto const & word : words)
                Stage0Data::add_book_vocab(word, attributes, vocab_file, encounters);
        }
        else
        {
            // ********** TODO REMOVE ME ********************
            if (line.rfind("~~https", 0) == 0)
            {
                std::cout << "encountered error parsing '~~https' text: " << text << std::endl;
                return false;
            }
            else if (line.rfind("~;~", 0) == 0)
            {
                std::cout << "encountered error parsing '~;~' in text: " << text << std::endl;
                return false;
            } else
            //************************************************

            // each line is also just a "word"
            {
                Stage0Data::word_attribute::Flags att = attributes;
                att.set(Stage0Data::word_attribute::phrase::grab());
                Stage0Data::add_book_vocab(line, att, vocab_file, encounters);
            }

            // only parse out lines for non-images and non-archives
            if (line.rfind("~~~", 0) != 0 && line.rfind("~:~", 0) != 0)
            {
                // add each word within the line stripped of punctuation
                {
                    std::vector<std::string> tokenized_on_space;
                    tokenize(line, ' ', tokenized_on_space);
                    for (auto & token : tokenized_on_space)
                    {
                        // check for use of 'single quotes' (apostrophes (') misused as quotes (") are banned)
                        if (token[0] == '\'' ||
                                (token.length() >= 2 &&
                                 token[token.length() - 1] == '\'' &&
                                 token[token.length() - 2] != 's' &&
                                 token[token.length() - 2] != 'S' &&
                                 token[token.length() - 2] != 'n' &&
                                 token[token.length() - 2] != 'N'))
                        {
                            std::cout << "encountered apostrophe misused as \" within q text "
                                      << text << " on line: " << line << std::endl;
                            return false;
                        }

                        if (token.rfind("http", 0) != 0)
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

                                Stage0Data::add_book_vocab(slash_token, attributes, vocab_file, encounters);

                                // split on '='
                                std::vector<std::string> tokenized_on_eq;
                                tokenize(slash_token, '=', tokenized_on_eq);
                                for (auto & eq_token : tokenized_on_eq)
                                    Stage0Data::add_book_vocab(eq_token, attributes, vocab_file, encounters);

                                // for words with brackets, add version of word without brackets
                                bool had_brackets = strip_word('[', slash_token);
                                had_brackets = strip_word(']', slash_token) || had_brackets;
                                if (had_brackets)
                                    Stage0Data::add_book_vocab(slash_token, attributes, vocab_file, encounters);
                            }
                        }
                        else
                        {
                            Stage0Data::add_book_vocab(token, attributes, vocab_file, encounters);
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

                            Stage0Data::add_book_vocab(phrase, att, vocab_file, encounters);
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
            }
        }

        SerialTask::check_progress(task);
    }
    return true;
}



bool read_crumbs_line(
    FILE * f,
    std::string & word,
    Stage0Data::word_attribute::Flags & attributes,
    SerialTask::Type task
)
{
    word.clear();

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
        word += (char) ch;
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
