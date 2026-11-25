// relations_check
// Checks that the library answers each term's links as alphabetic indexes (src/relations.cpp): every list
// holds its terms in order, repeats included, and a term without a list has none.
// It is linked with src/tree.cpp and the three leaves of fixtures/relations, whose words refer to words of
// their own leaf and of the others, and the indexes come from those references.
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <relations.h>
#include <tree.h>



namespace
{
    char const * const terms[] = {"a", "Ab", "abc", "b", "bee", "c", "cat"};
    int const term_count = (int) (sizeof(terms) / sizeof(terms[0]));

    // each term's links, by the indexes of terms
    std::map<std::pair<relations::Kind, int>, std::vector<int>> const expected{
        {{relations::synonyms, 0}, {6, 1}},
        {{relations::embedded, 2}, {0, 3, 5}},
        {{relations::definition, 2}, {0, 0}},
        {{relations::synonyms, 4}, {4}},
        {{relations::antonyms, 4}, {0}},
        {{relations::antonyms, 5}, {2}},
        {{relations::synonyms, 6}, {0, 2, 4, 6}},
    };

    std::vector<int> links(relations::Kind kind, int term)
    {
        auto const iter = expected.find({kind, term});
        return iter == expected.end() ? std::vector<int>{} : iter->second;
    }
}



namespace
{
    int failures{0};

    void fail(std::string const & what)
    {
        ++failures;
        std::cout << "FAIL: " << what << std::endl;
    }


    std::string joined(std::vector<int> const & indexes)
    {
        std::string s;
        for (int i : indexes)
            s += " " + std::to_string(i);
        return s;
    }
}



int main()
{
    if (tree::count() != term_count)
        fail("tree::count() is " + std::to_string(tree::count()));
    for (int i = 0; i < term_count; ++i)
    {
        bool found{false};
        if (tree::lookup(terms[i], std::strlen(terms[i]), &found) != i || !found)
            fail(std::string(terms[i]) + " is not at " + std::to_string(i));
    }

    char const * const kinds[] = {"synonyms", "antonyms", "embedded", "definition"};

    // twice: computed from references, the second pass reads what the first one computed
    for (int pass = 0; pass < 2; ++pass)
    {
        for (int kind = 0; kind < relations::kind_count; ++kind)
        {
            for (int i = 0; i < term_count; ++i)
            {
                std::vector<int> const want = links((relations::Kind) kind, i);
                int const * got{nullptr};
                int count{-1};
                relations::get((relations::Kind) kind, i, &got, &count);
                std::vector<int> const have(got, got + (count > 0 && nullptr != got ? count : 0));
                if (count != (int) want.size() || have != want)
                    fail(std::string(kinds[kind]) + " of " + terms[i] + " are" + joined(have) + " (" +
                         std::to_string(count) + "), not" + joined(want));
            }
        }
    }

    std::cout << (failures == 0 ? "relations: ok" : "relations: FAILED") << std::endl;
    return failures == 0 ? 0 : 1;
}
