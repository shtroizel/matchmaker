#include "create_books.h"

#include <algorithm>
#include <vector>

#include "array_cuts.h"
#include "Stage1Data.h"
#include "write_array.h"



namespace
{
    // the ints a group of chapters holds besides its chapters': where each of its 9 parts begins, and the
    // entry that ends each of its 4 arrays of starts
    int const group_ints{13};


    int chapter_ints(Chapter const & chapter)
    {
        int ints = 3 + (int) (chapter.paragraphs.size() + chapter.title.size() + chapter.subtitle.size());
        for (std::vector<BookWord> const & paragraph : chapter.paragraphs)
        {
            ints += 4 * (int) paragraph.size();
            for (BookWord const & bw : paragraph)
                ints += (int) bw.ancestors.size();
        }
        return ints;
    }


    // a book's chapters from first up to end as the library compiles them: where each part begins, then
    // the parts. Paragraphs, words, ancestors and headings are numbered from the group's first, each level
    // saying where the next level's begin
    std::vector<int> chapter_group(Book const & book, size_t first, size_t end)
    {
        std::vector<int> paragraph_starts;
        std::vector<int> word_starts;
        std::vector<int> words;
        std::vector<int> ancestor_starts;
        std::vector<int> ancestors;
        std::vector<int> within_first_ancestor;
        std::vector<int> referenced;
        std::vector<int> heading_starts;
        std::vector<int> headings;
        for (size_t c = first; c < end; ++c)
        {
            Chapter const & chapter = book.chapters[c];
            paragraph_starts.push_back((int) word_starts.size());
            for (std::vector<BookWord> const & paragraph : chapter.paragraphs)
            {
                word_starts.push_back((int) words.size());
                for (BookWord const & bw : paragraph)
                {
                    words.push_back(bw.word);
                    ancestor_starts.push_back((int) ancestors.size());
                    ancestors.insert(ancestors.end(), bw.ancestors.begin(), bw.ancestors.end());
                    within_first_ancestor.push_back(bw.index_within_first_ancestor);
                    referenced.push_back(bw.referenced ? 1 : 0);
                }
            }
            heading_starts.push_back((int) headings.size());
            headings.insert(headings.end(), chapter.title.begin(), chapter.title.end());
            heading_starts.push_back((int) headings.size());
            headings.insert(headings.end(), chapter.subtitle.begin(), chapter.subtitle.end());
        }
        paragraph_starts.push_back((int) word_starts.size());
        word_starts.push_back((int) words.size());
        ancestor_starts.push_back((int) ancestors.size());
        heading_starts.push_back((int) headings.size());

        std::vector<std::vector<int> const *> const parts{
            &paragraph_starts, &word_starts, &words, &ancestor_starts, &ancestors, &within_first_ancestor,
            &referenced, &heading_starts, &headings
        };
        std::vector<int> group;
        int start = (int) parts.size();
        for (std::vector<int> const * part : parts)
        {
            group.push_back(start);
            start += (int) part->size();
        }
        for (std::vector<int> const * part : parts)
            group.insert(group.end(), part->begin(), part->end());
        return group;
    }
}



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

    // a book's chapters are cut into groups of 1 << shift chapters, the shift its own: book_groups holds
    // each book's first group and its shift
    std::vector<int> book_chapter_starts;
    std::vector<int> book_groups;
    std::vector<std::vector<int>> groups;
    int chapter_count{0};
    for (Book const & book : books)
    {
        book_chapter_starts.push_back(chapter_count);
        chapter_count += (int) book.chapters.size();

        std::vector<int> ints;
        for (Chapter const & chapter : book.chapters)
            ints.push_back(chapter_ints(chapter));
        int const shift = array_cuts::group_shift(ints, group_ints, Stage1Data::nil.as_array_budget());

        book_groups.push_back((int) groups.size());
        book_groups.push_back(shift);
        for (size_t first = 0; first < book.chapters.size(); first += size_t{1} << shift)
        {
            size_t const end = std::min(book.chapters.size(), first + (size_t{1} << shift));
            groups.push_back(chapter_group(book, first, end));
            for (size_t c = first; c < end; ++c)
            {
                ++task.as_mutable_progress();
                SerialTask::check_progress(task);
            }
        }
    }
    book_chapter_starts.push_back(chapter_count);

    // every book's title and author in one array, each kind with where its entries begin
    std::vector<int> headings;
    std::vector<int> book_title_starts;
    std::vector<int> book_author_starts;
    for (Book const & book : books)
    {
        book_title_starts.push_back((int) headings.size());
        headings.insert(headings.end(), book.title.begin(), book.title.end());
    }
    book_title_starts.push_back((int) headings.size());
    for (Book const & book : books)
    {
        book_author_starts.push_back((int) headings.size());
        headings.insert(headings.end(), book.author.begin(), book.author.end());
    }
    book_author_starts.push_back((int) headings.size());

    return write_array("book_chapter_starts", book_chapter_starts) &&
           write_array("book_groups", book_groups) &&
           write_units("chapters", groups) &&
           write_array("headings", headings) &&
           write_array("book_title_starts", book_title_starts) &&
           write_array("book_author_starts", book_author_starts);
}
