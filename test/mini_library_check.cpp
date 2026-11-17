// mini_library_check <libmatchmaker.so>
// Loads a matchmaker library built from small data and checks how its parts agree: every term looks itself
// up, terms are in strict order, cross-links (synonyms, antonyms, embedded words, definitions) are terms,
// the longest and ordinal summation tables agree with the terms, every book location holds the term that
// lists it (a phrase as an ancestor of the word there) and every book word lists its location, and
// completion finds exactly the terms with each prefix. Prints every term with its properties, then each
// book, for expect_output.py to compare.
#include <dlfcn.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <matchable/matchable.h>



#define MM_FUNCTIONS(X)                                                                                   \
    X(mm_count, int, (void))                                                                              \
    X(mm_lookup, int, (char const *, bool *))                                                             \
    X(mm_at, char const *, (int, int *))                                                                  \
    X(mm_as_longest, int, (int))                                                                          \
    X(mm_from_longest, int, (int))                                                                        \
    X(mm_lengths, void, (int const * *, int *))                                                           \
    X(mm_length_location, bool, (int, int *, int *))                                                      \
    X(mm_ordinal_summation, int, (int))                                                                   \
    X(mm_from_ordinal_summation, void, (int, int const * *, int *))                                       \
    X(mm_parts_of_speech, bool, (int, char const * const * *, int8_t const * *, int *))                  \
    X(mm_is_name, bool, (int))                                                                            \
    X(mm_is_male_name, bool, (int))                                                                       \
    X(mm_is_female_name, bool, (int))                                                                     \
    X(mm_is_place, bool, (int))                                                                           \
    X(mm_is_compound, bool, (int))                                                                        \
    X(mm_is_acronym, bool, (int))                                                                         \
    X(mm_is_phrase, bool, (int))                                                                          \
    X(mm_is_used_in_book, bool, (int, int))                                                               \
    X(mm_synonyms, void, (int, int const * *, int *))                                                     \
    X(mm_antonyms, void, (int, int const * *, int *))                                                     \
    X(mm_embedded, void, (int, int const * *, int *))                                                     \
    X(mm_definition, void, (int, int const * *, int *))                                                   \
    X(mm_locations, void, (int, int const * *, int const * *, int const * *, int const * *, int *))       \
    X(mm_complete, void, (char const *, int *, int *))                                                    \
    X(mm_book_count, int, (void))                                                                         \
    X(mm_book_title, void, (int, int const * *, int *))                                                   \
    X(mm_book_author, void, (int, int const * *, int *))                                                  \
    X(mm_chapter_count, int, (int))                                                                       \
    X(mm_chapter_title, void, (int, int, int const * *, int *))                                           \
    X(mm_chapter_subtitle, void, (int, int, int const * *, int *))                                        \
    X(mm_paragraph_count, int, (int, int))                                                                \
    X(mm_word_count, int, (int, int, int))                                                                \
    X(mm_word, int, (int, int, int, int, int const * *, int *, int *, bool *))

#define MM_DECLARE(name, ret, args) ret (*name) args = nullptr;
MM_FUNCTIONS(MM_DECLARE)



namespace
{
    std::vector<std::string> problems;


    void problem(std::string const & message)
    {
        if (problems.size() < 20)
            std::cerr << message << std::endl;
        problems.push_back(message);
    }


    std::string term(int index)
    {
        if (index < 0 || index >= mm_count())
            return "<" + std::to_string(index) + ">";
        return mm_at(index, nullptr);
    }


    std::vector<int> as_vector(int const * values, int count)
    {
        return std::vector<int>(values, values + count);
    }


    // the terms at indexes, as [a, b, c], checking each is a term
    std::string terms(std::vector<int> const & indexes, std::string const & what, int of)
    {
        std::string s = "[";
        for (size_t i = 0; i < indexes.size(); ++i)
        {
            if (indexes[i] < 0 || indexes[i] >= mm_count())
                problem(what + " of " + term(of) + " is not a term: " + std::to_string(indexes[i]));
            s += (i == 0 ? "" : ", ") + term(indexes[i]);
        }
        return s + "]";
    }


