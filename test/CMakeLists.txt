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
#     toolchain  option parsing and the source check in build_toolchain.py (nothing is built)
#     mini_build build_and_install.py on the fixture data, then the library it installs, again with -e,
#                then units of words loaded on top, again with -q, again with -S (the system
#                compiler), and stage 1 again with -r -f, on the same data and on moved book text
#                (about 10 minutes; build_and_install.py runs every label but this one before its
#                stage 0 data reader). The pinned toolchain builds the others, and these tests build
#                with this compiler. The toolchain is the one in -DMM_TOOLCHAIN=<LLVM install> (or
#                MM_TOOLCHAIN in the environment), else build_toolchain.py's, which the first run
#                builds (about 7 CPU hours, once per user)
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
            --time_compiles=t --linker=lld --leaf_budget=10 --toolchain=t --system_compiler -h
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

add_test(
    NAME options_toolchain_needs_dir
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --toolchain
)
set_tests_properties(options_toolchain_needs_dir PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option --toolchain requires argument"
)
add_test(
    NAME options_toolchain_needs_compiler
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --toolchain=${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(options_toolchain_needs_compiler PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^--toolchain must name an LLVM install: no bin/clang\\+\\+ in "
)
add_test(NAME options_refuse_c COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} -c -h)
set_tests_properties(options_refuse_c PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^option -c not recognized"
)
add_test(
    NAME options_toolchain_refuses_system
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --toolchain=t -S
)
set_tests_properties(options_toolchain_refuses_system PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^--toolchain names the pinned toolchain and can't be combined with -S"
)
add_test(
    NAME options_jobs_needs_number
    COMMAND ${Python3_EXECUTABLE} ${BUILD_AND_INSTALL} --jobs=many
)
set_tests_properties(options_jobs_needs_number PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION "^--jobs must be a whole number of at least 1"
)

# toolchain: build_toolchain.py names its directory under the cache directory, and refuses a source
# release that isn't the pinned one before building anything
set(BUILD_TOOLCHAIN ${MATCHMAKER_ROOT}/scripts/build_toolchain.py)
set(TOOLCHAIN_CACHE ${CMAKE_CURRENT_BINARY_DIR}/toolchain_cache)
add_test(
    NAME toolchain_long_values
    COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN} --jobs=2 --tarball=t --print_dir -h
)
add_test(NAME toolchain_print_dir COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN} -p)
add_test(
    NAME toolchain_tarball_needs_file
    COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN} --tarball=${CMAKE_CURRENT_BINARY_DIR}/no_such_file
)
add_test(
    NAME toolchain_refuses_other_source
    COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN} --tarball=${CMAKE_CURRENT_LIST_FILE}
)
set_tests_properties(toolchain_long_values toolchain_print_dir toolchain_tarball_needs_file
    toolchain_refuses_other_source
    PROPERTIES LABELS toolchain ENVIRONMENT MM_TOOLCHAIN_CACHE=${TOOLCHAIN_CACHE}
)
set_tests_properties(toolchain_print_dir PROPERTIES
    PASS_REGULAR_EXPRESSION "^${TOOLCHAIN_CACHE}/llvm-[0-9.]+-[0-9a-f]+\n$"
)
set_tests_properties(toolchain_tarball_needs_file PROPERTIES
    PASS_REGULAR_EXPRESSION "^--tarball must name the LLVM source release: no "
)
set_tests_properties(toolchain_refuses_other_source PROPERTIES
    PASS_REGULAR_EXPRESSION "CMakeLists.txt is not the pinned LLVM release"
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
# the pinned toolchain builds every mini build: the one MM_TOOLCHAIN names, else build_toolchain.py's,
# which mini_toolchain_ready builds first when it isn't there yet
set(MM_TOOLCHAIN "$ENV{MM_TOOLCHAIN}" CACHE PATH "LLVM install holding the pinned toolchain")
if (MM_TOOLCHAIN)
    set(MINI_TOOLCHAIN_DIR ${MM_TOOLCHAIN})
    add_test(NAME mini_toolchain_ready COMMAND ${CMAKE_COMMAND} -E md5sum ${MM_TOOLCHAIN}/bin/clang++)
else()
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN} -p
        OUTPUT_VARIABLE MINI_TOOLCHAIN_DIR OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY
    )
    add_test(NAME mini_toolchain_ready COMMAND ${Python3_EXECUTABLE} ${BUILD_TOOLCHAIN})
