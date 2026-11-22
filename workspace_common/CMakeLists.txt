cmake_minimum_required(VERSION 3.16.3)

project(matchmaker VERSION 4.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dangling-reference")
endif()

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=medany")
endif()

option(ATOMIC_LIBS "builds a library for every prefix" OFF)
option(q "suffix lib & inc destination with _q" OFF)
option(STAGE0 "build stage 0" OFF)
option(EXPORT_ALL "export every symbol and install a type header per leaf, for loading units of words" OFF)

set(LEAFLIB_COUNT 4)

set(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")

set(lib_destination lib/matchmaker)
set(inc_destination include/matchmaker)
if(q)
    set(lib_destination "${lib_destination}_q")
    set(inc_destination "${inc_destination}_q")
endif()

if(STAGE0)
    add_definitions(-DSTAGE0)
endif()

if(NOT MATCHMAKER_ROOT)
    message(FATAL_ERROR "MATCHMAKER_ROOT must be set! please set to matchmaker repository root location")
endif()

add_definitions(-DMATCHABLE_OMIT_BY_INDEX)
find_package(matchable 15.1.0 REQUIRED)
include_directories(${matchable_DIR}/../../../include)

set(mm_exports_map ${MATCHMAKER_ROOT}/workspace_common/mm_exports.map)
macro(export_mm_only target)
    if(NOT EXPORT_ALL)
        target_link_options(${target} PRIVATE -Wl,--version-script=${mm_exports_map})
    endif()
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS ${mm_exports_map})
endmacro()

include(${MATCHMAKER_ROOT}/workspace_common/link_pool.cmake)
set(MM_LINKER "" CACHE STRING
    "linker for the libraries (-fuse-ld=): gold, lld, mold or default; empty picks gold when installed")
find_program(ld_gold ld.gold)
set(gold_found OFF)
if(ld_gold)
    set(gold_found ON)
endif()
mm_choose_linker(linker "${MM_LINKER}" ${gold_found} "${CMAKE_SYSTEM_PROCESSOR}")
mm_check_linker("${linker}")
if(linker)
    message(STATUS "linker: ${linker}")
else()
    message(STATUS "linker: the compiler's own")
endif()
find_program(size_tool NAMES size llvm-size)
set(lib_size_limit 1610612736)
macro(finish_link target)
    if(linker)
        target_link_options(${target} PRIVATE -fuse-ld=${linker})
    endif()
    if(size_tool)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -DLIB=$<TARGET_FILE:${target}> -DLIMIT=${lib_size_limit}
                    -DSIZE=${size_tool} -P ${MATCHMAKER_ROOT}/workspace_common/check_lib_size.cmake
            VERBATIM
        )
    endif()
endmacro()

