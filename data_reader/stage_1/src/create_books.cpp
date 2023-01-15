#include "create_books.h"

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



// The functions (code) for books are separated from the "content" (data structures) used
// by the implementation. All of these files though are just code and must be generated either
// by using a "template" (not to be confused with c++ templates) or in the case of "content" files
// by creating them here from scratch...


// "books content" describes the books available within the library
void create_books_content_and_get_book_identifiers(std::vector<std::string> & book_identifiers);

// "book content" describes the chapters available for a single book
void create_book_content(int book_index, std::string const & book_identifier);

// "chapter content" provides the vectors that store the data
void create_chapter_content(int book_index, std::string const & book_identifier, int chapter_index);

void create_file_from_template(
    std::string const & template_file,
    std::string const & destination_file,
    std::string const & book_identifier,
    int chapter_index
);

void replace(std::string & data, std::string search_pattern, std::string replacement);






bool create_books(int progress_steps)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();

    int progress{0};
    int goal{0};
    for (int b = 0; b < (int) books.size(); ++b)
        goal += (int) books[b].chapters.size();

    std::vector<std::string> book_identifiers;
    create_books_content_and_get_book_identifiers(book_identifiers);


    // create code for each book
    for (size_t b_i = 0; b_i < book_identifiers.size(); ++b_i)
    {
        std::string const & book_identifier = book_identifiers[b_i];

        // create book header file
        {
            std::string const header_template = Stage1Data::nil.as_template_dir() + "/book.h";
            std::string const header_destination = Stage1Data::nil.as_workspace_dir() +
                        "/generated_include/matchmaker/books/" + book_identifier + ".h";
            create_file_from_template(header_template, header_destination, book_identifier, -1);
        }

        // create book source file
        {
            std::string const source_template = Stage1Data::nil.as_template_dir() + "/book.cpp";
            std::string const source_destination = Stage1Data::nil.as_workspace_dir() +
                        "/generated_src/books/" + book_identifier + ".cpp";
            create_file_from_template(source_template, source_destination, book_identifier, -1);
        }

        // create book content file
        create_book_content(b_i, book_identifier);

        for (size_t ch_i = 0; ch_i < books[b_i].chapters.size(); ++ch_i)
        {
            std::string const ch_i_as_string = std::to_string(ch_i);

            // create chapter header file
            {
                std::string const header_template = Stage1Data::nil.as_template_dir() + "/chapter.h";
                std::string const header_destination = Stage1Data::nil.as_workspace_dir() +
                            "/generated_include/matchmaker/books/" + book_identifier +
                            "_chapter_" + ch_i_as_string + ".h";
                create_file_from_template(header_template, header_destination, book_identifier, ch_i);
            }

            {
                // create chapter source file
                std::string const source_template = Stage1Data::nil.as_template_dir() + "/chapter.cpp";
                std::string const source_destination = Stage1Data::nil.as_workspace_dir() +
                            "/generated_src/books/" + book_identifier + "_chapter_" + ch_i_as_string + ".cpp";
                create_file_from_template(source_template, source_destination, book_identifier, ch_i);
            }

            // create chapter content file
            create_chapter_content(b_i, book_identifier, ch_i);

            ++progress;
            if (progress % (goal / progress_steps) == 0)
                std::cout << "." << std::flush;
        }
    }

    return true;
}


