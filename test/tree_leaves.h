#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <matchable/matchable.h>

#include <routing.h>



// the parent prefixes of a parent prefix file as the stage 0 reader reads them, the root ("") included
inline std::set<std::string> read_parents(std::string const & filename)
{
    std::set<std::string> parents{""};
    std::ifstream f{filename};
    if (!f)
    {
        std::cerr << "cannot read " << filename << std::endl;
        exit(2);
    }
    for (std::string line; std::getline(f, line);)
    {
        std::string prefix;
        std::string const error = routing::parent_of_line(line, prefix);
        if (!error.empty())
        {
            std::cerr << filename << ": " << error << std::endl;
            exit(2);
        }
        if (!prefix.empty())
            parents.insert(prefix);
    }
    return parents;
}


// every leaf of the tree (a child of a parent that is not a parent itself), in term order
inline std::vector<std::string> all_leaves(std::set<std::string> const & parents)
{
    std::vector<std::string> leaves;
    for (std::string const & parent : parents)
        for (int c = 32; c <= 126; ++c)
            if (parents.count(parent + (char) c) == 0)
                leaves.push_back(parent + (char) c);
    std::sort(leaves.begin(), leaves.end(), matchable::str_lt_str);
    return leaves;
}