endif()
message(STATUS "mini builds use the pinned toolchain in ${MINI_TOOLCHAIN_DIR}")
set(MINI_TOOLCHAIN --toolchain=${MINI_TOOLCHAIN_DIR})
set_tests_properties(mini_toolchain_ready PROPERTIES
    LABELS mini_build FIXTURES_SETUP mini_toolchain TIMEOUT 43200
)
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
            ${MINI_TOOLCHAIN}
            --data_dir=${READER0_DATA} --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
            --time_compiles=${MINI_BUILD_COMPILES}
)
set_tests_properties(mini_build PROPERTIES
    LABELS mini_build
    FIXTURES_REQUIRED "mini_build_start;mini_toolchain"
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
# mini_library_one_check: --check=locations runs the locations check alone; mini_library_unknown_check: a
# name that belongs to no check is refused with the names that do
set(MINI_OTHER_CHECKS
    "terms|longest|ordinal_summation|completion|lookup_any_case|no_allocations|out_of_range"
)
add_test(
    NAME mini_library_one_check
    COMMAND mini_library_check --check=locations ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_library_one_check PROPERTIES
    LABELS mini_build FIXTURES_REQUIRED mini_library RESOURCE_LOCK mini_install
    PASS_REGULAR_EXPRESSION "check locations: 0 problems"
    FAIL_REGULAR_EXPRESSION "check (${MINI_OTHER_CHECKS}): "
)
add_test(
    NAME mini_library_unknown_check
    COMMAND mini_library_check --check=spelling ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_library_unknown_check PROPERTIES
    LABELS options
    PASS_REGULAR_EXPRESSION
        "^no check is named spelling\n.*\nchecks: terms longest .* locations out_of_range\n$"
)

# mini_toolchain: the pinned toolchain built everything mini_build installed
add_test(
    NAME mini_toolchain
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/toolchain_check.py ${MINI_TOOLCHAIN_DIR}
            ${MINI_BUILD}/install/lib/matchmaker
)
set_tests_properties(mini_toolchain PROPERTIES
    LABELS mini_build FIXTURES_REQUIRED mini_library RESOURCE_LOCK mini_install
)

# mini_rerun: stage 1 again (-r -f) on mini_build's build with the same data, which compiles nothing in the
# workspace, then mini_rerun_library checks what it installs; it rewrites the install that mini_library
# reads, so those tests take turns with it
set(MINI_RERUN_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/mini_rerun_compiles.txt)
add_test(NAME mini_rerun_compiles_clear COMMAND ${CMAKE_COMMAND} -E rm -f ${MINI_RERUN_COMPILES})
set_tests_properties(mini_rerun_compiles_clear PROPERTIES LABELS mini_build FIXTURES_SETUP mini_rerun_start)
add_test(
    NAME mini_rerun
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -r -f
            ${MINI_TOOLCHAIN} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
            --leaf_budget=${MINI_LEAF_BUDGET}
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

# mini_moved: stage 1 again (-r -f) on the same build, with the text of two posts trading places. No leaf
# holds a location, so mini_moved_compiles finds the arrays compiled again and nothing of the workspace,
# and mini_moved_library finds the text where it now is. mini_moved_back runs stage 1 on the fixture data
# again (timing compiles too, which keeps the build's compile commands) and mini_moved_back_library checks
# that the first library is back; the tests that read the install run before all of these
set(MINI_MOVED_DATA ${CMAKE_CURRENT_BINARY_DIR}/mini_moved_data)
set(MINI_MOVED_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/mini_moved_compiles.txt)
add_test(
    NAME mini_moved_data
    COMMAND ${CMAKE_COMMAND} -DDATA=${READER0_DATA} -DOUT=${MINI_MOVED_DATA}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/moved_text_data.cmake
)
add_test(NAME mini_moved_compiles_clear COMMAND ${CMAKE_COMMAND} -E rm -f ${MINI_MOVED_COMPILES})
set_tests_properties(mini_moved_data mini_moved_compiles_clear PROPERTIES
    LABELS mini_build FIXTURES_SETUP mini_moved_start
)
add_test(
    NAME mini_moved
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -r -f
            ${MINI_TOOLCHAIN} -j ${MINI_BUILD_JOBS} --data_dir=${MINI_MOVED_DATA}
            --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
            --time_compiles=${MINI_MOVED_COMPILES}
)
set_tests_properties(mini_moved PROPERTIES
    LABELS mini_build
    FIXTURES_REQUIRED "mini_library;mini_moved_start"
    FIXTURES_SETUP mini_moved
    DEPENDS "mini_library;mini_library_by_spelling;mini_rerun;mini_rerun_library"
    TIMEOUT 3600
)
add_test(
    NAME mini_moved_compiles
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mini_rerun_check.py
            ${MINI_MOVED_COMPILES} ${MINI_BUILD}/build moved_text
)
add_test(
    NAME mini_moved_library
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library_moved_text.txt
            $<TARGET_FILE:mini_library_check> ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_moved_compiles mini_moved_library PROPERTIES
    LABELS mini_build FIXTURES_REQUIRED mini_moved
)
add_test(
    NAME mini_moved_back
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -r -f
            ${MINI_TOOLCHAIN} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
            --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_BUILD}/build -i ${MINI_BUILD}/install
            --time_compiles=${MINI_MOVED_COMPILES}
)
set_tests_properties(mini_moved_back PROPERTIES
    LABELS mini_build FIXTURES_CLEANUP mini_moved FIXTURES_SETUP mini_moved_back TIMEOUT 3600
)
add_test(
    NAME mini_moved_back_library
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library.txt
            $<TARGET_FILE:mini_library_check> ${MINI_BUILD}/install/lib/matchmaker/libmatchmaker.so
)
set_tests_properties(mini_moved_back_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_moved_back)
set_tests_properties(mini_moved mini_moved_library mini_moved_back mini_moved_back_library
    PROPERTIES RESOURCE_LOCK mini_install)

# mini_q: the same build with -q, which installs under the _q names (matchmaker_q), and mini_q_library runs
# mini_library_check's checks on what it installs
set(MINI_Q ${CMAKE_CURRENT_BINARY_DIR}/mini_q)
add_test(
    NAME mini_q
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -q
            ${MINI_TOOLCHAIN} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
            --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_Q}/build -i ${MINI_Q}/install
)
set_tests_properties(mini_q PROPERTIES
    LABELS mini_build FIXTURES_REQUIRED mini_toolchain FIXTURES_SETUP mini_q TIMEOUT 3600
)
add_test(
    NAME mini_q_library
    COMMAND mini_library_check ${MINI_Q}/install/lib/matchmaker_q/libmatchmaker.so
)
set_tests_properties(mini_q_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_q)

