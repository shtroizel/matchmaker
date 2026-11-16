// crumbs_hour_check: crumbs_hour_24() turns a Crumbs post hour (12 hour clock with AM or PM) into the hour
// both data readers use, on the 24 hour clock padded to two digits, and refuses anything else
#include <iostream>
#include <string>

#include <matchmaker/data_reader_common.h>



int main()
{
    struct Case { std::string hour; std::string am_pm; std::string expected; };
    Case const cases[]{
        {"12", "AM", "00"}, {"1", "AM", "01"}, {"9", "AM", "09"}, {"11", "AM", "11"},
        {"12", "PM", "12"}, {"1", "PM", "13"}, {"9", "PM", "21"}, {"11", "PM", "23"},
        {"01", "PM", "13"},
        {"0", "AM", ""}, {"13", "PM", ""}, {"", "AM", ""}, {"1a", "AM", ""}, {"123", "AM", ""},
        {"1", "am", ""}, {"1", "", ""},
    };
    int failures = 0;
    for (auto const & c : cases)
    {
        std::string const actual = crumbs_hour_24(c.hour, c.am_pm);
        if (actual != c.expected)
        {
            std::cout << "crumbs_hour_24(\"" << c.hour << "\", \"" << c.am_pm << "\") = \"" << actual
                      << "\", expected \"" << c.expected << "\"" << std::endl;
            ++failures;
        }
    }
    std::cout << (failures == 0 ? "ok" : "FAILED") << std::endl;
    return failures == 0 ? 0 : 1;
}