    std::string words(int const * indexes, int count, std::string const & what)
    {
        std::string s;
        for (int i = 0; i < count; ++i)
        {
            if (indexes[i] < 0 || indexes[i] >= mm_count())
                problem(what + " holds a non-term: " + std::to_string(indexes[i]));
            s += term(indexes[i]);
        }
        return s;
    }


    void check_terms()
    {
        int const count = mm_count();
        for (int i = 0; i < count; ++i)
        {
            int length = -1;
            std::string const t = mm_at(i, &length);
            if (length != (int) t.size())
                problem("mm_at length of " + t + " is " + std::to_string(length));
            bool found = false;
            if (mm_lookup(t.c_str(), &found) != i || !found)
                problem("mm_lookup(mm_at(" + std::to_string(i) + ")) does not find " + t);
            if (i > 0 && !matchable::str_lt_str(mm_at(i - 1, nullptr), t))
                problem("terms out of order: " + term(i - 1) + ", " + t);
        }
    }


    void check_longest()
    {
        int const count = mm_count();
        std::set<int> seen;
        int previous_length = -1;
        for (int l = 0; l < count; ++l)
        {
            int const i = mm_from_longest(l);
            if (i < 0 || i >= count || !seen.insert(i).second)
            {
                problem("mm_from_longest(" + std::to_string(l) + ") is not a new term: " +
                        std::to_string(i));
                continue;
            }
            if (mm_as_longest(i) != l)
                problem("mm_as_longest(mm_from_longest(" + std::to_string(l) + ")) is not " +
                        std::to_string(l));
            int const length = (int) term(i).size();
            if (previous_length != -1 && length > previous_length)
                problem("longest order is not longest first at " + term(i));
            previous_length = length;
        }

        int const * lengths = nullptr;
        int lengths_count = 0;
        mm_lengths(&lengths, &lengths_count);
        int covered = 0;
        for (int k = 0; k < lengths_count; ++k)
        {
            int location = -1;
            int with_length = 0;
            if (!mm_length_location(lengths[k], &location, &with_length))
            {
                problem("mm_length_location(" + std::to_string(lengths[k]) + ") not found");
                continue;
            }
            for (int l = location; l < location + with_length; ++l)
                if ((int) term(mm_from_longest(l)).size() != lengths[k])
                    problem("length " + std::to_string(lengths[k]) + " holds " + term(mm_from_longest(l)));
            covered += with_length;
        }
        if (covered != count)
            problem("mm_lengths covers " + std::to_string(covered) + " of " + std::to_string(count) +
                    " terms");
    }


    // the sum of the term's letters' ordinal values, upper and lower case alike, nothing else counted
    int ordinal_summation(std::string const & t)
    {
        int sum = 0;
        for (char ch : t)
            if (ch >= 'a' && ch <= 'z')
                sum += ch - 'a' + 1;
            else if (ch >= 'A' && ch <= 'Z')
                sum += ch - 'A' + 1;
        return sum;
    }


    void check_ordinal_summation()
    {
        for (int i = 0; i < mm_count(); ++i)
        {
            if (mm_ordinal_summation(i) != ordinal_summation(term(i)))
                problem("ordinal summation of " + term(i) + " is " +
                        std::to_string(mm_ordinal_summation(i)));
            int const * same = nullptr;
            int same_count = 0;
            mm_from_ordinal_summation(mm_ordinal_summation(i), &same, &same_count);
            std::vector<int> const group = as_vector(same, same_count);
            if (std::find(group.begin(), group.end(), i) == group.end())
                problem(term(i) + " is not among the terms with its ordinal summation");
            for (int j : group)
                if (j < 0 || j >= mm_count() || mm_ordinal_summation(j) != mm_ordinal_summation(i))
                    problem("ordinal summation " + std::to_string(mm_ordinal_summation(i)) + " lists " +
                            term(j));
        }
    }


