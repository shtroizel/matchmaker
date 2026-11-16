cmake_minimum_required(VERSION 3.16.3)

# matchmaker checks, labeled by the point in the build they check:
#     generator  scripts/generate_code.py and src/tree.cpp, with no data read
#     reader0    the stage 0 data reader
#
# cmake -S test -B <build dir> && cmake --build <build dir> && ctest --test-dir <build dir> -L <label>

project(matchmaker_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(MATCHMAKER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GENERATE_CODE ${MATCHMAKER_ROOT}/scripts/generate_code.py)
set(FIXTURES ${CMAKE_CURRENT_SOURCE_DIR}/fixtures)

include_directories(
    ${MATCHMAKER_ROOT}/matchable/include
    ${MATCHMAKER_ROOT}/data_reader/stage_0/src
    ${MATCHMAKER_ROOT}/src
)

enable_testing()

add_executable(sparse_leaf_list sparse_leaf_list.cpp)
target_compile_options(sparse_leaf_list PRIVATE -Wall -Werror -Wextra)


# routing_<name>: the tree generate_code.py makes from <parents> (all leaves, or "sparse": those
# sparse_leaf_list names) must send every term to the leaf the stage 0 reader puts it in
function(add_routing_test name parents leaves)
    set(workspace ${CMAKE_CURRENT_BINARY_DIR}/tree_${name})
    set(generate_args -p -w ${workspace} -t ${parents})
    set(check_args ${parents} ${workspace}/tree.cmake)
    set(depends ${parents} ${GENERATE_CODE})
    if (leaves STREQUAL "sparse")
        set(leaf_list ${workspace}/leaf_list)
        add_custom_command(
            OUTPUT ${leaf_list}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${workspace}
            COMMAND sparse_leaf_list ${parents} > ${leaf_list}
            DEPENDS sparse_leaf_list ${parents}
            VERBATIM
        )
        list(APPEND generate_args -l ${leaf_list})
        list(APPEND check_args ${leaf_list})
        list(APPEND depends ${leaf_list})
    endif()

    add_custom_command(
        OUTPUT ${workspace}/generated_src/tree_data.cpp ${workspace}/tree.cmake
        COMMAND ${CMAKE_COMMAND} -E make_directory ${workspace}/generated_src
        COMMAND ${Python3_EXECUTABLE} ${GENERATE_CODE} ${generate_args}
        DEPENDS ${depends}
        VERBATIM
    )

    add_executable(routing_${name}
        routing_check.cpp
        ${MATCHMAKER_ROOT}/src/tree.cpp
        ${workspace}/generated_src/tree_data.cpp
    )
    target_compile_options(routing_${name} PRIVATE -Wall -Werror -Wextra)
    set_source_files_properties(${workspace}/generated_src/tree_data.cpp PROPERTIES
        COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/stub_leaves.h"
    )

    add_test(NAME routing_${name} COMMAND routing_${name} ${check_args})
    set_tests_properties(routing_${name} PROPERTIES LABELS "generator;reader0")
endfunction()

add_routing_test(small ${FIXTURES}/parents_small.txt all)
add_routing_test(small_sparse ${FIXTURES}/parents_small.txt sparse)
add_routing_test(full ${MATCHMAKER_ROOT}/scripts/parent_prefixes.txt all)
add_routing_test(full_sparse ${MATCHMAKER_ROOT}/scripts/parent_prefixes.txt sparse)


# generate_code.py refuses bad input with a message naming the problem
function(add_generator_refusal_test name expected)
    add_test(
        NAME generator_refuses_${name}
        COMMAND ${Python3_EXECUTABLE} ${GENERATE_CODE}
                -p -w ${CMAKE_CURRENT_BINARY_DIR}/refused_${name} ${ARGN}
    )
    set_tests_properties(generator_refuses_${name} PROPERTIES
        LABELS generator
        PASS_REGULAR_EXPRESSION "${expected}"
    )
endfunction()

add_generator_refusal_test(too_deep "parent prefix deeper than 5" -t ${FIXTURES}/parents_too_deep.txt)
add_generator_refusal_test(bad_symbol "unknown symbol \"xyz\"" -t ${FIXTURES}/parents_bad_symbol.txt)
add_generator_refusal_test(orphan "parent prefix \"a b\" lacks parent \"a\""
    -t ${FIXTURES}/parents_orphan.txt
)
add_generator_refusal_test(not_a_leaf "1 leaves are not leaves of the tree"
    -t ${FIXTURES}/parents_small.txt -l ${FIXTURES}/leaf_list_not_a_leaf.txt
)


# the stage 0 reader refuses the parent prefix files generate_code.py refuses (read through the reader's
# routing::parent_of_line by sparse_leaf_list)
function(add_reader_refusal_test name expected parents)
    add_test(NAME reader_refuses_${name} COMMAND sparse_leaf_list ${parents})
    set_tests_properties(reader_refuses_${name} PROPERTIES
        LABELS reader0
        PASS_REGULAR_EXPRESSION "${expected}"
    )
endfunction()

add_reader_refusal_test(too_deep "parent prefix deeper than 5" ${FIXTURES}/parents_too_deep.txt)
add_reader_refusal_test(bad_symbol "invalid symbol 'xyz'" ${FIXTURES}/parents_bad_symbol.txt)
