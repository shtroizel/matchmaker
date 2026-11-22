// leaf_cuts_check: the stage 0 reader's leaf cut rule (data_reader/stage_0/src/leaf_cuts.h) against its
// definition: published FNV-1a 64 values, leaf names computed independently, and cuts over generated word
// lists checked leaf by leaf
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <leaf_cuts.h>



namespace
{
    int failures{0};

    void expect(bool ok, std::string const & what)
    {
        if (!ok && ++failures <= 20)
            std::cout << "FAIL: " << what << std::endl;
    }


    // checks starts against the rule's definition: every leaf holds 1 to budget words, a leaf ends early
    // only before a boundary word once it holds the minimum, and no such cut is skipped
    void check_cuts(std::vector<std::string> const & words, int budget)
    {
        leaf_cuts::Rule const r = leaf_cuts::rule(budget);
        std::vector<size_t> const starts = leaf_cuts::starts(words, r);
        std::string const where = "budget " + std::to_string(budget) + ": ";
        expect(!starts.empty() && starts[0] == 0, where + "the first leaf starts at the first word");
        for (size_t l = 0; l < starts.size(); ++l)
        {
            size_t const end = l + 1 < starts.size() ? starts[l + 1] : words.size();
            int const size = (int) (end - starts[l]);
            expect(size >= 1 && size <= budget, where + "leaf " + std::to_string(l) + " holds " +
                                                std::to_string(size) + " words");
            for (size_t i = starts[l] + 1; i < end; ++i)
                expect((int) (i - starts[l]) < r.min_size || !leaf_cuts::is_boundary(words[i], r),
                       where + "no cut before boundary word " + words[i]);
            if (end < words.size() && size < budget)
                expect(size >= r.min_size && leaf_cuts::is_boundary(words[end], r),
                       where + "leaf " + std::to_string(l) + " ends early before " + words[end]);
        }
        expect(leaf_cuts::starts(words, r) == starts, where + "the same words give the same leaves");
    }
}



int main()
{
    // published FNV-1a 64 test values
    expect(leaf_cuts::fnv1a("") == 0xcbf29ce484222325ull, "fnv1a(\"\")");
    expect(leaf_cuts::fnv1a("a") == 0xaf63dc4c8601ec8cull, "fnv1a(\"a\")");

    leaf_cuts::Rule const r170 = leaf_cuts::rule(170);
    expect(r170.budget == 170 && r170.divisor == 60 && r170.min_size == 20, "rule(170) is 170, 60, 20");
    leaf_cuts::Rule const r10 = leaf_cuts::rule(10);
    expect(r10.divisor == 4 && r10.min_size == 1, "rule(10) is 10, 4, 1");
    leaf_cuts::Rule const r1 = leaf_cuts::rule(1);
    expect(r1.divisor == 1 && r1.min_size == 1, "rule(1) is 1, 1, 1");

    // names computed independently (Python, 2026-11-22)
    std::vector<std::pair<std::string, std::string>> const names{
        {"a", "a_dc4c8601ec8c"},
        {"Q", "Q_0c4c86023e1c"},
        {"Apple pie", "Appl_06fbd7a8a15d"},
        {" ~", "x20x7e_4a07b48e34b3"},
        {"caf\xc3\xa9", "cafxc3_823acfa40d89"},
    };
    for (auto const & [word, name] : names)
        expect(leaf_cuts::name(word) == name, "name of \"" + word + "\" is " + leaf_cuts::name(word));

    std::vector<std::string> words;
    char buf[16];
    for (int i = 0; i < 20000; ++i)
    {
        std::snprintf(buf, sizeof(buf), "w%05d", i);
        words.push_back(buf);
    }
    for (int budget : {1, 2, 10, 170, 400})
        check_cuts(words, budget);
    check_cuts({"only"}, 170);

    std::cout << failures << " failures" << std::endl;
    return failures == 0 ? 0 : 1;
}
