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
    // the characters of a parent prefix file line; false, with prefix set to the symbol, if a symbol is not
    // one character
    inline bool parent_of_line(std::string const & line, std::string & prefix)
    {
        prefix.clear();
        std::istringstream symbols{line};
        for (std::string symbol; symbols >> symbol;)
        {
            std::string const ch = matchable::escapable::unescape_all(symbol);
            if (ch.size() != 1)
            {
                prefix = symbol;
                return false;
            }
            prefix += ch;
        }
        return true;
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
