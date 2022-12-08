#include "create_books.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "Stage1Data.h"


bool create_books(int progress_steps)
{
    std::vector<Book> const & books = Stage1Data::nil.as_books();
    std::string const & BOOKS_HEADER = Stage1Data::nil.as_workspace_dir() +
                                       "/generated_include/matchmaker/books.h";

    int progress{0};
    int goal{0};
    for (int b = 0; b < (int) books.size(); ++b)
        goal += (int) books[b].chapters.size();

    FILE * f = fopen(BOOKS_HEADER.c_str(), "w");
    if (nullptr == f)
    {
        std::cout << "failed to open " << BOOKS_HEADER << std::endl;
        return false;
    }

    if (fputs("#pragma once\n#include <vector>\n\n", f) == EOF)
        goto books_err;

    if (fputs("struct BookWord\n", f) == EOF) goto books_err;
    if (fputs("{\n", f) == EOF) goto books_err;
    if (fputs("    int word{-1};\n", f) == EOF) goto books_err;
    if (fputs("    int parent_phrase{-1};\n", f) == EOF) goto books_err;
    if (fputs("    int parent_phrase_start_index{-1};\n", f) == EOF) goto books_err;
    if (fputs("};\n", f) == EOF) goto books_err;
    if (fputs("struct Chapter\n", f) == EOF) goto books_err;
    if (fputs("{\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<int> title;\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<int> subtitle;\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<std::vector<BookWord>> paragraphs;\n", f) == EOF) goto books_err;
    if (fputs("};\n", f) == EOF) goto books_err;
    if (fputs("struct Book\n", f) == EOF) goto books_err;
    if (fputs("{\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<int> title;\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<int> author;\n", f) == EOF) goto books_err;
    if (fputs("    std::vector<Chapter> chapters;\n", f) == EOF) goto books_err;
    if (fputs("};\n\n\n", f) == EOF) goto books_err;
    if (fputs("inline std::vector<Book> const books\n", f) == EOF) goto books_err;

    // open books
    if (fputs("{\n", f) == EOF) goto books_err;

    for (size_t bk_i = 0; bk_i < books.size(); ++bk_i)
    {
        // open book
        if (fputs("    {\n", f) == EOF) goto books_err;

        // book title
        if (fputs("        { ", f) == EOF) goto books_err;
        for (size_t bk_title_i = 0; bk_title_i < books[bk_i].title.size(); ++bk_title_i)
        {
            std::string s = std::to_string(books[bk_i].title[bk_title_i]);
            s += ", ";
            if (fputs(s.c_str(), f) == EOF) goto books_err;
        }
        if (fputs("},\n", f) == EOF) goto books_err;

        // book author
        if (fputs("        { ", f) == EOF) goto books_err;
        for (size_t bk_author_i = 0; bk_author_i < books[bk_i].author.size(); ++bk_author_i)
        {
            std::string s = std::to_string(books[bk_i].author[bk_author_i]);
            s += ", ";
            if (fputs(s.c_str(), f) == EOF) goto books_err;
        }
        if (fputs("},\n", f) == EOF) goto books_err;

        // open chapters
        if (fputs("        {\n", f) == EOF) goto books_err;

        for (size_t ch_i = 0; ch_i < books[bk_i].chapters.size(); ++ch_i)
        {
            // open chapter
            if (fputs("            {\n", f) == EOF) goto books_err;

            // chapter title
            if (fputs("                { ", f) == EOF) goto books_err;
            for (size_t ch_title_i = 0; ch_title_i < books[bk_i].chapters[ch_i].title.size(); ++ch_title_i)
            {
                std::string s = std::to_string(books[bk_i].chapters[ch_i].title[ch_title_i]);
                s += ", ";
                if (fputs(s.c_str(), f) == EOF) goto books_err;
            }
            if (fputs("},\n", f) == EOF) goto books_err;

            // chapter subtitle
            if (fputs("                { ", f) == EOF) goto books_err;
            for (size_t ch_subtitle_i = 0;
                 ch_subtitle_i < books[bk_i].chapters[ch_i].subtitle.size(); ++ch_subtitle_i)
            {
                std::string s = std::to_string(books[bk_i].chapters[ch_i].subtitle[ch_subtitle_i]);
                s += ", ";
                if (fputs(s.c_str(), f) == EOF) goto books_err;
            }
            if (fputs("},\n", f) == EOF) goto books_err;

            // open paragraphs
            if (fputs("                {\n", f) == EOF) goto books_err;

            for (size_t p_i = 0; p_i < books[bk_i].chapters[ch_i].paragraphs.size(); ++p_i)
            {
                // open paragraph
                if (fputs("                    { ", f) == EOF) goto books_err;

                for (size_t bw_i = 0; bw_i < books[bk_i].chapters[ch_i].paragraphs[p_i].size(); ++bw_i)
                {
                    BookWord const & bw = books[bk_i].chapters[ch_i].paragraphs[p_i][bw_i];
                    std::string s = "{";
                    s += std::to_string(bw.word);
                    s += ", ";
                    s += std::to_string(bw.parent_phrase);
                    s += ", ";
                    s += std::to_string(bw.parent_phrase_start_index);
                    s += "}, ";
                    if (fputs(s.c_str(), f) == EOF) goto books_err;
                }

                // close paragraph
                if (fputs("},\n", f) == EOF) goto books_err;
            }

            // close paragraphs
            if (fputs("                },\n", f) == EOF) goto books_err;

            // close chapter
            if (fputs("            },\n", f) == EOF) goto books_err;

            ++progress;
            if (progress % (goal / progress_steps) == 0)
                std::cout << "." << std::flush;
        }

        // close chapters
        if (fputs("        }\n", f) == EOF) goto books_err;

        // close book
        if (fputs("    },\n", f) == EOF) goto books_err;
    }

    // close books
    if (fputs("};\n", f) == EOF) goto books_err;

    fclose(f);
    return true;

books_err:
    std::cout << "FAILED!" << std::endl;
    fclose(f);
    return false;
}
