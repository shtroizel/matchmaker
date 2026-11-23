cmake_minimum_required(VERSION 3.16.3)

# matchmaker checks, labeled by the point in the build they check:
#     generator  scripts/generate_code.py and src/tree.cpp (the leaf table), with no data read
#     options    option parsing in the build scripts (-h comes last, so nothing is built)
#     time_compiles  scripts/time_compile.py, the launcher build_and_install.py --time_compiles runs
#                each compile through
#     link       workspace_common's linker choice, link pool estimate and library size check
#     order      order.py's sort rules on fixtures/order, and its today rule against matchable's
#     reader0    the stage 0 data reader, on small fixture data
#     full_data  the stage 0 data reader on all of data_reader/data (about 45 s; -LE full_data skips it)
#     mini_build build_and_install.py on the fixture data, then the library it installs, again with -e,
#                then units of words loaded on top, again with -q, again with --linker=lld when the
#                compiler can link with lld, and stage 1 again with -r -f (about 15 minutes;
#                build_and_install.py runs every label but this one before its stage 0 data reader)
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
    ${MATCHMAKER_ROOT}/include
    ${MATCHMAKER_ROOT}/data_reader/stage_0/src
    ${MATCHMAKER_ROOT}/src
)

enable_testing()

add_executable(sparse_leaf_list sparse_leaf_list.cpp)
target_compile_options(sparse_leaf_list PRIVATE -Wall -Werror -Wextra)


# routing_<name>: the leaf table generate_code.py makes from a stage 0 leaf list must send every term to the
# last leaf whose first word is not after it
function(add_routing_test name leaf_list)
    set(workspace ${CMAKE_CURRENT_BINARY_DIR}/tree_${name})
    add_custom_command(
        OUTPUT ${workspace}/generated_src/tree_data.cpp ${workspace}/tree.cmake
        COMMAND ${CMAKE_COMMAND} -E make_directory ${workspace}/generated_src
        COMMAND ${Python3_EXECUTABLE} ${GENERATE_CODE} -p -w ${workspace} -l ${leaf_list}
        DEPENDS ${leaf_list} ${GENERATE_CODE}
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

    add_test(NAME routing_${name} COMMAND routing_${name} ${leaf_list} ${workspace}/tree.cmake)
    set_tests_properties(routing_${name} PROPERTIES LABELS "generator;reader0")
endfunction()

add_routing_test(small ${FIXTURES}/leaf_list_small.txt)


# generate_code.py refuses bad leaf lists with a message naming the problem
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

add_generator_refusal_test(no_list "a leaf list is required")
add_generator_refusal_test(empty "no leaves" -l ${FIXTURES}/leaf_list_empty.txt)
add_generator_refusal_test(no_word "line 2: expected a leaf name and its first word"
    -l ${FIXTURES}/leaf_list_no_word.txt
)
add_generator_refusal_test(bad_name "line 2: leaf name \"bad-name\" is not letters, digits and underscores"
    -l ${FIXTURES}/leaf_list_bad_name.txt
)
add_generator_refusal_test(twice "line 2: leaf name \"l00\" appears twice"
    -l ${FIXTURES}/leaf_list_twice.txt
)


# leaf_cuts: the stage 0 reader's leaf cut rule and leaf names
add_executable(leaf_cuts_check leaf_cuts_check.cpp)
target_compile_options(leaf_cuts_check PRIVATE -Wall -Werror -Wextra)
add_test(NAME leaf_cuts COMMAND leaf_cuts_check)
set_tests_properties(leaf_cuts PROPERTIES LABELS reader0)


# the stage 0 reader refuses bad parent prefix files, which still sort words into buckets while it reads
# (read through the reader's routing::parent_of_line by sparse_leaf_list)
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



# the build scripts take each long option that has a value in its --name=value form, and
# build_and_install.py refuses options it doesn't have
set(BUILD_AND_INSTALL ${MATCHMAKER_ROOT}/scripts/build_and_install.py)
add_test(
    NAME options_long_values
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --jobs=2 --build_dir=b --install_dir=i --debug
            --time_compiles=t --linker=lld --leaf_budget=10 -h
)
add_test(
    NAME options_long_values_stage_0
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/data_reader/stage_0/scripts/build_and_install.py
            --build_dir=b --install_dir=i --matchable_dir=m --jobs=2 -h
)
add_test(
    NAME options_long_values_stage_1
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/data_reader/stage_1/scripts/build_and_install.py
            --build_dir=b --install_dir=i --matchable_dir=m --stage_0_dir=s --jobs=2 -h
)
add_test(NAME options_refuse_m COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} -m -h)
add_test(
    NAME options_time_compiles_needs_file
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --time_compiles
)
set_tests_properties(options_time_compiles_needs_file PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option --time_compiles requires argument"
)

