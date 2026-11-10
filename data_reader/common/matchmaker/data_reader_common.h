#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <matchable/matchable.h>


MATCHABLE(
    dictionary_usage,
    all,
    reduced,
    none
)


MATCHABLE(
    Buch,

    Crumbs
)


inline Buch::Flags & selected_books()
{
    static Buch::Flags books;
    return books;
}


// "all" or a comma separated list of books, at least one
inline bool select_books(std::string const & arg)
{
    selected_books() = Buch::Flags{};
    if (arg == "all")
    {
        for (auto const & book : Buch::variants_by_index())
            selected_books().set(book);
        return true;
    }

    size_t start = 0;
    while (start <= arg.size())
    {
        size_t end = arg.find(',', start);
        if (end == std::string::npos)
            end = arg.size();
        Buch::Type book = Buch::from_string(arg.substr(start, end - start));
        if (book.is_nil())
            return false;
        selected_books().set(book);
        start = end + 1;
    }
    return true;
}


struct QFile
{
    std::filesystem::directory_entry entry;
    friend bool operator<(QFile const & l, QFile const & r);
};
inline bool operator<(QFile const & l, QFile const & r)
{
    return r.entry.path().filename() < l.entry.path().filename();
}