void create_books_content_and_get_book_identifiers(std::vector<std::string> & book_identifiers)
{
    book_identifiers.clear();
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    std::string const & BOOKS_CONTENT = Stage1Data::nil.as_workspace_dir() +
                                       "/generated_include/matchmaker/books/books_content.h";
    FILE * f = fopen(BOOKS_CONTENT.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << BOOKS_CONTENT << std::endl;
        abort();
    }

    if (fputs("#include <array>\n", f) == EOF) abort();
    if (fputs("#include <functional>\n\n", f) == EOF) abort();

    // get book identifiers and write book includes
    {
        bool ok{false};
        int space = mm_lookup(" ", &ok);
        if (!ok)
        {
            std::cout << "\nfailed to lookup space (' ')!" << std::endl;
            abort();
        }
        for (Book const & b : books)
        {
            std::string book_identifier;
            for (auto t : b.title)
            {
                if (t == space)
                    book_identifier += "_";
                else
                    book_identifier += mm_at(t, nullptr);
            }
            book_identifiers.push_back(book_identifier);
            if (fputs("#include <matchmaker/books/", f) == EOF) abort();
            if (fputs(book_identifier.c_str(), f) == EOF) abort();
            if (fputs(".h>\n", f) == EOF) abort();
        }
    }
    if (fputs("\n\n\n", f) == EOF) abort();

    if (fputs("constexpr int const BOOK_COUNT{", f) == EOF) abort();
    {
        std::string book_count_as_string = std::to_string(books.size());
        if (fputs(book_count_as_string.c_str(), f) == EOF) abort();
        if (fputs("};\n\n\n\n", f) == EOF) abort();
    }


    // book title funcs
    if (fputs("std::array<std::function<void (int const * *, int *)>, BOOK_COUNT> book_title_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_title,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // book author funcs
    if (fputs("std::array<std::function<void (int const * *, int *)>, BOOK_COUNT> book_author_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_author,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // chapter count funcs
    if (fputs("std::array<std::function<int ()>, BOOK_COUNT> chapter_count_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_chapter_count,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // chapter title funcs
    if (fputs("std::array<std::function<void (int, int const * *, int *)>, BOOK_COUNT> chapter_title_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_chapter_title,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // chapter subtitle funcs
    if (fputs("std::array<std::function<void (int, int const * *, int *)>, BOOK_COUNT> chapter_subtitle_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_chapter_subtitle,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // paragraph count funcs
    if (fputs("std::array<std::function<int (int)>, BOOK_COUNT> paragraph_count_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_paragraph_count,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();


    // word count funcs
    if (fputs("std::array<std::function<int (int, int)>, BOOK_COUNT> word_count_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_word_count,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n", f) == EOF) abort();


    // word funcs
    if (fputs("std::array<std::function<int (int, int, int, int const * *, int *, int *, bool *)>, "
              "BOOK_COUNT> word_funcs\n", f) == EOF) abort();
    if (fputs("{\n", f) == EOF) abort();
    for (std::string const & bi : book_identifiers)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(bi.c_str(), f) == EOF) abort();
        if (fputs("_word,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    fclose(f);
}


void create_book_content(int book_index, std::string const & book_identifier)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    std::string const & BOOK_CONTENT = Stage1Data::nil.as_workspace_dir() +
            "/generated_src/books/" + book_identifier + "_content.h";

    FILE * f = fopen(BOOK_CONTENT.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << BOOK_CONTENT << std::endl;
        abort();
    }

    // write includes
    if (fputs("#pragma once\n\n", f) == EOF) abort();
    if (fputs("#include <array>\n", f) == EOF) abort();
    if (fputs("#include <functional>\n", f) == EOF) abort();
    if (fputs("#include <vector>\n\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("#include <matchmaker/books/", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs(".h>\n", f) == EOF) abort();
    }

    // write chapter count constant
    if (fputs("\n\n\n", f) == EOF) abort();
    if (fputs("constexpr int const ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT{", f) == EOF) abort();
    std::string const chapter_count_as_string = std::to_string(books[book_index].chapters.size());
    if (fputs(chapter_count_as_string.c_str(), f) == EOF) abort();
    if (fputs("};\n\n\n\n", f) == EOF) abort();

    // write title data
    if (fputs("std::vector<int> ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_book_title\n{\n", f) == EOF) abort();
    for (int t : books[book_index].title)
    {
        if (fputs("    ", f) == EOF) abort();
        std::string t_as_string = std::to_string(t);
        if (fputs(t_as_string.c_str(), f) == EOF) abort();
        if (fputs(",\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // write author data
    if (fputs("std::vector<int> ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_book_author\n{\n", f) == EOF) abort();
    for (int a : books[book_index].author)
    {
        if (fputs("    ", f) == EOF) abort();
        std::string a_as_string = std::to_string(a);
        if (fputs(a_as_string.c_str(), f) == EOF) abort();
        if (fputs(",\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n\n", f) == EOF) abort();


    // write chapter title funcs
    if (fputs("std::array<std::function<void (int const * *, int *)>, ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT> Crumbs_chapter_title_funcs\n{\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs("_title,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // write chapter subtitle funcs
    if (fputs("std::array<std::function<void (int const * *, int *)>, ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT> Crumbs_chapter_subtitle_funcs\n{\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs("_subtitle,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // write paragraph count funcs
    if (fputs("std::array<std::function<int ()>, ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT> Crumbs_paragraph_count_funcs\n{\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs("_paragraph_count,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // write word count funcs
    if (fputs("std::array<std::function<int (int)>, ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT> Crumbs_word_count_funcs\n{\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs("_word_count,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // write word funcs
    if (fputs("std::array<std::function<int (int, int, int const * *, int *, int *, bool *)>, ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_CHAPTER_COUNT> Crumbs_word_funcs\n{\n", f) == EOF) abort();
    for (size_t ch_i = 0; ch_i < books[book_index].chapters.size(); ++ch_i)
    {
        if (fputs("    &mm_", f) == EOF) abort();
        if (fputs(book_identifier.c_str(), f) == EOF) abort();
        if (fputs("_chapter_", f) == EOF) abort();
        std::string ch_i_as_string = std::to_string(ch_i);
        if (fputs(ch_i_as_string.c_str(), f) == EOF) abort();
        if (fputs("_word,\n", f) == EOF) abort();
    }
    if (fputs("};\n\n", f) == EOF) abort();

    fclose(f);
}


void create_chapter_content(int book_index, std::string const & book_identifier, int chapter_index)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    std::string const chapter_index_as_string = std::to_string(chapter_index);
    std::string const & chapter_content = Stage1Data::nil.as_workspace_dir() +
            "/generated_src/books/" + book_identifier + "_chapter_" + chapter_index_as_string + "_content.h";

    FILE * f = fopen(chapter_content.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << chapter_content << std::endl;
        abort();
    }

    // includes
    if (fputs("#pragma once\n\n", f) == EOF) abort();
    if (fputs("#include <vector>\n\n", f) == EOF) abort();
    if (fputs("#include \"BookWord.h\"\n\n\n\n", f) == EOF) abort();

    // title
    if (fputs("std::vector<int> ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_chapter_", f) == EOF) abort();
    if (fputs(chapter_index_as_string.c_str(), f) == EOF) abort();
    if (fputs("_title\n{\n", f) == EOF) abort();
    for (int t : books[book_index].chapters[chapter_index].title)
    {
        std::string t_as_string = std::to_string(t);
        if (fputs("    ", f) == EOF) abort();
        if (fputs(t_as_string.c_str(), f) == EOF) abort();
        if (fputs(",\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // subtitle
    if (fputs("std::vector<int> ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_chapter_", f) == EOF) abort();
    if (fputs(chapter_index_as_string.c_str(), f) == EOF) abort();
    if (fputs("_subtitle\n{\n", f) == EOF) abort();
    for (int st : books[book_index].chapters[chapter_index].subtitle)
    {
        std::string st_as_string = std::to_string(st);
        if (fputs("    ", f) == EOF) abort();
        if (fputs(st_as_string.c_str(), f) == EOF) abort();
        if (fputs(",\n", f) == EOF) abort();
    }
    if (fputs("};\n\n\n", f) == EOF) abort();

    // paragraphs
    if (fputs("std::vector<std::vector<BookWord>> const ", f) == EOF) abort();
    if (fputs(book_identifier.c_str(), f) == EOF) abort();
    if (fputs("_chapter_", f) == EOF) abort();
    if (fputs(chapter_index_as_string.c_str(), f) == EOF) abort();
    if (fputs("_paragraphs\n{\n", f) == EOF) abort();
    for (auto const & paragraph : books[book_index].chapters[chapter_index].paragraphs)
    {
        if (fputs("    {\n", f) == EOF) abort();
        for (auto const & bw : paragraph)
        {
            if (fputs("        { ", f) == EOF) abort();
            std::string const word_as_string = std::to_string(bw.word);
            if (fputs(word_as_string.c_str(), f) == EOF) abort();
            if (fputs(", { ", f) == EOF) abort();
            for (int ancestor : bw.ancestors)
            {
                std::string ancestor_as_string = std::to_string(ancestor);
                if (fputs(ancestor_as_string.c_str(), f) == EOF) abort();
                if (fputs(", ", f) == EOF) abort();
            }
            if (fputs("}, ", f) == EOF) abort();
            std::string const iwfa_as_string = std::to_string(bw.index_within_first_ancestor);
            if (fputs(iwfa_as_string.c_str(), f) == EOF) abort();
            if (fputs(", ", f) == EOF) abort();
            if (bw.referenced)
            {
                if (fputs("true", f) == EOF) abort();
            }
            else
            {
                if (fputs("false", f) == EOF) abort();
            }
            if (fputs(" },\n", f) == EOF) abort();
        }
        if (fputs("    },\n", f) == EOF) abort();
    }
    if (fputs("};\n", f) == EOF) abort();

    fclose(f);
}


void create_file_from_template(
    std::string const & template_file,
    std::string const & destination_file,
    std::string const & book_identifier,
    int chapter_index
)
{
    std::string template_file_content;
    {
        FILE * f = fopen(template_file.c_str(), "r");
        if (nullptr == f)
        {
            std::cout << "failed to open " << template_file << std::endl;
            abort();
        }

        int ch;
        while ((ch = fgetc(f)) != EOF)
            template_file_content += (char) ch;

        fclose(f);
    }

    replace(template_file_content, "aoeu", book_identifier);

    if (chapter_index != -1)
    {
        std::string const chapter_index_as_string = std::to_string(chapter_index);
        replace(template_file_content, "snth", chapter_index_as_string);
    }

    {
        FILE * f = fopen(destination_file.c_str(), "w");
        if (nullptr == f)
        {
            std::cout << "failed to open " << destination_file << std::endl;
            abort();
        }

        if (fputs(template_file_content.c_str(), f) == EOF)
        {
            std::cout << "failed to write content to " << destination_file << std::endl;
            abort();
        }

        fclose(f);
    }
}


void replace(std::string & data, std::string search_pattern, std::string replacement)
{
    size_t position = data.find(search_pattern);
    while( position != std::string::npos)
    {
        data.replace(position, search_pattern.size(), replacement);
        position = data.find(search_pattern, position + replacement.size());
    }
}
