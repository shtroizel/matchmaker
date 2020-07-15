cmake_minimum_required(VERSION 3.13)

project(matchmaker VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=medany")
endif()

option(ATOMIC_LIBS "builds a library for every prefix" OFF)
option(q "build q (and Q) only" OFF)

if(q AND NOT ATOMIC_LIBS)
    message(FATAL_ERROR "ATOMIC_LIBS required when building q only")
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/matchmaker")

set(JOBS "1" CACHE STRING "parallel job count")

set(suffix "")
if(q)
    add_definitions(-DQ_ONLY)
    set(suffix "_q")
elseif(ATOMIC_LIBS)
    set(suffix "_atomic")
endif()

add_definitions(-DMATCHABLE_OMIT_BY_INDEX)

find_package(matchable 10.1.0 REQUIRED)
include_directories(${matchable_DIR}/../../../include)

set(lib_list)
macro(add_lib_finalization lib_name)
    list(APPEND lib_list matchmaker_${lib_name})
    target_include_directories(
        matchmaker_${lib_name}
        PRIVATE
        generated_include${suffix}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated_include${suffix}>
        $<INSTALL_INTERFACE:include>
    )
    set_target_properties(matchmaker_${lib_name} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    install(
        TARGETS matchmaker_${lib_name}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker
        EXPORT matchmaker
    )
endmacro()



set(prefix_list)

set(A_z_letters)
foreach(A_Z_number RANGE 65 90)
    string(ASCII ${A_Z_number} A_Z_letter)
    if(q)
        if(NOT A_Z_letter STREQUAL "Q")
            continue()
        endif()
    endif()
    list(APPEND A_z_letters ${A_Z_letter})
endforeach()

foreach(a_z_number RANGE 97 122)
    string(ASCII ${a_z_number} a_z_letter)
    if(q)
        if(NOT a_z_letter STREQUAL "q")
            continue()
        endif()
    endif()
    list(APPEND A_z_letters ${a_z_letter})
endforeach()

# matchmaker_srcs are parents (no leaves)
set(matchmaker_srcs)

foreach(first_letter ${A_z_letters})

    # add single letter parents to matchmaker_srcs
    list(APPEND matchmaker_srcs generated_src${suffix}/${first_letter}.cpp)

    foreach(second_a_z_number RANGE 97 122)
        string(ASCII ${second_a_z_number} second_letter)
        if(
            (first_letter STREQUAL "q" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "n" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "u" AND second_letter STREQUAL "n") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "i" AND second_letter STREQUAL "n") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "d" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "d" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "t") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "r" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "n") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "h") OR
            (first_letter STREQUAL "m" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "o" AND second_letter STREQUAL "v") OR
            (first_letter STREQUAL "m" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "m" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "h" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "h" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "h") OR
            (first_letter STREQUAL "m" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "h") OR
            (first_letter STREQUAL "l" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "e" AND second_letter STREQUAL "x") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "p") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "h" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "c") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "e" AND second_letter STREQUAL "n") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "h" AND second_letter STREQUAL "y") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "l") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "l") OR
            (first_letter STREQUAL "l" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "g" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "r" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "g" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "l") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "c") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "o" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "l") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "m" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "h") OR
            (first_letter STREQUAL "d" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "n" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "i" AND second_letter STREQUAL "m") OR
            (first_letter STREQUAL "l" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "w" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "m") OR
            (first_letter STREQUAL "v" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "v" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "l" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "r" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "b" AND second_letter STREQUAL "l") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "p") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "a" AND second_letter STREQUAL "s") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "s") OR
            (first_letter STREQUAL "g" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "f" AND second_letter STREQUAL "e")
        )
            foreach(third_a_z_number RANGE 97 122)
                string(ASCII ${third_a_z_number} third_letter)
                if(
                    (first_letter STREQUAL "p" AND second_letter STREQUAL "r" AND third_letter STREQUAL "e") OR
                    (first_letter STREQUAL "d" AND second_letter STREQUAL "i" AND third_letter STREQUAL "s") OR
                    (first_letter STREQUAL "c" AND second_letter STREQUAL "o" AND third_letter STREQUAL "n") OR
                    (first_letter STREQUAL "n" AND second_letter STREQUAL "o" AND third_letter STREQUAL "n") OR
                    (first_letter STREQUAL "u" AND second_letter STREQUAL "n" AND third_letter STREQUAL "d") OR
                    (first_letter STREQUAL "p" AND second_letter STREQUAL "r" AND third_letter STREQUAL "o") OR
                    (first_letter STREQUAL "u" AND second_letter STREQUAL "n" AND third_letter STREQUAL "s") OR
                    (first_letter STREQUAL "s" AND second_letter STREQUAL "u" AND third_letter STREQUAL "b") OR
                    (first_letter STREQUAL "i" AND second_letter STREQUAL "n" AND third_letter STREQUAL "t") OR
                    (first_letter STREQUAL "s" AND second_letter STREQUAL "u" AND third_letter STREQUAL "p") OR
                    (first_letter STREQUAL "u" AND second_letter STREQUAL "n" AND third_letter STREQUAL "c") OR
                    (first_letter STREQUAL "p" AND second_letter STREQUAL "a" AND third_letter STREQUAL "r") OR
                    (first_letter STREQUAL "t" AND second_letter STREQUAL "r" AND third_letter STREQUAL "a") OR
                    (first_letter STREQUAL "u" AND second_letter STREQUAL "n" AND third_letter STREQUAL "p") OR
                    (first_letter STREQUAL "t" AND second_letter STREQUAL "r" AND third_letter STREQUAL "i") OR
                    (first_letter STREQUAL "c" AND second_letter STREQUAL "o" AND third_letter STREQUAL "m") OR
                    (first_letter STREQUAL "c" AND second_letter STREQUAL "a" AND third_letter STREQUAL "r") OR
                    (first_letter STREQUAL "r" AND second_letter STREQUAL "e" AND third_letter STREQUAL "c")
                )
                    # three letter parents
                    list(
                        APPEND
                        matchmaker_srcs
                        generated_src${suffix}/${first_letter}_${second_letter}_${third_letter}.cpp
                    )

                    # four letter leaves
                    foreach(fourth_a_z_number RANGE 97 122)
                        string(ASCII ${fourth_a_z_number} fourth_letter)
                        list(
                            APPEND
                            prefix_list
                            "${first_letter}_${second_letter}_${third_letter}_${fourth_letter}"
                        )
                    endforeach()
                else()
                    # two letter parents
                    list(APPEND matchmaker_srcs generated_src${suffix}/${first_letter}_${second_letter}.cpp)

                    # three letter leaves
                    list(APPEND prefix_list "${first_letter}_${second_letter}_${third_letter}")
                endif()
            endforeach()
        else()
            list(APPEND prefix_list "${first_letter}_${second_letter}")
        endif()
    endforeach()
