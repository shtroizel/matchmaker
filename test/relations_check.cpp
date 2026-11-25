// relations_check
// Checks that the library answers each term's links as alphabetic indexes (src/relations.cpp): every list
// holds its terms in order, repeats included, and a term without a list has none.
// Without MM_GENERATED_RELATIONS it is linked with src/tree.cpp and the three leaves of fixtures/relations,
// whose words refer to words of their own leaf and of the others, and the indexes come from those
// references. With it, they come from arrays laid out here as the stage 1 reader writes them, each kind in
// blocks of another size.
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <relations.h>



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



#ifdef MM_GENERATED_RELATIONS

#include <arrays.h>

namespace
{
    // one kind's lists in blocks of 1 << shift terms: a block begins with where in it each of its terms'
    // list begins, and one entry more for where the last term's ends
    std::span<int const * const> blocks(relations::Kind kind, int shift)
    {
        static std::vector<std::vector<int>> storage[relations::kind_count];
        static std::vector<int const *> pointers[relations::kind_count];
        for (int first = 0; first < term_count; first += 1 << shift)
        {
            int const end = std::min(term_count, first + (1 << shift));
            std::vector<int> block;
            int start = end - first + 1;
            for (int term = first; term < end; ++term)
            {
                block.push_back(start);
                start += (int) links(kind, term).size();
            }
            block.push_back(start);
            for (int term = first; term < end; ++term)
                for (int linked : links(kind, term))
                    block.push_back(linked);
            storage[kind].push_back(block);
        }
        for (auto const & block : storage[kind])
            pointers[kind].push_back(block.data());
        return {pointers[kind].data(), pointers[kind].size()};
    }
}

int const arrays::synonyms_shift{0};
int const arrays::antonyms_shift{1};
int const arrays::embedded_shift{2};
int const arrays::definitions_shift{3};
std::span<int const * const> const arrays::synonyms{blocks(relations::synonyms, 0)};
std::span<int const * const> const arrays::antonyms{blocks(relations::antonyms, 1)};
std::span<int const * const> const arrays::embedded{blocks(relations::embedded, 2)};
std::span<int const * const> const arrays::definitions{blocks(relations::definition, 3)};

#else

#include <tree.h>

#endif



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
#ifndef MM_GENERATED_RELATIONS
    if (tree::count() != term_count)
        fail("tree::count() is " + std::to_string(tree::count()));
    for (int i = 0; i < term_count; ++i)
    {
        bool found{false};
        if (tree::lookup(terms[i], std::strlen(terms[i]), &found) != i || !found)
            fail(std::string(terms[i]) + " is not at " + std::to_string(i));
    }
#endif

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
