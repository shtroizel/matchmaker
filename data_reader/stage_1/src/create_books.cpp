#include "create_books.h"

#include <vector>

#include "Stage1Data.h"
#include "write_array.h"



bool create_books(SerialTask::Type task)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();

    {
        int goal{0};
        for (int b = 0; b < (int) books.size(); ++b)
            goal += (int) books[b].chapters.size();

        task.set_goal(goal);
        task.set_progress(0);
    }

    // chapters, paragraphs and words are numbered through all books, each level saying where the next
    // level's begin
    std::vector<int> book_chapter_starts;
    std::vector<int> chapter_paragraph_starts;
    std::vector<int> paragraph_word_starts;
    std::vector<int> text_words;
    std::vector<int> text_ancestor_starts;
    std::vector<int> text_ancestors;
    std::vector<int> text_within_first_ancestor;
    std::vector<int> text_referenced;
    for (Book const & book : books)
    {
        book_chapter_starts.push_back((int) chapter_paragraph_starts.size());
        for (Chapter const & chapter : book.chapters)
        {
            chapter_paragraph_starts.push_back((int) paragraph_word_starts.size());
            for (std::vector<BookWord> const & paragraph : chapter.paragraphs)
            {
                paragraph_word_starts.push_back((int) text_words.size());
                for (BookWord const & bw : paragraph)
                {
                    text_words.push_back(bw.word);
                    text_ancestor_starts.push_back((int) text_ancestors.size());
                    text_ancestors.insert(text_ancestors.end(), bw.ancestors.begin(), bw.ancestors.end());
                    text_within_first_ancestor.push_back(bw.index_within_first_ancestor);
                    text_referenced.push_back(bw.referenced ? 1 : 0);
                }
            }
            ++task.as_mutable_progress();
            SerialTask::check_progress(task);
        }
    }
    book_chapter_starts.push_back((int) chapter_paragraph_starts.size());
    chapter_paragraph_starts.push_back((int) paragraph_word_starts.size());
    paragraph_word_starts.push_back((int) text_words.size());
    text_ancestor_starts.push_back((int) text_ancestors.size());

    // every title, author and subtitle in one array, each kind with where its entries begin
    std::vector<int> headings;
    std::vector<int> book_title_starts;
    std::vector<int> book_author_starts;
    std::vector<int> chapter_title_starts;
    std::vector<int> chapter_subtitle_starts;
    auto const add_heading =
        [&](std::vector<int> & starts, std::vector<int> const & heading)
        {
            starts.push_back((int) headings.size());
            headings.insert(headings.end(), heading.begin(), heading.end());
        };
    for (Book const & book : books)
        add_heading(book_title_starts, book.title);
    book_title_starts.push_back((int) headings.size());
    for (Book const & book : books)
        add_heading(book_author_starts, book.author);
    book_author_starts.push_back((int) headings.size());
    for (Book const & book : books)
        for (Chapter const & chapter : book.chapters)
            add_heading(chapter_title_starts, chapter.title);
    chapter_title_starts.push_back((int) headings.size());
    for (Book const & book : books)
        for (Chapter const & chapter : book.chapters)
            add_heading(chapter_subtitle_starts, chapter.subtitle);
    chapter_subtitle_starts.push_back((int) headings.size());

    return write_array("book_chapter_starts", book_chapter_starts) &&
           write_array("chapter_paragraph_starts", chapter_paragraph_starts) &&
           write_array("paragraph_word_starts", paragraph_word_starts) &&
           write_array("text_words", text_words) &&
           write_array("text_ancestor_starts", text_ancestor_starts) &&
           write_array("text_ancestors", text_ancestors) &&
           write_array("text_within_first_ancestor", text_within_first_ancestor) &&
           write_array("text_referenced", text_referenced) &&
           write_array("headings", headings) &&
           write_array("book_title_starts", book_title_starts) &&
           write_array("book_author_starts", book_author_starts) &&
           write_array("chapter_title_starts", chapter_title_starts) &&
           write_array("chapter_subtitle_starts", chapter_subtitle_starts);
}