endforeach()


# parent of all
list(APPEND matchmaker_srcs src/matchmaker.cpp)


if(ATOMIC_LIBS)
    foreach(prefix ${prefix_list})
        add_library(matchmaker_${prefix} SHARED "generated_src${suffix}/${prefix}.cpp")
        add_lib_finalization(${prefix})
    endforeach()
else()
    foreach(num_0_107 RANGE 0 107)
        set(lib_group_${num_0_107})
    endforeach()

    set(lib_group_index "0")
    foreach(prefix ${prefix_list})
        list(APPEND lib_group_${lib_group_index} ${prefix})
        if(lib_group_index LESS 107)
            math(EXPR lib_group_index "${lib_group_index} + 1")
        else()
            set(lib_group_index "0")
        endif()
    endforeach()

    foreach(num_0_107 RANGE 0 107)
        set(cur_sources "")
        foreach(prefix ${lib_group_${num_0_107}})
            list(APPEND cur_sources "generated_src${suffix}/${prefix}.cpp")
        endforeach()
        add_library(matchmaker_${num_0_107} SHARED ${cur_sources})
        add_lib_finalization(${num_0_107})
    endforeach()
endif()



# matchmaker shared library
add_library(matchmaker SHARED ${matchmaker_srcs})
foreach(lib ${lib_list})
    target_link_libraries(matchmaker ${lib})
endforeach()
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    find_program(ld_gold ld.gold)
    if(NOT "${ld_gold}" STREQUAL "ld_gold-NOTFOUND")
        target_link_options(matchmaker PUBLIC -fuse-ld=gold)
    endif()
endif()
install(
    TARGETS matchmaker
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker
    EXPORT matchmaker
)
install(
    DIRECTORY include/matchmaker
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)


# play nice with others
install(EXPORT matchmaker FILE matchmaker.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker/cmake)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/matchmaker-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/matchmaker/cmake"
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
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker/cmake
)
