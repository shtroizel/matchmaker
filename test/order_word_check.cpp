// order_word_check <file>: prints the file's ASCII lines sorted by matchmaker's word order (word_order.h),
// for comparing with order.py's folded rule
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <matchmaker/word_order.h>


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
        if (std::all_of(line.begin(), line.end(), [](char c) { return c >= 32 && c < 127; }))
            lines.push_back(line);
    std::sort(lines.begin(), lines.end(), mm_word_less);
    for (auto const & line : lines)
        std::cout << line << "\n";
    return 0;
}