add_test(
    NAME options_linker_needs_name
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --linker
)
set_tests_properties(options_linker_needs_name PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option --linker requires argument"
)
add_test(
    NAME options_leaf_budget_needs_number
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --leaf_budget=ten -h
)
set_tests_properties(options_leaf_budget_needs_number PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^--leaf_budget must be a whole number of at least 1"
)

# link: the linker choice and the pool that keeps leaf library links within the machine's memory, a linker
# the compiler can't use, and the warning for a library whose loaded sections grow too large
add_test(
    NAME link_refuses_unknown
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/linker_check
            -B ${CMAKE_CURRENT_BINARY_DIR}/linker_check -DMATCHMAKER_ROOT=${MATCHMAKER_ROOT}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DMM_LINKER=no_such_linker
)
set_tests_properties(link_refuses_unknown PROPERTIES
    LABELS link
    PASS_REGULAR_EXPRESSION "can't link with -fuse-ld=no_such_linker \\(MM_LINKER\\)"
)
add_test(
    NAME link_pool
    COMMAND ${CMAKE_COMMAND} -DMATCHMAKER_ROOT=${MATCHMAKER_ROOT}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/link_pool_check.cmake
)
find_program(SIZE_TOOL NAMES size llvm-size)
add_test(
    NAME link_size_warns
    COMMAND ${CMAKE_COMMAND} -DLIB=$<TARGET_FILE:sparse_leaf_list> -DLIMIT=1 -DSIZE=${SIZE_TOOL}
            -P ${MATCHMAKER_ROOT}/workspace_common/check_lib_size.cmake
)
add_test(
    NAME link_size_quiet
    COMMAND ${CMAKE_COMMAND} -DLIB=$<TARGET_FILE:sparse_leaf_list> -DLIMIT=1000000000 -DSIZE=${SIZE_TOOL}
            -P ${MATCHMAKER_ROOT}/workspace_common/check_lib_size.cmake
)
set_tests_properties(link_pool link_size_warns link_size_quiet PROPERTIES LABELS link)
set_tests_properties(link_size_warns PROPERTIES
    PASS_REGULAR_EXPRESSION "sparse_leaf_list loads [0-9]+ bytes"
)
set_tests_properties(link_size_quiet PROPERTIES FAIL_REGULAR_EXPRESSION "loads")

# time_compiles: scripts/time_compile.py, the compiler launcher behind build_and_install.py --time_compiles
add_test(
    NAME time_compile
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/time_compile_check.py
            ${MATCHMAKER_ROOT}/scripts/time_compile.py ${CMAKE_CXX_COMPILER}
            ${CMAKE_CURRENT_BINARY_DIR}/time_compile
)
set_tests_properties(time_compile PROPERTIES LABELS time_compiles)
add_test(
    NAME options_refuse_memory_usage_limit
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --memory_usage_limit -h
)
set_tests_properties(options_long_values options_long_values_stage_0 options_long_values_stage_1
    PROPERTIES LABELS options
)
set_tests_properties(options_refuse_m PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option -m not recognized"
)
set_tests_properties(options_refuse_memory_usage_limit PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option --memory_usage_limit not recognized"
)




