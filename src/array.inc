// defines the array named by MM_ARRAY from the stage 1 reader's file of that name

#ifdef STAGE0
std::span<int const> const arrays::MM_ARRAY{};
#else
// the 0 comes first so that an empty file still leaves an array
static int const MM_CAT(MM_ARRAY, _values)[] = {
    0,
#include MM_STR(arrays/MM_ARRAY.inc)
};
std::span<int const> const arrays::MM_ARRAY{
    MM_CAT(MM_ARRAY, _values) + 1,
    sizeof(MM_CAT(MM_ARRAY, _values)) / sizeof(int) - 1
};
#endif

#undef MM_ARRAY
