// order_today_check <file>: prints the file's lines sorted by matchable::str_lt_str, for comparing with
// order.py's today rule
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <matchable/matchable.h>


int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <file>" << std::endl;
        return 2;
    }
    std::ifstream in(argv[1], std::ios::binary);
    std::vector<std::string> lines;
    for (std::string line; std::getline(in, line);)
        lines.push_back(line);
    std::sort(lines.begin(), lines.end(), matchable::str_lt_str);
    for (auto const & line : lines)
        std::cout << line << "\n";
    return 0;
}
