# cmake -DMATCHMAKER_ROOT=<root> -P link_pool_check.cmake: the link memory estimate and the link pool depth
include(${MATCHMAKER_ROOT}/workspace_common/link_pool.cmake)

function(expect actual expected what)
    if(NOT "${actual}" STREQUAL "${expected}")
        message(FATAL_ERROR "${what}: expected ${expected}, got ${actual}")
    endif()
endfunction()

# a quarter of the full build: 85,998,803 header bytes linked at 10,128 MiB with gold
mm_link_estimate_mib(estimate 85998803 ON)
expect(${estimate} 10661 "gold estimate for a quarter")
mm_link_estimate_mib(estimate 85998803 OFF)
expect(${estimate} 14762 "GNU ld estimate for a quarter")
mm_link_estimate_mib(estimate 0 ON)
expect(${estimate} 0 "estimate for no headers")

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