    void check_completion()
    {
        std::set<std::string> prefixes{""};
        for (int i = 0; i < mm_count(); ++i)
        {
            std::string const t = term(i);
            for (size_t n = 1; n <= t.size(); ++n)
                prefixes.insert(t.substr(0, n));
        }
        for (auto const & prefix : prefixes)
        {
            int start = -1;
            int length = -1;
            mm_complete(prefix.c_str(), &start, &length);
            int expected_start = -1;
            int expected_length = 0;
            for (int i = 0; i < mm_count(); ++i)
                if (term(i).rfind(prefix, 0) == 0)
                {
                    if (expected_start == -1)
                        expected_start = i;
                    ++expected_length;
                }
            if (start != expected_start || length != expected_length)
                problem("mm_complete(\"" + prefix + "\") gives " + std::to_string(start) + " +" +
                        std::to_string(length) + ", expected " + std::to_string(expected_start) + " +" +
                        std::to_string(expected_length));
        }
    }


    // stage 1 records no locations for single characters but Q and q, the articles the, The, an and An, and
    // the Crumbs markup http, >>, ~~~, ~~: and ]~~
    bool has_locations(std::string const & t)
    {
        static std::set<std::string> const skipped{
            "the", "The", "an", "An", "http", ">>", "~~~", "~~:", "]~~"
        };
        return (t.size() != 1 || t == "Q" || t == "q") && skipped.count(t) == 0;
    }


    // every location a term lists holds that term (or a phrase starting there), and every book word that
    // has locations lists its own
    void check_locations()
    {
        std::set<std::tuple<int, int, int, int, int>> listed;
        for (int i = 0; i < mm_count(); ++i)
        {
            int const * b = nullptr;
            int const * c = nullptr;
            int const * p = nullptr;
            int const * w = nullptr;
            int count = 0;
            mm_locations(i, &b, &c, &p, &w, &count);
            for (int k = 0; k < count; ++k)
            {
                listed.insert({b[k], c[k], p[k], w[k], i});
                int const * ancestors = nullptr;
                int ancestor_count = 0;
                int const word =
                    mm_word(b[k], c[k], p[k], w[k], &ancestors, &ancestor_count, nullptr, nullptr);
                std::vector<int> const phrases = as_vector(ancestors, ancestor_count);
                if (word != i && std::find(phrases.begin(), phrases.end(), i) == phrases.end())
                    problem(term(i) + " lists a location holding " + term(word));
            }
            bool used = false;
            for (int book = 0; book < mm_book_count(); ++book)
                used = used || mm_is_used_in_book(book, i);
            if (count > 0 && !used)
                problem(term(i) + " has locations but is not used in any book");
        }
        for (int b = 0; b < mm_book_count(); ++b)
            for (int c = 0; c < mm_chapter_count(b); ++c)
                for (int p = 0; p < mm_paragraph_count(b, c); ++p)
                    for (int w = 0; w < mm_word_count(b, c, p); ++w)
                    {
                        int const i = mm_word(b, c, p, w, nullptr, nullptr, nullptr, nullptr);
                        bool const should = has_locations(term(i));
                        if (should != (listed.count({b, c, p, w, i}) == 1))
                            problem("book word " + term(i) + (should ? " does not list" : " lists") +
                                    " its location");
                    }
    }


