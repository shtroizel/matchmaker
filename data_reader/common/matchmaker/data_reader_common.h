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



// a Crumbs post hour ("1" to "12" with "AM" or "PM") on the 24 hour clock padded to two digits ("00" to
// "23"), as stage 1 prints it in each chapter subtitle and stage 0 adds it to the vocabulary; empty when
// the hour or AM/PM is not valid
inline std::string crumbs_hour_24(std::string const & hour, std::string const & am_pm)
{
    if (hour.empty() || hour.size() > 2 || hour.find_first_not_of("0123456789") != std::string::npos)
        return "";
    if (am_pm != "AM" && am_pm != "PM")
        return "";
    int h = std::stoi(hour);
    if (h < 1 || h > 12)
        return "";
    h = h % 12 + (am_pm == "PM" ? 12 : 0);
    return std::string{char('0' + h / 10), char('0' + h % 10)};
}
