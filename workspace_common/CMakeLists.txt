cmake_minimum_required(VERSION 3.13)

project(matchmaker VERSION 1.4.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=medany")
endif()

option(ATOMIC_LIBS "builds a library for every prefix" OFF)
option(q "build q (and Q) only" OFF)


set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/matchmaker")

if(q)
    add_definitions(-DQ_ONLY)
endif()

if(NOT MATCHMAKER_ROOT)
    message(FATAL_ERROR "MATCHMAKER_ROOT must be set! please set to matchmaker repository root location")
endif()

add_definitions(-DMATCHABLE_OMIT_BY_INDEX)
find_package(matchable 10.2.0 REQUIRED)
include_directories(${matchable_DIR}/../../../include)

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
    install(
        TARGETS matchmaker_${lib_name}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker
        EXPORT matchmaker
    )
endmacro()



set(prefix_list)

set(A_z_letters)
set(l0_A_z_letters)
foreach(A_Z_number RANGE 65 90)
    string(ASCII ${A_Z_number} A_Z_letter)
    list(APPEND A_z_letters ${A_Z_letter})
    if(q)
        if(NOT A_Z_letter STREQUAL "Q")
            continue()
        endif()
    endif()
    list(APPEND l0_A_z_letters ${A_Z_letter})
endforeach()

foreach(a_z_number RANGE 97 122)
    string(ASCII ${a_z_number} a_z_letter)
    list(APPEND A_z_letters ${a_z_letter})
    if(q)
        if(NOT a_z_letter STREQUAL "q")
            continue()
        endif()
    endif()
    list(APPEND l0_A_z_letters ${a_z_letter})
endforeach()

# matchmaker_srcs are parents (no leaves)
set(matchmaker_srcs)

foreach(l0 ${l0_A_z_letters})

    # add single letter parents to matchmaker_srcs
    list(APPEND matchmaker_srcs generated_src/${l0}.cpp)

    foreach(l1 ${A_z_letters})
        if(
            (l0 STREQUAL "q" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "n" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "u" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "i" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "t") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "r" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "m" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "o" AND l1 STREQUAL "v") OR
            (l0 STREQUAL "m" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "m" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "h" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "h" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "m" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "l" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "e" AND l1 STREQUAL "x") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "p") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "h" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "c") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "e" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "h" AND l1 STREQUAL "y") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "l" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "r" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "c") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "o" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "m" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "n" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "i" AND l1 STREQUAL "m") OR
            (l0 STREQUAL "l" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "w" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "m") OR
            (l0 STREQUAL "v" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "v" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "l" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "r" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "b" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "p") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "s") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "p" AND l1 STREQUAL "s") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "M" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "d") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "w" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "y") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "o" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "v" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "e" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "r") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "b") OR
            (l0 STREQUAL "e" AND l1 STREQUAL "p") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "l") OR
            (l0 STREQUAL "c" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "e" AND l1 STREQUAL "m") OR
            (l0 STREQUAL "w" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "n" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "f" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "w" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "C" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "h" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "C" AND l1 STREQUAL "h") OR
            (l0 STREQUAL "C" AND l1 STREQUAL "o") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "w") OR
            (l0 STREQUAL "t" AND l1 STREQUAL "w") OR
            (l0 STREQUAL "r" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "g" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "S" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "B" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "a" AND l1 STREQUAL "t") OR
            (l0 STREQUAL "P" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "d" AND l1 STREQUAL "u") OR
            (l0 STREQUAL "L" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "n" AND l1 STREQUAL "i") OR
            (l0 STREQUAL "A" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "s" AND l1 STREQUAL "n") OR
            (l0 STREQUAL "B" AND l1 STREQUAL "e") OR
            (l0 STREQUAL "H" AND l1 STREQUAL "a") OR
            (l0 STREQUAL "r" AND l1 STREQUAL "i")
        )
            # two letter parents
            list(APPEND matchmaker_srcs generated_src/${l0}_${l1}.cpp)

            foreach(l2 ${A_z_letters})
                if(
                    (l0 STREQUAL "p" AND l1 STREQUAL "r" AND l2 STREQUAL "e") OR
                    (l0 STREQUAL "q" AND l1 STREQUAL "u" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "q" AND l1 STREQUAL "u" AND l2 STREQUAL "i") OR
                    (l0 STREQUAL "d" AND l1 STREQUAL "i" AND l2 STREQUAL "s") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "o" AND l2 STREQUAL "n") OR
                    (l0 STREQUAL "n" AND l1 STREQUAL "o" AND l2 STREQUAL "n") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "d") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "r" AND l2 STREQUAL "o") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "s") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "u" AND l2 STREQUAL "b") OR
                    (l0 STREQUAL "i" AND l1 STREQUAL "n" AND l2 STREQUAL "t") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "u" AND l2 STREQUAL "p") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "c") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "a" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "t" AND l1 STREQUAL "r" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "p") OR
                    (l0 STREQUAL "t" AND l1 STREQUAL "r" AND l2 STREQUAL "i") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "o" AND l2 STREQUAL "m") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "a" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "r" AND l1 STREQUAL "e" AND l2 STREQUAL "c") OR
                    (l0 STREQUAL "a" AND l1 STREQUAL "n" AND l2 STREQUAL "t") OR
                    (l0 STREQUAL "o" AND l1 STREQUAL "u" AND l2 STREQUAL "t") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "e" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "m" AND l1 STREQUAL "i" AND l2 STREQUAL "s") OR
                    (l0 STREQUAL "h" AND l1 STREQUAL "y" AND l2 STREQUAL "p") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "e" AND l2 STREQUAL "m") OR
                    (l0 STREQUAL "f" AND l1 STREQUAL "o" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "h" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "o" AND l2 STREQUAL "l") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "o" AND l1 STREQUAL "v" AND l2 STREQUAL "e") OR
                    (l0 STREQUAL "r" AND l1 STREQUAL "e" AND l2 STREQUAL "s") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "i") OR
                    (l0 STREQUAL "m" AND l1 STREQUAL "o" AND l2 STREQUAL "n") OR
                    (l0 STREQUAL "i" AND l1 STREQUAL "m" AND l2 STREQUAL "p") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "t" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "o" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "t" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "i" AND l1 STREQUAL "n" AND l2 STREQUAL "c") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "e") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "l" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "m" AND l1 STREQUAL "e" AND l2 STREQUAL "t") OR
                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "f") OR
                    (l0 STREQUAL "d" AND l1 STREQUAL "e" AND l2 STREQUAL "c") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "e" AND l2 STREQUAL "l") OR
                    (l0 STREQUAL "g" AND l1 STREQUAL "r" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "t" AND l2 STREQUAL "e") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "o" AND l2 STREQUAL "l") OR
                    (l0 STREQUAL "p" AND l1 STREQUAL "s" AND l2 STREQUAL "e") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "t" AND l2 STREQUAL "o") OR
                    (l0 STREQUAL "m" AND l1 STREQUAL "a" AND l2 STREQUAL "n") OR
                    (l0 STREQUAL "s" AND l1 STREQUAL "h" AND l2 STREQUAL "o") OR
                    (l0 STREQUAL "h" AND l1 STREQUAL "e" AND l2 STREQUAL "a") OR
                    (l0 STREQUAL "b" AND l1 STREQUAL "a" AND l2 STREQUAL "r") OR
                    (l0 STREQUAL "h" AND l1 STREQUAL "a" AND l2 STREQUAL "l") OR
                    (l0 STREQUAL "a" AND l1 STREQUAL "l" AND l2 STREQUAL "l") OR
                    (l0 STREQUAL "r" AND l1 STREQUAL "e" AND l2 STREQUAL "d") OR
                    (l0 STREQUAL "c" AND l1 STREQUAL "o" AND l2 STREQUAL "u")
                )
                    # three letter parents
                    list(APPEND matchmaker_srcs generated_src/${l0}_${l1}_${l2}.cpp)

                    foreach(l3 ${A_z_letters})
                        if(
                            (l0 STREQUAL "o" AND l1 STREQUAL "v" AND l2 STREQUAL "e" AND l3 STREQUAL "r") OR
                            (l0 STREQUAL "i" AND l1 STREQUAL "n" AND l2 STREQUAL "t" AND l3 STREQUAL "e") OR
                            (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "d" AND l3 STREQUAL "e") OR
                            (l0 STREQUAL "s" AND l1 STREQUAL "u" AND l2 STREQUAL "p" AND l3 STREQUAL "e") OR
                            (l0 STREQUAL "a" AND l1 STREQUAL "n" AND l2 STREQUAL "t" AND l3 STREQUAL "i") OR
                            (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "c" AND l3 STREQUAL "o") OR
                            (l0 STREQUAL "s" AND l1 STREQUAL "e" AND l2 STREQUAL "m" AND l3 STREQUAL "i") OR
                            (l0 STREQUAL "q" AND l1 STREQUAL "u" AND l2 STREQUAL "a" AND l3 STREQUAL "s") OR
                            (l0 STREQUAL "n" AND l1 STREQUAL "o" AND l2 STREQUAL "n" AND l3 STREQUAL "c")
                        )
                            # four letter parents
                            list(APPEND matchmaker_srcs generated_src/${l0}_${l1}_${l2}_${l3}.cpp)

                            foreach(l4 ${A_z_letters})
                                if(
                                    (l0 STREQUAL "i" AND l1 STREQUAL "n" AND l2 STREQUAL "t"
                                            AND l3 STREQUAL "e" AND l4 STREQUAL "r") OR
                                    (l0 STREQUAL "s" AND l1 STREQUAL "u" AND l2 STREQUAL "p"
                                            AND l3 STREQUAL "e" AND l4 STREQUAL "r") OR
                                    (l0 STREQUAL "u" AND l1 STREQUAL "n" AND l2 STREQUAL "d"
                                            AND l3 STREQUAL "e" AND l4 STREQUAL "r") OR
                                    (l0 STREQUAL "q" AND l1 STREQUAL "u" AND l2 STREQUAL "a"
                                            AND l3 STREQUAL "s" AND l4 STREQUAL "i")
                                )
                                    # five letter parents
                                    list(
                                        APPEND
                                        matchmaker_srcs
                                        generated_src/${l0}_${l1}_${l2}_${l3}_${l4}.cpp
                                    )

                                    foreach(l5 ${A_z_letters})
                                        # six letter leaves
                                        list(APPEND prefix_list "${l0}_${l1}_${l2}_${l3}_${l4}_${l5}")
                                    endforeach()
                                else()
                                    # five letter leaves
                                    list(APPEND prefix_list "${l0}_${l1}_${l2}_${l3}_${l4}")
                                endif()
                            endforeach()
                        else()
                            # four letter leaves
                            list(APPEND prefix_list "${l0}_${l1}_${l2}_${l3}")
                        endif()
                    endforeach()
                else()
                    # three letter leaves
                    list(APPEND prefix_list "${l0}_${l1}_${l2}")
                endif()
            endforeach()
        else()
            # two letter leaves
            list(APPEND prefix_list "${l0}_${l1}")
        endif()
    endforeach()
