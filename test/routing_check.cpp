// routing_check <parent prefix file> <tree.cmake> [leaf list]
// Checks that the generated tree (generate_code.py tables read by src/tree.cpp) sends terms to the leaves
// the stage 0 reader puts them in. Linked with a tree_data.cpp built on stub_leaves.h, so tree::lookup()
// returns the index of the leaf a term reaches, or for a leaf that was not generated, the index of the
// next generated leaf (where the term would be inserted).
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <matchable/matchable.h>

#include <routing.h>
#include <tree.h>

#include "tree_leaves.h"



namespace
{
    int failures{0};

    void fail(std::string const & what)
    {
        if (++failures <= 20)
            std::cout << "FAIL: " << what << std::endl;
    }


    std::string shown(std::string const & term)
    {
        return "\"" + term + "\"";
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


    // the leaves a leaf list names, in term order
    std::vector<std::string> listed_leaves(std::string const & filename,
                                           std::vector<std::string> const & all)
    {
        std::map<std::string, std::string> by_symbols;
        for (auto const & leaf : all)
            by_symbols[routing::symbols(leaf, ' ')] = leaf;

        std::vector<std::string> ret;
        std::ifstream f{filename};
        for (std::string line; std::getline(f, line);)
        {
            auto it = by_symbols.find(line);
            if (it == by_symbols.end())
                fail("leaf list names \"" + line + "\", which is not a leaf of the tree");
            else
                ret.push_back(it->second);
        }
        std::sort(ret.begin(), ret.end(), matchable::str_lt_str);
        return ret;
    }


    // each parent alone, and each child of each parent alone and followed by more characters
    std::vector<std::string> probes(std::set<std::string> const & parents)
    {
        std::vector<std::string> ret;
        for (std::string const & parent : parents)
        {
            if (!parent.empty())
                ret.push_back(parent);
            for (int c = 32; c <= 126; ++c)
            {
                std::string const child = parent + (char) c;
                for (std::string const tail : {"", " ", "!", "A", "a", "~", "zzzzzzzz"})
                    ret.push_back(child + tail);
            }
        }
        std::sort(ret.begin(), ret.end(), matchable::str_lt_str);
        ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
        return ret;
    }
}



int main(int argc, char ** argv)
{
    if (argc != 3 && argc != 4)
    {
        std::cerr << "usage: " << argv[0] << " <parent prefix file> <tree.cmake> [leaf list]" << std::endl;
        return 2;
    }

    auto const parents = read_parents(argv[1]);
    auto const all = all_leaves(parents);
    auto const expected = argc == 4 ? listed_leaves(argv[3], all) : all;

    // the generated leaves are the expected ones, in term order
    auto const generated = generated_leaves(argv[2]);
    if (generated.size() != expected.size() || tree::leaf_count != (int) expected.size())
        fail("leaves generated: " + std::to_string(generated.size()) + " (leaf_count "
             + std::to_string(tree::leaf_count) + "), expected " + std::to_string(expected.size()));
    for (size_t i = 0; i < std::min(generated.size(), expected.size()); ++i)
    {
        std::string const name = routing::symbols(expected[i], '_');
        if (generated[i] != name)
        {
            fail("leaf " + std::to_string(i) + " is " + generated[i] + ", expected " + name);
            break;
        }
    }

    // every probe reaches the reader's leaf for it, or where that leaf would be
    auto const terms = probes(parents);
    int previous{-1};
    std::string previous_term;
    for (auto const & term : terms)
    {
        std::string const leaf = routing::leaf_of(parents, term);
        int const want =
            (int) (std::lower_bound(expected.begin(), expected.end(), leaf, matchable::str_lt_str)
                   - expected.begin());
        bool found{false};
        int const got = tree::lookup(term, &found);
        bool const want_found = want < (int) expected.size() && expected[want] == leaf;
        if (got != want || found != want_found)
            fail(shown(term) + ": lookup " + std::to_string(got) + (found ? " found" : " not found")
                 + ", reader leaf " + routing::symbols(leaf, ' ') + " is " + std::to_string(want)
                 + (want_found ? " found" : " not found"));

        // term order is index order
        if (got < previous)
            fail(shown(term) + " sorts after " + shown(previous_term) + " but reaches "
                 + std::to_string(got) + " < " + std::to_string(previous));
        previous = got;
        previous_term = term;
    }

    std::cout << terms.size() << " terms, " << expected.size() << " of " << all.size() << " leaves, "
              << parents.size() << " parents: " << failures << " failures" << std::endl;
    return failures == 0 ? 0 : 1;
}