# order: order.py sorts fixtures/order/words.txt by each rule as fixtures/order/<rule>.txt shows
# (bytes.txt matches LC_ALL=C sort), check passes only sorted lines, and matchmaker's word order
# (word_order.h) sorts the ASCII words as order.py's folded rule does (Unicode folding comes later)
set(ORDER_FIXTURES ${FIXTURES}/order)
foreach(rule today bytes folded)
    add_test(
        NAME order_sort_${rule}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
                ${ORDER_FIXTURES}/${rule}.txt
                ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/order.py
                sort ${rule} ${ORDER_FIXTURES}/words.txt
    )
    add_test(
        NAME order_check_${rule}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/order.py check ${rule}
                ${ORDER_FIXTURES}/${rule}.txt
    )
    add_test(
        NAME order_check_${rule}_refuses
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/order.py check ${rule}
                ${ORDER_FIXTURES}/words.txt
    )
    set_tests_properties(order_sort_${rule} order_check_${rule} PROPERTIES LABELS order)
    set_tests_properties(order_check_${rule}_refuses PROPERTIES LABELS order WILL_FAIL TRUE)
endforeach()
add_executable(order_word_check order_word_check.cpp)
target_compile_options(order_word_check PRIVATE -Wall -Werror -Wextra)
add_test(
    NAME order_word_matches_folded
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${ORDER_FIXTURES}/folded_ascii.txt $<TARGET_FILE:order_word_check> ${ORDER_FIXTURES}/words.txt
)
set_tests_properties(order_word_matches_folded PROPERTIES LABELS order)



# mini_build: the whole matchmaker build (build_and_install.py, both stages, without these tests) from the
# fixture data, about 4 minutes; mini_library checks the library it installs and compares a dump of it with
# expected/mini_library.txt; mini_build_compiles checks the --time_compiles log it writes
set(MINI_BUILD ${CMAKE_CURRENT_BINARY_DIR}/mini_build)
# a small budget, so the fixture words fill several leaves and routing between them is exercised
set(MINI_LEAF_BUDGET 10)
set(MINI_BUILD_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/mini_build_compiles.txt)
include(ProcessorCount)
ProcessorCount(MINI_BUILD_JOBS)
if (MINI_BUILD_JOBS EQUAL 0)
    set(MINI_BUILD_JOBS 1)
endif()
add_test(NAME mini_build_compiles_clear COMMAND ${CMAKE_COMMAND} -E rm -f ${MINI_BUILD_COMPILES})
set_tests_properties(mini_build_compiles_clear PROPERTIES LABELS mini_build FIXTURES_SETUP mini_build_start)
add_test(
    NAME mini_build
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -j ${MINI_BUILD_JOBS}
            --data_dir=${READER0_DATA} --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
            --time_compiles=${MINI_BUILD_COMPILES}
)
set_tests_properties(mini_build PROPERTIES
    LABELS mini_build
    FIXTURES_REQUIRED mini_build_start
    FIXTURES_SETUP mini_library
    TIMEOUT 3600
)
add_test(
    NAME mini_build_compiles
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mini_build_compiles_check.py
            ${MINI_BUILD_COMPILES} ${MINI_BUILD}/build
)
set_tests_properties(mini_build_compiles PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_library)

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
add_test(
    NAME mini_library_by_spelling
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library_by_spelling.txt
            $<TARGET_FILE:mini_library_check> --by-spelling
            ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_library_by_spelling PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_library)

# mini_rerun: stage 1 again (-r -f) on mini_build's build with the same data, which compiles nothing in the
# workspace, then mini_rerun_library checks what it installs; it rewrites the install that mini_library
# reads, so those tests take turns with it
set(MINI_RERUN_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/mini_rerun_compiles.txt)
add_test(NAME mini_rerun_compiles_clear COMMAND ${CMAKE_COMMAND} -E rm -f ${MINI_RERUN_COMPILES})
set_tests_properties(mini_rerun_compiles_clear PROPERTIES LABELS mini_build FIXTURES_SETUP mini_rerun_start)
add_test(
    NAME mini_rerun
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -r -f
            -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA} --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
            --time_compiles=${MINI_RERUN_COMPILES}
)
set_tests_properties(mini_rerun PROPERTIES
    LABELS mini_build
    FIXTURES_REQUIRED "mini_library;mini_rerun_start"
    FIXTURES_SETUP mini_rerun
    TIMEOUT 3600
)
add_test(
    NAME mini_rerun_compiles
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mini_rerun_check.py
            ${MINI_RERUN_COMPILES} ${MINI_BUILD}/build
)
set_tests_properties(mini_rerun_compiles PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_rerun)
add_test(
    NAME mini_rerun_library
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library.txt
            $<TARGET_FILE:mini_library_check> ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_rerun_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_rerun)
