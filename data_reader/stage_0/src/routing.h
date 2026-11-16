#pragma once

#include <cstdlib>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include <matchable/matchable.h>



// how the stage 0 reader places terms in leaves; test/routing_check.cpp checks that the generated tree
// (scripts/generate_code.py, src/tree.cpp) agrees
namespace routing
{
    // as in generate_code.py (MAX_PREFIX_DEPTH - 1)
    size_t constexpr MAX_PARENT_DEPTH{5};


    // reads the characters of a parent prefix file line into prefix; returns what is wrong with the line,
    // or "" if nothing is
    inline std::string parent_of_line(std::string const & line, std::string & prefix)
    {
        prefix.clear();
        std::istringstream symbols{line};
        for (std::string symbol; symbols >> symbol;)
        {
            std::string const ch = matchable::escapable::unescape_all(symbol);
            if (ch.size() != 1)
                return "invalid symbol '" + symbol + "'";
            prefix += ch;
        }
        if (prefix.size() > MAX_PARENT_DEPTH)
            return "parent prefix deeper than " + std::to_string(MAX_PARENT_DEPTH);
        return "";
    }


    // the leaf of term, given the parent prefixes (the root "" included); a term that ends above its leaf
    // continues with space, the first symbol
    inline std::string leaf_of(std::set<std::string> const & parents, std::string const & term)
    {
        std::string prefix;
        while (parents.count(prefix) > 0)
            prefix += prefix.size() < term.size() ? term[prefix.size()] : ' ';
        return prefix;
    }


    // the symbol names of prefix, as generate_code.py names them, joined by delim
    inline std::string symbols(std::string const & prefix, char delim)
    {
        std::string ret;

        if (prefix.empty())
        {
            std::cout << "routing::symbols('" << delim << "') --> prefix is empty!" << std::endl;
            abort();
        }

        for (size_t i = 0; i < prefix.size(); ++i)
        {
            std::string escaped = matchable::escapable::escape(prefix.substr(i, 1));
            if (escaped.size() > 2)
            {
                if (escaped[0] != escaped[escaped.size() - 1])
                {
                    std::cout << "routing::symbols('" << delim << "')\n"
                              << "    --> escaped.size() > 2 but first and last do not match!\n"
                              << "    --> prefix is: '" << prefix << "'" << std::endl;
                    abort();
                }
            }
            else if (escaped.size() != 1)
            {
                std::cout << "routing::symbols('" << delim << "')\n"
                          << "    --> escape failed with: '" << escaped << "'\n"
                          << "    -->          prefix is: '" << prefix << "'" << std::endl;
                abort();
            }

            if (i > 0)
                ret += delim;

            // manually explicitly escape numbers and upper case letters
            if (
                (prefix[i] >= '0' && prefix[i] <= '9') ||
                (prefix[i] >= 'A' && prefix[i] <= 'Z')
            )
                ret += "esc_";

            ret += escaped;
        }

        return ret;
    }
}
