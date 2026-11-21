# Linking a leaf library takes memory in proportion to its leaves' generated headers: about 125 times their
# size with gold and 170 times with GNU ld (full build, 2026-11-21), so these ratios leave some margin

function(mm_link_estimate_mib out header_bytes gold)
    if(gold)
        set(ratio 130)
    else()
        set(ratio 180)
    endif()
    math(EXPR mib "${header_bytes} * ${ratio} / 1048576")
    set(${out} ${mib} PARENT_SCOPE)
endfunction()

# links share half of the machine's memory with the compiles still running beside them
function(mm_link_pool_depth out total_mib largest_mib)
    set(depth 1)
    if(largest_mib GREATER 0)
        math(EXPR depth "${total_mib} / 2 / ${largest_mib}")
        if(depth LESS 1)
            set(depth 1)
        endif()
    endif()
    set(${out} ${depth} PARENT_SCOPE)
endfunction()

function(mm_link_fits out total_mib largest_mib)
    math(EXPR budget "${total_mib} / 2")
    if(largest_mib GREATER budget)
        set(${out} FALSE PARENT_SCOPE)
    else()
        set(${out} TRUE PARENT_SCOPE)
    endif()
endfunction()
