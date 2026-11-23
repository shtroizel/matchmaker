// routing_check <leaf list> <tree.cmake>
// Checks that the generated leaf table (generate_code.py from a stage 0 leaf list, read by src/tree.cpp)
// sends each term to the last leaf whose first word is not after it: linked with a tree_data.cpp built on
// stub_leaves.h, and tree::leaf_of() gives the leaf a term reaches.
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <matchmaker/word_order.h>

#include <tree.h>



namespace
{
    int failures{0};

    void fail(std::string const & what)
    {
        if (++failures <= 20)
            std::cout << "FAIL: " << what << std::endl;
    }


    // the names and first words of a leaf list: "<name> <first word>" per line
    std::vector<std::pair<std::string, std::string>> read_leaf_list(std::string const & filename)
    {
        std::vector<std::pair<std::string, std::string>> ret;
        std::ifstream f{filename, std::ios::binary};
        for (std::string line; std::getline(f, line);)
        {
            size_t const space = line.find(' ');
            if (space == std::string::npos)
            {
                std::cerr << filename << ": no first word in \"" << line << "\"" << std::endl;
                exit(2);
            }
            ret.emplace_back(line.substr(0, space), line.substr(space + 1));
        }
        return ret;
    }


    // the leaf names in tree.cmake's prefix_list, in generated (index) order
    std::vector<std::string> generated_leaves(std::string const & filename)
    {
        std::vector<std::string> ret;
        std::ifstream f{filename};
        bool in_list{false};
        for (std::string line; std::getline(f, line);)
        {
            line.erase(0, line.find_first_not_of(' '));
            if (line == "prefix_list")
                in_list = true;
            else if (in_list && line == ")")
                break;
            else if (in_list)
                ret.push_back(line);
        }
        if (!in_list)
        {
            std::cerr << "no prefix_list in " << filename << std::endl;
            exit(2);
        }
        return ret;
    }


    // each first word alone, with tails, without its last byte, and with its last byte one lower and one
    // higher, plus words before and after every leaf
    std::vector<std::string> probes(std::vector<std::string> const & firsts)
    {
        std::vector<std::string> ret{" ", "~", "zzzzzzzz", std::string(1, (char) 200)};
        for (auto const & w : firsts)
        {
            for (std::string const tail : {"", " ", "!", "0", "A", "a", "~", "zzzz"})
                ret.push_back(w + tail);
            if (w.size() > 1)
                ret.push_back(w.substr(0, w.size() - 1));
            std::string lower = w;
            --lower.back();
            ret.push_back(lower);
            std::string higher = w;
            ++higher.back();
            ret.push_back(higher);
        }
        std::sort(ret.begin(), ret.end(), mm_word_less);
        ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
        return ret;
    }
}



int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "usage: " << argv[0] << " <leaf list> <tree.cmake>" << std::endl;
        return 2;
    }

    auto const listed = read_leaf_list(argv[1]);
    std::vector<std::string> firsts;
    for (auto const & [name, first] : listed)
        firsts.push_back(first);

    // the list is in term order, and the generated leaves are its leaves, in its order
    for (size_t i = 1; i < firsts.size(); ++i)
        if (!mm_word_less(firsts[i - 1], firsts[i]))
            fail("first words out of order: \"" + firsts[i - 1] + "\", \"" + firsts[i] + "\"");
    auto const generated = generated_leaves(argv[2]);
    if (generated.size() != listed.size() || tree::leaf_count != (int) listed.size())
        fail("leaves generated: " + std::to_string(generated.size()) + " (leaf_count " +
             std::to_string(tree::leaf_count) + "), listed " + std::to_string(listed.size()));
    for (size_t i = 0; i < std::min(generated.size(), listed.size()); ++i)
        if (generated[i] != listed[i].first)
            fail("leaf " + std::to_string(i) + " is " + generated[i] + ", listed " + listed[i].first);
    for (int i = 0; i < std::min(tree::leaf_count, (int) firsts.size()); ++i)
        if (tree::first_terms[i] != firsts[i])
            fail("leaf " + std::to_string(i) + " first word \"" + tree::first_terms[i] + "\", listed \"" +
                 firsts[i] + "\"");

    // every probe reaches the last leaf whose first word is not after it (the first leaf if none)
    auto const terms = probes(firsts);
    for (auto const & term : terms)
    {
        int const after = (int) (std::upper_bound(firsts.begin(), firsts.end(), term, mm_word_less)
                                 - firsts.begin());
        int const want = std::max(0, after - 1);
        int const got = tree::leaf_of(term.data(), term.size());
        if (got != want)
            fail("\"" + term + "\" reaches leaf " + std::to_string(got) + ", expected " +
                 std::to_string(want));
    }

    std::cout << terms.size() << " terms, " << listed.size() << " leaves: " << failures << " failures"
              << std::endl;
    return failures == 0 ? 0 : 1;
}
