#pragma once



// each term's links to other terms as alphabetic indexes. The leaves hold them as references to words,
// which stay as they are when other words come and go. The indexes come from those references, all of
// them when the first is asked for
namespace relations
{
    enum Kind
    {
        synonyms,
        antonyms,
        embedded,
        definition,
        kind_count
    };

    // index must be within [0, tree::count())
    void get(Kind kind, int index, int const * * terms, int * count);
}