    void print_terms()
    {
        std::cout << mm_count() << " terms\n";
        for (int i = 0; i < mm_count(); ++i)
        {
            std::cout << "[" << term(i) << "]";

            char const * const * pos_names = nullptr;
            int8_t const * pos_flags = nullptr;
            int pos_count = 0;
            if (mm_parts_of_speech(i, &pos_names, &pos_flags, &pos_count))
            {
                std::cout << " pos=";
                for (int k = 0; k < pos_count; ++k)
                    if (pos_flags[k])
                        std::cout << pos_names[k] << ";";
            }
            std::cout << " os=" << mm_ordinal_summation(i);
            if (mm_is_name(i)) std::cout << " name";
            if (mm_is_male_name(i)) std::cout << " male_name";
            if (mm_is_female_name(i)) std::cout << " female_name";
            if (mm_is_place(i)) std::cout << " place";
            if (mm_is_compound(i)) std::cout << " compound";
            if (mm_is_acronym(i)) std::cout << " acronym";
            if (mm_is_phrase(i)) std::cout << " phrase";

            int const * linked = nullptr;
            int linked_count = 0;
            mm_synonyms(i, &linked, &linked_count);
            if (linked_count > 0)
                std::cout << " syn=" << terms(as_vector(linked, linked_count), "synonym", i);
            mm_antonyms(i, &linked, &linked_count);
            if (linked_count > 0)
                std::cout << " ant=" << terms(as_vector(linked, linked_count), "antonym", i);
            mm_embedded(i, &linked, &linked_count);
            if (linked_count > 0)
                std::cout << " embedded=" << terms(as_vector(linked, linked_count), "embedded word", i);
            mm_definition(i, &linked, &linked_count);
            if (linked_count > 0)
                std::cout << " definition=" << terms(as_vector(linked, linked_count), "definition word", i);

            int const * b = nullptr;
            int const * c = nullptr;
            int const * p = nullptr;
            int const * w = nullptr;
            int count = 0;
            mm_locations(i, &b, &c, &p, &w, &count);
            if (count > 0)
            {
                std::cout << " at=";
                for (int k = 0; k < count; ++k)
                    std::cout << (k == 0 ? "" : ",") << b[k] << "." << c[k] << "." << p[k] << "." << w[k];
            }
            std::cout << "\n";
        }
    }


    void print_books()
    {
        for (int b = 0; b < mm_book_count(); ++b)
        {
            int const * text = nullptr;
            int count = 0;
            mm_book_title(b, &text, &count);
            std::cout << "book " << b << ": " << words(text, count, "book title");
            mm_book_author(b, &text, &count);
            std::cout << " by " << words(text, count, "book author") << ", " << mm_chapter_count(b)
                      << " chapters\n";
            for (int c = 0; c < mm_chapter_count(b); ++c)
            {
                mm_chapter_title(b, c, &text, &count);
                std::cout << "  chapter " << c << ": " << words(text, count, "chapter title");
                mm_chapter_subtitle(b, c, &text, &count);
                std::cout << " | " << words(text, count, "chapter subtitle") << "\n";
                for (int p = 0; p < mm_paragraph_count(b, c); ++p)
                {
                    std::cout << "    " << p << ":";
                    for (int w = 0; w < mm_word_count(b, c, p); ++w)
                    {
                        int const * ancestors = nullptr;
                        int ancestor_count = 0;
                        int within = -1;
                        bool referenced = false;
                        int const i =
                            mm_word(b, c, p, w, &ancestors, &ancestor_count, &within, &referenced);
                        std::cout << " [" << term(i) << "]";
                        if (referenced)
                            std::cout << "r";
                        if (ancestor_count > 0 && ancestors[0] != -1)
                            std::cout << "<" << term(ancestors[0]) << "@" << within << ">";
                    }
                    std::cout << "\n";
                }
            }
        }
    }
}



int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <libmatchmaker.so>" << std::endl;
        return 2;
    }
    void * library = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (library == nullptr)
    {
        std::cerr << "dlopen: " << dlerror() << std::endl;
        return 2;
    }
#define MM_RESOLVE(name, ret, args)                                                                       \
    name = reinterpret_cast<ret (*) args>(dlsym(library, #name));                                         \
    if (name == nullptr)                                                                                  \
    {                                                                                                     \
        std::cerr << "missing " #name << std::endl;                                                       \
        return 2;                                                                                         \
    }
    MM_FUNCTIONS(MM_RESOLVE)

    check_terms();
    check_longest();
    check_ordinal_summation();
    check_completion();
    check_locations();
    print_terms();
    print_books();

    if (!problems.empty())
    {
        std::cerr << problems.size() << " problems" << std::endl;
        return 1;
    }
    return 0;
}
