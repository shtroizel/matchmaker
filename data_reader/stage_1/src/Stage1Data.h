#pragma once



#ifdef MATCHABLE_OMIT_BY_INDEX
matchable with by_index required for stage 1 reader
#endif
#include <matchable/matchable.h>

#include <deque>
#include <functional>
#include <map>

#include <matchmaker/data_reader_common.h>


struct BookWord
{
    int word{-1};
    std::deque<int> ancestors;
    int first_ancestor_start_index{-1};
    int index_within_first_ancestor{-1};
    bool referenced{false};
};

struct Chapter
{
    std::vector<int> title;
    std::vector<int> subtitle;
    std::vector<std::vector<BookWord>> paragraphs;
};

struct Book
{
    std::vector<int> title;
    std::vector<int> author;
    std::vector<Chapter> chapters;
};

struct SynAnt
{
    std::vector<std::string> syn;
    std::vector<std::string> ant;
};


MATCHABLE(
    word_status,
    not_printable_ascii,
    has_spaces,
    has_hyphens,
    has_other_symbols
)


using SynAntTable = std::map<std::string, SynAnt>;
using IndexTable = std::vector<std::vector<int>>;
using KeyWords = std::vector<int>;

PROPERTYx9_MATCHABLE(
    std::string, workspace_dir,
    std::string, data_dir,
    SynAntTable, syn_ant_table,
    IndexTable, embedded_words,
    IndexTable, definitions,
    std::vector<int>, by_longest,
    std::vector<Book>, books,
    KeyWords, key_words,
    int, thread_count,

    Stage1Data
)
