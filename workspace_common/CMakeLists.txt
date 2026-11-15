cmake_minimum_required(VERSION 3.16.3)

project(matchmaker VERSION 4.0.0)

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

set(LEAFLIB_COUNT 16)

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
find_package(matchable 14.0.0 REQUIRED)
include_directories(${matchable_DIR}/../../../include)

set(mm_exports_map ${MATCHMAKER_ROOT}/workspace_common/mm_exports.map)
macro(export_mm_only target)
    target_link_options(${target} PRIVATE -Wl,--version-script=${mm_exports_map})
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS ${mm_exports_map})
endmacro()

set(lib_list)
macro(add_lib_finalization lib_name)
    list(APPEND lib_list matchmaker_${lib_name})
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
        add_lib_finalization(${prefix})
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
        add_lib_finalization(${index})
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
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    find_program(ld_gold ld.gold)
    if(NOT "${ld_gold}" STREQUAL "ld_gold-NOTFOUND")
        target_link_options(matchmaker PUBLIC -fuse-ld=gold)
    endif()
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
