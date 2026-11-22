# cmake -DMATCHMAKER_ROOT=<root> -P link_pool_check.cmake: the linker choice, the link memory estimate and
# the link pool depth
include(${MATCHMAKER_ROOT}/workspace_common/link_pool.cmake)

function(expect actual expected what)
    if(NOT "${actual}" STREQUAL "${expected}")
        message(FATAL_ERROR "${what}: expected ${expected}, got ${actual}")
    endif()
endfunction()

# a quarter of the full build: 85,998,803 header bytes linked at 10,128 MiB with gold, 8,995 with lld 23
# and 7,206 with mold
mm_link_estimate_mib(estimate 85998803 gold)
expect(${estimate} 10661 "gold estimate for a quarter")
mm_link_estimate_mib(estimate 85998803 lld)
expect(${estimate} 9431 "lld estimate for a quarter")
mm_link_estimate_mib(estimate 85998803 mold)
expect(${estimate} 7791 "mold estimate for a quarter")
mm_link_estimate_mib(estimate 85998803 "")
expect(${estimate} 14762 "estimate for the compiler's own linker")
mm_link_estimate_mib(estimate 85998803 bfd)
expect(${estimate} 14762 "estimate for another linker")
mm_link_estimate_mib(estimate 0 gold)
expect(${estimate} 0 "estimate for no headers")

# the linker: the one asked for, else gold when installed (not on sparc64), else the compiler's own
mm_choose_linker(linker "" ON x86_64)
expect("${linker}" "gold" "gold when installed")
mm_choose_linker(linker "" OFF x86_64)
expect("${linker}" "" "the compiler's own without gold")
mm_choose_linker(linker "" ON sparc64)
expect("${linker}" "" "the compiler's own on sparc64")
mm_choose_linker(linker lld ON x86_64)
expect("${linker}" "lld" "lld when asked for")
mm_choose_linker(linker mold OFF sparc64)
expect("${linker}" "mold" "the one asked for, anywhere")
mm_choose_linker(linker default ON x86_64)
expect("${linker}" "" "default asks for the compiler's own")

mm_link_pool_depth(depth 31744 10661)
expect(${depth} 1 "depth with 31 GiB")
mm_link_pool_depth(depth 63488 10661)
expect(${depth} 2 "depth with 62 GiB")
mm_link_pool_depth(depth 15872 10661)
expect(${depth} 1 "depth never below 1")
mm_link_pool_depth(depth 31744 300)
expect(${depth} 52 "depth with small links")
mm_link_pool_depth(depth 31744 0)
expect(${depth} 1 "depth with no estimate")

mm_link_fits(fits 31744 10661)
expect(${fits} TRUE "a quarter fits half of 31 GiB")
mm_link_fits(fits 15872 10661)
expect(${fits} FALSE "a quarter does not fit half of 15.5 GiB")