set(generated_matchables ${CMAKE_CURRENT_SOURCE_DIR}/generated_include/matchmaker/generated_matchables)
file(GLOB_RECURSE leaf_headers ${generated_matchables}/*.h)
foreach(leaf_header ${leaf_headers})
    get_filename_component(prefix ${leaf_header} NAME_WE)
    file(SIZE ${leaf_header} header_bytes_${prefix})
endforeach()
set(largest_link_mib 0)

set(lib_list)
macro(add_lib_finalization lib_name)
    list(APPEND lib_list matchmaker_${lib_name})
    set(lib_header_bytes 0)
    foreach(prefix ${ARGN})
        if(DEFINED header_bytes_${prefix})
            math(EXPR lib_header_bytes "${lib_header_bytes} + ${header_bytes_${prefix}}")
        endif()
    endforeach()
    mm_link_estimate_mib(lib_link_mib ${lib_header_bytes} "${linker}")
    if(lib_link_mib GREATER largest_link_mib)
        set(largest_link_mib ${lib_link_mib})
    endif()
    finish_link(matchmaker_${lib_name})
    target_include_directories(
        matchmaker_${lib_name}
        PRIVATE
        generated_include
        INTERFACE
        $<BUILD_INTERFACE:${MATCHMAKER_ROOT}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated_include>
        $<INSTALL_INTERFACE:include>
    )
    set_target_properties(matchmaker_${lib_name} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    export_mm_only(matchmaker_${lib_name})
    install(
        TARGETS matchmaker_${lib_name}
        DESTINATION ${lib_destination}
        EXPORT matchmaker
    )
endmacro()


# defines matchmaker_srcs (the tree tables) and prefix_list (leaves)
include(${CMAKE_CURRENT_SOURCE_DIR}/tree.cmake)

# books first
file(GLOB book_srcs "generated_src/books/*.cpp")
foreach(book_src ${book_srcs})
    list(PREPEND matchmaker_srcs ${book_src})
endforeach()

# except for these
list(PREPEND matchmaker_srcs ${MATCHMAKER_ROOT}/src/matchmaker.cpp ${MATCHMAKER_ROOT}/src/tree.cpp)

if(ATOMIC_LIBS)
    foreach(prefix ${prefix_list})
        add_library(matchmaker_${prefix} SHARED "generated_src/${prefix}.cpp")
        add_lib_finalization(${prefix} ${prefix})
    endforeach()
else()
    # deal out prefixes
    set(index "0")
    foreach(prefix ${prefix_list})
        list(APPEND leaflib_${index} ${prefix})
        math(EXPR index "${index} + 1")
        if(index EQUAL ${LEAFLIB_COUNT})
            set(index "0")
        endif()
    endforeach()

    # create the libs
    set(index "0")
    while(${index} LESS ${LEAFLIB_COUNT})
        set(cur_sources "")
        foreach(prefix ${leaflib_${index}})
            list(APPEND cur_sources "generated_src/${prefix}.cpp")
        endforeach()
        add_library(matchmaker_${index} SHARED ${cur_sources})
        add_lib_finalization(${index} ${leaflib_${index}})
        math(EXPR index "${index} + 1")
    endwhile()
endif()



# matchmaker shared library
add_library(matchmaker SHARED ${matchmaker_srcs})
target_include_directories(
    matchmaker
    PRIVATE
    ${MATCHMAKER_ROOT}/src
    ${MATCHMAKER_ROOT}/data_reader/stage_0/include
)
foreach(lib ${lib_list})
    target_link_libraries(matchmaker ${lib})
endforeach()
export_mm_only(matchmaker)
finish_link(matchmaker)

cmake_host_system_information(RESULT total_mib QUERY TOTAL_PHYSICAL_MEMORY)
mm_link_pool_depth(link_depth ${total_mib} ${largest_link_mib})
set_property(GLOBAL APPEND PROPERTY JOB_POOLS mm_link=${link_depth})
set_property(TARGET matchmaker ${lib_list} PROPERTY JOB_POOL_LINK mm_link)
list(LENGTH lib_list leaf_lib_count)
message(STATUS "links: ${link_depth} at a time, the largest needs about ${largest_link_mib} MiB "
               "(${total_mib} MiB of memory)")
mm_link_fits(link_fits ${total_mib} ${largest_link_mib})
if(NOT link_fits)
    message(WARNING "linking the largest leaf library needs about ${largest_link_mib} MiB, more than half "
                    "of this machine's ${total_mib} MiB")
endif()
if(NOT CMAKE_GENERATOR MATCHES "Ninja" AND link_depth LESS leaf_lib_count)
    message(WARNING "only Ninja limits how many links run at once (${link_depth} fit in memory here); "
                    "with ${CMAKE_GENERATOR}, -j decides")
endif()

install(
    TARGETS matchmaker
    DESTINATION ${lib_destination}
    EXPORT matchmaker
)
install(
    DIRECTORY ${MATCHMAKER_ROOT}/include/matchmaker/
    DESTINATION ${inc_destination}
    FILES_MATCHING PATTERN "*.h"
)




# with EXPORT_ALL, a header per leaf holding only its type, without words or values, for units of words to
# compile against: leaf_types/<path>.h mirrors generated_matchables/<path>.h
if(EXPORT_ALL)
    set(generated_matchables ${CMAKE_CURRENT_SOURCE_DIR}/generated_include/matchmaker/generated_matchables)
    set(leaf_types ${CMAKE_CURRENT_BINARY_DIR}/leaf_types)
    file(REMOVE_RECURSE ${leaf_types})
    file(GLOB_RECURSE leaf_headers RELATIVE ${generated_matchables} ${generated_matchables}/*.h)
    foreach(leaf_header ${leaf_headers})
        get_filename_component(prefix ${leaf_header} NAME_WE)
        file(READ ${generated_matchables}/${leaf_header} head LIMIT 65536)
        string(FIND "${head}" "\nPROPERTYx" begin)
        string(FIND "${head}" ", word_${prefix}" end)
        if(begin EQUAL -1 OR end EQUAL -1 OR end LESS begin)
            message(FATAL_ERROR "no declaration of word_${prefix} in ${leaf_header}")
        endif()
        math(EXPR begin "${begin} + 1")
        string(LENGTH ", word_${prefix}" name_length)
        math(EXPR length "${end} + ${name_length} - ${begin}")
        string(SUBSTRING "${head}" ${begin} ${length} declaration)
        file(WRITE ${leaf_types}/${leaf_header}
            "#pragma once\n#include <matchable/matchable.h>\n\n${declaration})\n")
    endforeach()
    install(DIRECTORY ${leaf_types}/ DESTINATION ${inc_destination}/leaf_types)
endif()




# play nice with others

install(EXPORT matchmaker FILE matchmaker.cmake DESTINATION ${lib_destination}/cmake)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/matchmaker-config.cmake"
    INSTALL_DESTINATION "${lib_destination}/cmake"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/matchmaker-config-version.cmake"
    VERSION "${matchmaker_VERSION_MAJOR}.${matchmaker_VERSION_MINOR}.${matchmaker_VERSION_PATCH}"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/matchmaker-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/matchmaker-config-version.cmake
    DESTINATION ${lib_destination}/cmake
)