endforeach()


# parent of all
list(APPEND matchmaker_srcs ${MATCHMAKER_ROOT}/src/matchmaker.cpp)

if(ATOMIC_LIBS)
    foreach(prefix ${prefix_list})
        add_library(matchmaker_${prefix} SHARED "generated_src/${prefix}.cpp")
        add_lib_finalization(${prefix})
    endforeach()
else()
    foreach(num_0_6 RANGE 0 6)
        set(lib_group_${num_0_6})
    endforeach()

    set(lib_group_index "0")
    foreach(prefix ${prefix_list})
        list(APPEND lib_group_${lib_group_index} ${prefix})
        if(lib_group_index LESS 6)
            math(EXPR lib_group_index "${lib_group_index} + 1")
        else()
            set(lib_group_index "0")
        endif()
    endforeach()

    foreach(num_0_6 RANGE 0 6)
        set(cur_sources "")
        foreach(prefix ${lib_group_${num_0_6}})
            list(APPEND cur_sources "generated_src/${prefix}.cpp")
        endforeach()
        add_library(matchmaker_${num_0_6} SHARED ${cur_sources})
        add_lib_finalization(${num_0_6})
    endforeach()
endif()



# matchmaker shared library
add_library(matchmaker SHARED ${matchmaker_srcs})
target_include_directories(
    matchmaker
    PRIVATE
    ${MATCHMAKER_ROOT}/data_reader/stage_0/include
)
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
    DIRECTORY ${MATCHMAKER_ROOT}/include/matchmaker
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
