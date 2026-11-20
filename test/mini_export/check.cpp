// mini_export_check <unit 1> <unit 2>
// Linked with a matchmaker library built with build_and_install.py -e, and compiled against the type header
// it installs for LEAF. Loads units of words on top of the library: each unit's words are found with their
// values while it is loaded, the leaf is back to the library's own words once it is unloaded, and the
// library's terms are unchanged throughout.
#include <dlfcn.h>

#include <iostream>
#include <string>

#include <matchmaker/matchmaker.h>

#include LEAF_TYPE_HEADER



namespace
{
    int failures = 0;

    void check(bool ok, std::string const & what)
    {
        if (!ok)
        {
            ++failures;
            std::cout << "FAILED: " << what << std::endl;
        }
    }

    std::string words()
    {
        std::string s;
        for (auto const & w : LEAF::variants_by_string())
            s += w.as_string() + " ";
        return s;
    }

    // every word's by-string index is its position, the order is by string, and each word looks itself up
    bool consistent()
    {
        auto const & ws = LEAF::variants_by_string();
        for (int i = 0; i < (int) ws.size(); ++i)
        {
            if (ws[i].as_by_string_index() != i)
                return false;
            if (i > 0 && !ws[i - 1].lt_by_string(ws[i]))
                return false;
            if (LEAF::from_string(ws[i].as_string()) != ws[i])
                return false;
        }
        return true;
    }

    // the library's own terms, which units of words leave alone
    bool library_terms(int count)
    {
        bool found = false;
        return mm_count() == count && mm_lookup("About", &found) >= 0 && found;
    }

    int ordinal_summation(char const * word)
    {
        LEAF::Type const w = LEAF::from_string(word);
        return w.is_nil() ? -1 : w.as_ordinal_summation();
    }

    void * load(char const * path)
    {
        void * unit = dlopen(path, RTLD_NOW | RTLD_LOCAL);
        check(nullptr != unit, std::string("dlopen ") + path + ": " + (unit ? "" : dlerror()));
        return unit;
    }

    // true once the unit is no longer mapped
    bool unload(void * unit, char const * path)
    {
        if (nullptr != unit)
            dlclose(unit);
        return nullptr == dlopen(path, RTLD_NOW | RTLD_LOCAL | RTLD_NOLOAD);
    }
}



int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: mini_export_check <unit 1> <unit 2>" << std::endl;
        return 2;
    }

    std::string const library_words = words();
    size_t const library_count = LEAF::variants_by_string().size();
    check(library_count > 0, "the leaf has words");
    int const term_count = mm_count();
    check(library_terms(term_count), "the library has About");
    check(ordinal_summation("Abzz1") == -1, "Abzz1 is not a word before loading");

    void * unit = load(argv[1]);
    check(ordinal_summation("Abzz1") == 81, "unit 1 adds Abzz1 (81)");
    check(ordinal_summation("Abzz2") == 82, "unit 1 adds Abzz2 (82)");
    check(LEAF::variants_by_string().size() == library_count + 2, "unit 1 adds two words");
    check(consistent(), "consistent with unit 1");
    check(unload(unit, argv[1]), "unit 1 unloads");
    check(words() == library_words, "only the library's words after unit 1");
    check(consistent(), "consistent after unit 1");

    unit = load(argv[2]);
    check(ordinal_summation("Abzz1") == 91, "unit 2 adds Abzz1 (91)");
    check(ordinal_summation("Abzz2") == -1, "unit 2 lacks Abzz2");
    check(ordinal_summation("Abzz3") == 93, "unit 2 adds Abzz3 (93)");
    check(consistent(), "consistent with unit 2");
    check(unload(unit, argv[2]), "unit 2 unloads");
    check(words() == library_words, "only the library's words after unit 2");
    check(library_terms(term_count), "the library's terms are unchanged");

    std::cout << (failures == 0 ? "ok" : "failed") << std::endl;
    return failures == 0 ? 0 : 1;
}
