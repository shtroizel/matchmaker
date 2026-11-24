// defines the array named by MM_ARRAY from the stage 1 reader's file of that name

#ifdef STAGE0
std::span<int const> const arrays::MM_ARRAY{};
#else
// aligned for the ints the reader wrote
alignas(int) static unsigned char const MM_CAT(MM_ARRAY, _bytes)[] = {
#embed MM_STR(arrays/MM_ARRAY.i32) if_empty(0)
};
std::span<int const> const arrays::MM_ARRAY{
    reinterpret_cast<int const *>(MM_CAT(MM_ARRAY, _bytes)),
    sizeof(MM_CAT(MM_ARRAY, _bytes)) / sizeof(int)
};
#endif

#undef MM_ARRAY