set_tests_properties(mini_library mini_library_by_spelling mini_rerun mini_rerun_library
    PROPERTIES RESOURCE_LOCK mini_install)

# mini_q: the same build with -q, which installs under the _q names (matchmaker_q), and mini_q_library runs
# mini_library_check's checks on what it installs
set(MINI_Q ${CMAKE_CURRENT_BINARY_DIR}/mini_q)
add_test(
    NAME mini_q
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -q
            -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA} --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_Q}/build -i ${MINI_Q}/install
)
set_tests_properties(mini_q PROPERTIES LABELS mini_build FIXTURES_SETUP mini_q TIMEOUT 3600)
add_test(
    NAME mini_q_library
    COMMAND mini_library_check ${MINI_Q}/install/lib/matchmaker_q/libmatchmaker.so
)
set_tests_properties(mini_q_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_q)

# mini_export: the same build with -e, which exports every symbol and installs a type header per leaf;
# mini_export_units builds two units of words against that install and loads, unloads and swaps them
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(MINI_EXPORT ${CMAKE_CURRENT_BINARY_DIR}/mini_export)
    # the units are compiled with this compiler, so the library is too
    set(MINI_EXPORT_CLANG "")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(MINI_EXPORT_CLANG -c)
    endif()
    add_test(
        NAME mini_export
        COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -e
                ${MINI_EXPORT_CLANG} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
                --leaf_budget=${MINI_LEAF_BUDGET} -b ${MINI_EXPORT}/build -i ${MINI_EXPORT}/install
    )
    set_tests_properties(mini_export PROPERTIES LABELS mini_build FIXTURES_SETUP mini_export TIMEOUT 3600)
    add_test(
        NAME mini_export_units
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mini_export_check.py ${CMAKE_CXX_COMPILER}
                ${CMAKE_CXX_COMPILER_ID} ${MINI_EXPORT}/install ${MINI_EXPORT}/units
    )
    set_tests_properties(mini_export_units PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_export)
endif()

# mini_lld: the same build with --linker=lld, when this compiler can link with lld; mini_lld_linker checks
# that lld linked every library, and mini_lld_library that the library matches mini_library
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_LINK_OPTIONS -fuse-ld=lld)
check_cxx_source_compiles("int main() { return 0; }" LINKS_WITH_LLD)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if (LINKS_WITH_LLD AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(MINI_LLD ${CMAKE_CURRENT_BINARY_DIR}/mini_lld)
    set(MINI_LLD_CLANG "")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(MINI_LLD_CLANG -c)
    endif()
    add_test(
        NAME mini_lld
        COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s --linker=lld
                ${MINI_LLD_CLANG} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
                --leaf_budget=${MINI_LEAF_BUDGET} -b ${MINI_LLD}/build -i ${MINI_LLD}/install
    )
    set_tests_properties(mini_lld PROPERTIES LABELS mini_build FIXTURES_SETUP mini_lld TIMEOUT 3600)
    add_test(
        NAME mini_lld_linker
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/linker_comment_check.py LLD
                ${MINI_LLD}/install/lib/matchmaker
    )
    add_test(
        NAME mini_lld_library
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
                ${READER0_EXPECTED}/mini_library.txt
                $<TARGET_FILE:mini_library_check> ${MINI_LLD}/install/lib/matchmaker/libmatchmaker.so
    )
    set_tests_properties(mini_lld_linker mini_lld_library PROPERTIES
        LABELS mini_build FIXTURES_REQUIRED mini_lld
    )
else()
    message(STATUS "mini_lld skipped: ${CMAKE_CXX_COMPILER} can't link with -fuse-ld=lld")
endif()
