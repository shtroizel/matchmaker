// defines the array named by MM_UNITS, which the stage 1 reader writes in units: a source file each, and
// the list of them this includes

#ifdef STAGE0
std::span<int const * const> const arrays::MM_UNITS{};
#else
namespace arrays
{
#define MM_UNIT(unit) extern int const unit[];
#include MM_STR(arrays/MM_UNITS.inc)
#undef MM_UNIT
}
// the nullptr comes first so that an empty list still leaves an array
static int const * const MM_CAT(MM_UNITS, _units)[] = {
    nullptr,
#define MM_UNIT(unit) arrays::unit,
#include MM_STR(arrays/MM_UNITS.inc)
#undef MM_UNIT
};
std::span<int const * const> const arrays::MM_UNITS{
    MM_CAT(MM_UNITS, _units) + 1,
    sizeof(MM_CAT(MM_UNITS, _units)) / sizeof(int const *) - 1
};
#endif

#undef MM_UNITS