# mini_system: the same build with -S, the system compiler in place of the toolchain; mini_system_library
# checks that its library matches mini_library, and mini_system_compiler that the toolchain check tells
# the two apart
set(MINI_SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/mini_system)
add_test(
    NAME mini_system
    COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -S
            -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA} --leaf_budget=${MINI_LEAF_BUDGET}
            -b ${MINI_SYSTEM}/build -i ${MINI_SYSTEM}/install
)
set_tests_properties(mini_system PROPERTIES LABELS mini_build FIXTURES_SETUP mini_system TIMEOUT 3600)
add_test(
    NAME mini_system_library
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.py
            ${READER0_EXPECTED}/mini_library.txt
            $<TARGET_FILE:mini_library_check> ${MINI_SYSTEM}/install/lib/matchmaker/libmatchmaker.so
)
add_test(
    NAME mini_system_compiler
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/toolchain_check.py ${MINI_TOOLCHAIN_DIR}
            ${MINI_SYSTEM}/install/lib/matchmaker
)
set_tests_properties(mini_system_library PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_system)
set_tests_properties(mini_system_compiler PROPERTIES
    LABELS mini_build FIXTURES_REQUIRED "mini_system;mini_toolchain" WILL_FAIL TRUE
)

# mini_export: the same build with -e, which exports every symbol and installs a type header per leaf;
# mini_export_units builds two units of words against that install with the toolchain that built it, and
# loads, unloads and swaps them
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(MINI_EXPORT ${CMAKE_CURRENT_BINARY_DIR}/mini_export)
    add_test(
        NAME mini_export
        COMMAND ${Python3_EXECUTABLE} ${MATCHMAKER_ROOT}/scripts/build_and_install.py -s -e
                ${MINI_TOOLCHAIN} -j ${MINI_BUILD_JOBS} --data_dir=${READER0_DATA}
                --leaf_budget=${MINI_LEAF_BUDGET} -b ${MINI_EXPORT}/build -i ${MINI_EXPORT}/install
    )
    set_tests_properties(mini_export PROPERTIES
        LABELS mini_build FIXTURES_REQUIRED mini_toolchain FIXTURES_SETUP mini_export TIMEOUT 3600
    )
    add_test(
        NAME mini_export_units
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mini_export_check.py
                ${MINI_TOOLCHAIN_DIR} ${MINI_EXPORT}/install ${MINI_EXPORT}/units
    )
    set_tests_properties(mini_export_units PROPERTIES LABELS mini_build FIXTURES_REQUIRED mini_export)
endif()
