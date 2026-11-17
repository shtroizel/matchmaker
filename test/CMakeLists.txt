cmake_minimum_required(VERSION 3.16.3)

# matchmaker checks, labeled by the point in the build they check:
#     generator  scripts/generate_code.py and src/tree.cpp, with no data read
#     reader0    the stage 0 data reader, on small fixture data
#     full_data  the stage 0 data reader on all of data_reader/data (about 45 s; -LE full_data skips it)
#
# reader0_* tests compare what the reader keeps with test/fixtures/reader0/expected; after a deliberate
# change, UPDATE_EXPECTED=1 ctest -R reader0_ rewrites those files (review the diff)
#
# cmake -S test -B <build dir> && cmake --build <build dir> && ctest --test-dir <build dir> -L <label>

project(matchmaker_tests)

# optimized like build_and_install.py builds the reader; unoptimized, reader0_full takes minutes
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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



# the stage 0 reader without its main, and MatchableMaker, which it saves leaves with; an object library,
# so every task's object is linked and sets its own run function, as in data_reader_stage_0
set(READER0_SRC ${MATCHMAKER_ROOT}/data_reader/stage_0/src)
file(GLOB reader0_srcs ${READER0_SRC}/*.cpp)
list(REMOVE_ITEM reader0_srcs ${READER0_SRC}/data_reader_stage_0.cpp)
add_library(matchablemaker_for_tests STATIC ${MATCHMAKER_ROOT}/matchable/src/MatchableMaker.cpp)
add_library(reader0 OBJECT ${reader0_srcs})
target_include_directories(reader0 PUBLIC
    ${READER0_SRC}
    ${MATCHMAKER_ROOT}/data_reader/stage_0/include
    ${MATCHMAKER_ROOT}/data_reader/common
)
target_compile_options(reader0 PUBLIC -Wall -Werror -Wextra)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(reader0 PUBLIC -Wno-dangling-reference)
endif()
target_link_libraries(reader0 PUBLIC matchablemaker_for_tests)

add_executable(reader0_fixture reader0_fixture.cpp)
target_link_libraries(reader0_fixture reader0)

# crumbs_hour: the post hour conversion both data readers share (data_reader/common)
add_executable(crumbs_hour_check crumbs_hour_check.cpp)
target_link_libraries(crumbs_hour_check reader0)
add_test(NAME crumbs_hour COMMAND crumbs_hour_check)
set_tests_properties(crumbs_hour PROPERTIES LABELS reader0)

set(READER0_DATA ${FIXTURES}/reader0/data)
set(READER0_EXPECTED ${FIXTURES}/reader0/expected)


# reader0_<name>: the words the stage 0 reader keeps from the fixture data for these tasks, in this
# dictionary mode, with their leaves and properties (test/fixtures/reader0/expected/<name>.txt)
function(add_reader0_test name mode)
    add_test(
        NAME reader0_${name}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
                ${READER0_EXPECTED}/${name}.txt
                $<TARGET_FILE:reader0_fixture> ${READER0_DATA} ${FIXTURES}/parents_small.txt ${mode}
                ${CMAKE_CURRENT_BINARY_DIR}/reader0_${name} ${ARGN}
    )
    set_tests_properties(reader0_${name} PROPERTIES LABELS reader0)
endfunction()

set(READ_3201
    "reading 3201 (SINGLE)" "reading 3201 (COMPOUND)" "reading 3201 (COMMON)" "reading 3201 (NAMES)"
    "reading 3201 (NAMES-F)" "reading 3201 (NAMES-M)" "reading 3201 (PLACES)" "reading 3201 (CROSSWD)"
    "reading 3201 (CRSWD D)" "reading 3201 (ACRONYMS)"
)
set(READ_DICTIONARIES ${READ_3201} "reading 3202" "reading 3203")

add_reader0_test(3201 all ${READ_3201})
add_reader0_test(3202 all "reading 3202")
add_reader0_test(3203 all "reading 3203")
add_reader0_test(keys all "reading Keys")
add_reader0_test(crumbs all "reading Crumbs")
add_reader0_test(all all ${READ_DICTIONARIES} "reading Crumbs" "reading Keys")
add_reader0_test(reduced reduced ${READ_DICTIONARIES} "reading Crumbs" "reading Keys")
add_reader0_test(none none ${READ_DICTIONARIES} "reading Crumbs" "reading Keys")

# reader0_saved: saving writes one header per leaf with words, declaring exactly its words and their
# property values, and the leaf list
add_test(
    NAME reader0_saved
    COMMAND reader0_fixture ${READER0_DATA} ${FIXTURES}/parents_small.txt all
            ${CMAKE_CURRENT_BINARY_DIR}/reader0_saved ${READ_DICTIONARIES} "reading Crumbs" "reading Keys"
            "saving matchables"
)
set_tests_properties(reader0_saved PROPERTIES
    LABELS reader0
    PASS_REGULAR_EXPRESSION "saved [0-9]+ leaves, [0-9]+ words, [0-9]+ values: ok"
)



# full_data: the stage 0 reader over all of data_reader/data with the real parent prefixes;
# reader0_full.py checks how the modes relate and prints counts compared with expected/full.txt
set(FULL_DATA ${CMAKE_CURRENT_BINARY_DIR}/full_data)
add_test(
    NAME full_data_unpack
    COMMAND ${CMAKE_COMMAND} -DDATA=${MATCHMAKER_ROOT}/data_reader/data -DOUT=${FULL_DATA}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/extract_data.cmake
)
set_tests_properties(full_data_unpack PROPERTIES LABELS full_data FIXTURES_SETUP full_data)

add_test(
    NAME reader0_full
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py ${READER0_EXPECTED}/full.txt
            ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/reader0_full.py $<TARGET_FILE:reader0_fixture>
            ${FULL_DATA} ${MATCHMAKER_ROOT}/scripts/parent_prefixes.txt
            ${CMAKE_CURRENT_BINARY_DIR}/reader0_full
)
set_tests_properties(reader0_full PROPERTIES LABELS full_data FIXTURES_REQUIRED full_data)



# mini_build: the whole matchmaker build (build_and_install.py, both stages, without these tests) from the
# fixture data, about 4 minutes; mini_library checks the library it installs and compares a dump of it with
# expected/mini_library.txt
set(MINI_BUILD ${CMAKE_CURRENT_BINARY_DIR}/mini_build)
include(ProcessorCount)
ProcessorCount(MINI_BUILD_JOBS)
if (MINI_BUILD_JOBS EQUAL 0)
    set(MINI_BUILD_JOBS 1)
endif()
add_test(
    NAME mini_build
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -j ${MINI_BUILD_JOBS}
            --data_dir=${READER0_DATA} -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
)
set_tests_properties(mini_build PROPERTIES LABELS mini_build FIXTURES_SETUP mini_library TIMEOUT 3600)

add_executable(mini_library_check mini_library_check.cpp)
target_compile_options(mini_library_check PRIVATE -Wall -Werror -Wextra)
target_link_libraries(mini_library_check ${CMAKE_DL_LIBS})
add_test(
    NAME mini_library
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library.txt
            $<TARGET_FILE:mini_library_check> ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_library)
