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

set(suffix_q "")
if(q)
    add_definitions(-DQ_ONLY)
    set(suffix_q "_q")
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
        generated_include${suffix_q}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated_include${suffix_q}>
        $<INSTALL_INTERFACE:include>
    )
    set_target_properties(matchmaker_${lib_name} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    install(
        TARGETS matchmaker_${lib_name}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/matchmaker
        EXPORT matchmaker
    )
endmacro()


macro(add_single_prefix_lib_without_deps prefix)
    add_library(matchmaker_${prefix} SHARED generated_src${suffix_q}/${prefix}.cpp)
    add_lib_finalization(${prefix})
endmacro()


macro(add_single_prefix_libs_without_deps prefixes)
    foreach(prefix ${prefixes})
        add_single_prefix_lib_without_deps(${prefix})
    endforeach()
endmacro()


macro(add_single_prefix_libs_dep_on_prev prefixes)
    if(prev_lib_group)
        list(LENGTH prev_lib_group prev_lib_group_length)
        math(EXPR prev_lib_group_last "${prev_lib_group_length} - 1")
        set(prev_lib_group_index "0")
    endif()
    foreach(prefix ${prefixes})
        if(NOT ATOMIC_LIBS)
            add_single_prefix_lib_without_deps(${prefix})
        endif()
        if(prev_lib_group)
            list(GET prev_lib_group ${prev_lib_group_index} prev_lib)
            add_dependencies(matchmaker_${prefix} matchmaker_${prev_lib})

            if(prev_lib_group_index LESS ${prev_lib_group_last})
                math(EXPR prev_lib_group_index "${prev_lib_group_index} + 1")
            else()
                set(prev_lib_group_index "0")
            endif()
       	endif()
    endforeach()
    set(prev_lib_group "${prefixes}")
endmacro()


macro(add_multi_lib multi_index srcs)
    add_library(matchmaker_${multi_index} SHARED ${srcs})
    add_lib_finalization(${multi_index})
endmacro()




if(ATOMIC_LIBS)

    # first handle capital single letter prefixes
    foreach(A_Z_number RANGE 65 90)
        string(ASCII ${A_Z_number} A_Z_letter)

        if(q)
            if(NOT A_Z_letter STREQUAL "Q")
                continue()
            endif()
        endif()

        add_single_prefix_lib_without_deps(${A_Z_letter})
    endforeach()


    if(NOT q)
        add_single_prefix_lib_without_deps(j)
        add_single_prefix_lib_without_deps(k)
        add_single_prefix_lib_without_deps(q)
        add_single_prefix_lib_without_deps(x)
        add_single_prefix_lib_without_deps(y)
        add_single_prefix_lib_without_deps(z)
    endif()

    foreach(first_a_z_number RANGE 97 122)
        string(ASCII ${first_a_z_number} first_letter)

        if(
            first_letter STREQUAL "j" OR
            first_letter STREQUAL "k" OR
            first_letter STREQUAL "x" OR
            first_letter STREQUAL "y" OR
            first_letter STREQUAL "z"
        )
            continue()
        endif()

        if(q)
            # if only building q then skip all others
            # note that when only building q we split into single, double, and triple letter prefixes
            # so that all templates are exercised - this is normally not done for q when building everything
            if(NOT first_letter STREQUAL "q")
                continue()
            endif()
        else()
            # when building everything, q is only a single letter prefix (no splitting), and is already
            # handled a few lines above with 'add_single_prefix_lib_without_deps(q)'
            if(first_letter STREQUAL "q")
                continue()
            endif()
        endif()

        foreach(second_a_z_number RANGE 97 122)
            string(ASCII ${second_a_z_number} second_letter)
            if(
                (q AND first_letter STREQUAL "q" AND second_letter STREQUAL "u") OR
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
                (first_letter STREQUAL "r" AND second_letter STREQUAL "e")
            )
                foreach(third_a_z_number RANGE 97 122)
                    string(ASCII ${third_a_z_number} third_letter)
                    add_single_prefix_lib_without_deps(${first_letter}_${second_letter}_${third_letter})
                endforeach()
            else()
                add_single_prefix_lib_without_deps(${first_letter}_${second_letter})
            endif()

        endforeach()
    endforeach()
endif()


if(NOT q)
    # "near groups" group together libs that require about the same amount of memory (in GB)

    set(near_15)
    list(APPEND near_15 p_r_e)
    list(APPEND near_15 a_n)
    list(APPEND near_15 c_h)
    list(APPEND near_15 m_a)
    list(APPEND near_15 s_e)
    list(APPEND near_15 o_v)
    list(APPEND near_15 m_i)
    list(APPEND near_15 p_e)
    list(APPEND near_15 p_o)
    list(APPEND near_15 m_e)
    list(APPEND near_15 b_a)

    set(near_13)
    list(APPEND near_13 b_e)
    list(APPEND near_13 A)
    list(APPEND near_13 S)
    list(APPEND near_13 d_i_s)
    list(APPEND near_13 B)
    list(APPEND near_13 C)
    list(APPEND near_13 c_o_n)
    list(APPEND near_13 M)

    set(near_10)
    list(APPEND near_10 k)
    list(APPEND near_10 h_e)
    list(APPEND near_10 h_a)
    list(APPEND near_10 p_h)
    list(APPEND near_10 m_o)
    list(APPEND near_10 u_n_d)
    list(APPEND near_10 p_r_o)
    list(APPEND near_10 t_h)
    list(APPEND near_10 u_n_s)
    list(APPEND near_10 l_a)
    list(APPEND near_10 e_x)
    list(APPEND near_10 j)
    list(APPEND near_10 s_p)
    list(APPEND near_10 c_r)
    list(APPEND near_10 h_o)
    list(APPEND near_10 f_o)
    list(APPEND near_10 s_c)
    list(APPEND near_10 s_u_b)
    list(APPEND near_10 b_r)
    list(APPEND near_10 e_n)
    list(APPEND near_10 t_e)
    list(APPEND near_10 t_a)
    list(APPEND near_10 i_n_t)
    list(APPEND near_10 s_a)
    list(APPEND near_10 h_y)
    list(APPEND near_10 L)
    list(APPEND near_10 H)
    list(APPEND near_10 s_u_p)
    list(APPEND near_10 D)

    set(near_7)
    list(APPEND near_7 c_l)
    list(APPEND near_7 a_r)
    list(APPEND near_7 a_l)
    list(APPEND near_7 P)
    list(APPEND near_7 l_i)
    list(APPEND near_7 g_a)
    list(APPEND near_7 s_o)
    list(APPEND near_7 u_n_c)
    list(APPEND near_7 r_a)
    list(APPEND near_7 g_r)
    list(APPEND near_7 b_i)
    list(APPEND near_7 p_l)
    list(APPEND near_7 a_c)
    list(APPEND near_7 t_o)
    list(APPEND near_7 G)
    list(APPEND near_7 p_i)
    list(APPEND near_7 o_u)
    list(APPEND near_7 f_l)
    list(APPEND near_7 s_i)
    list(APPEND near_7 p_a_r)
    list(APPEND near_7 T)
    list(APPEND near_7 b_u)
    list(APPEND near_7 m_u)
    list(APPEND near_7 s_h)
    list(APPEND near_7 d_o)
    list(APPEND near_7 b_o)
    list(APPEND near_7 n_e)
    list(APPEND near_7 i_m)
    list(APPEND near_7 q)
    list(APPEND near_7 K)

    set(near_5)
    list(APPEND near_5 l_e)
    list(APPEND near_5 R)
    list(APPEND near_5 t_r_a)
    list(APPEND near_5 f_a)
    list(APPEND near_5 f_i)
    list(APPEND near_5 u_n_p)
    list(APPEND near_5 w_a)
    list(APPEND near_5 t_r_i)
    list(APPEND near_5 a_m)
    list(APPEND near_5 v_i)
    list(APPEND near_5 v_e)
    list(APPEND near_5 E)
    list(APPEND near_5 l_o)
    list(APPEND near_5 f_r)
    list(APPEND near_5 c_o_m)
    list(APPEND near_5 r_o)
    list(APPEND near_5 b_l)
    list(APPEND near_5 p_u)
    list(APPEND near_5 a_p)
    list(APPEND near_5 c_u)
    list(APPEND near_5 c_a_r)
    list(APPEND near_5 a_s)
    list(APPEND near_5 r_e_c)
    list(APPEND near_5 c_e)
    list(APPEND near_5 p_s)
    list(APPEND near_5 g_e)
    list(APPEND near_5 f_e)

    set(near_4)
    list(APPEND near_4 F)
    list(APPEND near_4 a_u)
    list(APPEND near_4 d_a)
    list(APPEND near_4 a_d)
    list(APPEND near_4 r_e_s)
    list(APPEND near_4 u_n_a)
    list(APPEND near_4 w_i)
    list(APPEND near_4 u_n_r)
    list(APPEND near_4 s_y)
    list(APPEND near_4 g_l)
    list(APPEND near_4 u_n_i)
    list(APPEND near_4 o_r)
    list(APPEND near_4 v_a)
    list(APPEND near_4 N)
    list(APPEND near_4 g_o)
    list(APPEND near_4 t_i)
    list(APPEND near_4 z)
    list(APPEND near_4 t_u)
    list(APPEND near_4 e_l)
    list(APPEND near_4 d_r)
    list(APPEND near_4 a_b)
    list(APPEND near_4 s_t_r)
    list(APPEND near_4 e_p)
    list(APPEND near_4 c_o_r)
    list(APPEND near_4 s_l)
    list(APPEND near_4 s_t_a)
    list(APPEND near_4 J)
    list(APPEND near_4 W)
    list(APPEND near_4 c_i)

    set(near_3)
    list(APPEND near_3 w_h)
    list(APPEND near_3 i_n_c)
    list(APPEND near_3 e_m)
    list(APPEND near_3 n_a)
    list(APPEND near_3 f_u)
    list(APPEND near_3 u_n_e)
    list(APPEND near_3 y)
    list(APPEND near_3 u_n_f)
    list(APPEND near_3 d_e_c)
    list(APPEND near_3 c_o_u)
    list(APPEND near_3 g_u)
    list(APPEND near_3 r_i)
    list(APPEND near_3 s_t_e)
    list(APPEND near_3 w_o)
    list(APPEND near_3 r_e_p)
    list(APPEND near_3 u_n_t)
    list(APPEND near_3 h_i)
    list(APPEND near_3 p_a_l)
    list(APPEND near_3 c_o_l)
    list(APPEND near_3 d_u)
    list(APPEND near_3 c_y)
    list(APPEND near_3 u_n_m)
    list(APPEND near_3 u_n_b)
    list(APPEND near_3 i_n_s)
    list(APPEND near_3 r_e_a)
    list(APPEND near_3 O)
    list(APPEND near_3 w_e)
    list(APPEND near_3 i_s)
    list(APPEND near_3 i_n_d)
    list(APPEND near_3 r_u)
    list(APPEND near_3 I)
    list(APPEND near_3 n_i)
    list(APPEND near_3 m_y)
    list(APPEND near_3 s_w)
    list(APPEND near_3 h_u)
    list(APPEND near_3 c_a_l)
    list(APPEND near_3 u_p)
    list(APPEND near_3 a_t)
    list(APPEND near_3 r_e_t)

    if(MEM_LIMIT)
        message(STATUS "******* memory usage limit: ${MEM_LIMIT} GB *******")

        macro(add_near_group near_suffix)
            # memory usage model based on g++ Release and Debug builds
            #
            # for clang and clang + debug we reuse the g++ model by scaling the "memory limit"
            # used in the calculations for how many of which libs can run together
            set(adjusted_MEM_LIMIT ${MEM_LIMIT})
            if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                    math(EXPR adjusted_MEM_LIMIT "${MEM_LIMIT} * 33")
                    math(EXPR adjusted_MEM_LIMIT "${adjusted_MEM_LIMIT} / 17")
                else()
                    math(EXPR adjusted_MEM_LIMIT "${MEM_LIMIT} * 107")
                    math(EXPR adjusted_MEM_LIMIT "${adjusted_MEM_LIMIT} / 76")
                endif()
            endif()

            # how many libs can run in parallel (group_size)?
            math(EXPR group_size "${adjusted_MEM_LIMIT} / ${near_suffix}")

            list(LENGTH near_${near_suffix} near_length)
            set(index "0")
            while(index LESS ${near_length})
                set(group "")
                if(${group_size} GREATER_EQUAL "2")
                    set(broke "0")
                    foreach(dummy RANGE 2 ${group_size})
                        list(GET near_${near_suffix} ${index} cur)
                        list(APPEND group ${cur})
                        math(EXPR index "${index} + 1")
                        if(index GREATER_EQUAL ${near_length})
                            add_single_prefix_libs_dep_on_prev("${group}")
                            set(broke "1")
                            break()
                        endif()
                    endforeach()
                    if("${broke}" STREQUAL "1")
                        break()
                    endif()
                endif()

                # separated out from above to guarantee at least 1 regardless of group size
                list(GET near_${near_suffix} ${index} cur)
                list(APPEND group ${cur})
                add_single_prefix_libs_dep_on_prev("${group}")
                math(EXPR index "${index} + 1")
            endwhile()
        endmacro()

        add_near_group(15)
        add_near_group(13)
        add_near_group(10)
        add_near_group(7)
        add_near_group(5)
        add_near_group(4)
        add_near_group(3)
    else()
        message(STATUS "******* memory usage limit: OFF *******")
        add_single_prefix_libs_without_deps("${near_15}")
        add_single_prefix_libs_without_deps("${near_13}")
        add_single_prefix_libs_without_deps("${near_10}")
        add_single_prefix_libs_without_deps("${near_7}")
        add_single_prefix_libs_without_deps("${near_5}")
        add_single_prefix_libs_without_deps("${near_4}")
        add_single_prefix_libs_without_deps("${near_3}")
    endif()

endif()


if(NOT ATOMIC_LIBS)
    set(
        multi_0_srcs
        generated_src${suffix_q}/s_n.cpp
        generated_src${suffix_q}/l_u.cpp
        generated_src${suffix_q}/c_a_n.cpp
        generated_src${suffix_q}/V.cpp
        generated_src${suffix_q}/p_a_n.cpp
        generated_src${suffix_q}/a_g.cpp
    )
    set(
        multi_1_srcs
        generated_src${suffix_q}/c_a_t.cpp
        generated_src${suffix_q}/d_e_s.cpp
        generated_src${suffix_q}/r_e_d.cpp
        generated_src${suffix_q}/s_t_o.cpp
        generated_src${suffix_q}/d_i_a.cpp
        generated_src${suffix_q}/s_k.cpp
        generated_src${suffix_q}/i_n_f.cpp
        generated_src${suffix_q}/o_p.cpp
    )
    set(
        multi_2_srcs
        generated_src${suffix_q}/o_b.cpp
        generated_src${suffix_q}/g_i.cpp
        generated_src${suffix_q}/e_s.cpp
        generated_src${suffix_q}/d_e_m.cpp
        generated_src${suffix_q}/p_y.cpp
        generated_src${suffix_q}/u_r.cpp
        generated_src${suffix_q}/i_r.cpp
        generated_src${suffix_q}/v_o.cpp
        generated_src${suffix_q}/v_p.cpp
    )
    set(
        multi_3_srcs
        generated_src${suffix_q}/r_e_f.cpp
        generated_src${suffix_q}/r_h.cpp
        generated_src${suffix_q}/u_n_h.cpp
        generated_src${suffix_q}/n_u.cpp
        generated_src${suffix_q}/u_n_g.cpp
        generated_src${suffix_q}/p_r_i.cpp
        generated_src${suffix_q}/t_r_o.cpp
        generated_src${suffix_q}/e_r.cpp
        generated_src${suffix_q}/e_u.cpp
    )
    set(
        multi_4_srcs
        generated_src${suffix_q}/a_f.cpp
        generated_src${suffix_q}/s_u_l.cpp
        generated_src${suffix_q}/u_n_l.cpp
        generated_src${suffix_q}/r_e_i.cpp
        generated_src${suffix_q}/o_s.cpp
        generated_src${suffix_q}/e_c.cpp
        generated_src${suffix_q}/o_c.cpp
        generated_src${suffix_q}/r_e_v.cpp
        generated_src${suffix_q}/p_a_t.cpp
        generated_src${suffix_q}/s_q.cpp
        generated_src${suffix_q}/d_e_p.cpp
    )
    set(
        multi_5_srcs
        generated_src${suffix_q}/s_m.cpp
        generated_src${suffix_q}/d_e_f.cpp
        generated_src${suffix_q}/u_n_w.cpp
        generated_src${suffix_q}/s_t_i.cpp
        generated_src${suffix_q}/a_e.cpp
        generated_src${suffix_q}/c_a_p.cpp
        generated_src${suffix_q}/r_e_m.cpp
        generated_src${suffix_q}/e_t.cpp
        generated_src${suffix_q}/r_e_g.cpp
        generated_src${suffix_q}/d_e_n.cpp
        generated_src${suffix_q}/x.cpp
        generated_src${suffix_q}/e_v.cpp
        generated_src${suffix_q}/d_e_l.cpp
    )
    set(
        multi_6_srcs
        generated_src${suffix_q}/t_r_e.cpp
        generated_src${suffix_q}/r_e_l.cpp
        generated_src${suffix_q}/r_e_e.cpp
        generated_src${suffix_q}/c_a_s.cpp
        generated_src${suffix_q}/u_l.cpp
        generated_src${suffix_q}/i_d.cpp
        generated_src${suffix_q}/i_n_v.cpp
        generated_src${suffix_q}/u_n_v.cpp
        generated_src${suffix_q}/s_u_r.cpp
        generated_src${suffix_q}/c_a_m.cpp
        generated_src${suffix_q}/c_o_a.cpp
        generated_src${suffix_q}/d_e_r.cpp
        generated_src${suffix_q}/c_o_c.cpp
        generated_src${suffix_q}/t_w.cpp
    )
    set(
        multi_7_srcs
        generated_src${suffix_q}/o_x.cpp
        generated_src${suffix_q}/t_y.cpp
        generated_src${suffix_q}/Z.cpp
        generated_src${suffix_q}/u_n_o.cpp
        generated_src${suffix_q}/p_a_s.cpp
        generated_src${suffix_q}/w_r.cpp
        generated_src${suffix_q}/g_y.cpp
        generated_src${suffix_q}/i_n_e.cpp
        generated_src${suffix_q}/i_l.cpp
        generated_src${suffix_q}/c_o_p.cpp
        generated_src${suffix_q}/d_i_p.cpp
        generated_src${suffix_q}/U.cpp
        generated_src${suffix_q}/c_o_s.cpp
        generated_src${suffix_q}/a_v.cpp
        generated_src${suffix_q}/p_r_a.cpp
        generated_src${suffix_q}/d_y.cpp
        generated_src${suffix_q}/r_e_b.cpp
        generated_src${suffix_q}/a_i.cpp
        generated_src${suffix_q}/l_y.cpp
        generated_src${suffix_q}/o_l.cpp
        generated_src${suffix_q}/t_r_u.cpp
    )
    set(
        multi_8_srcs
        generated_src${suffix_q}/c_o_e.cpp
        generated_src${suffix_q}/e_a.cpp
        generated_src${suffix_q}/e_q.cpp
        generated_src${suffix_q}/o_n.cpp
        generated_src${suffix_q}/s_t_u.cpp
        generated_src${suffix_q}/d_e_v.cpp
        generated_src${suffix_q}/i_c.cpp
        generated_src${suffix_q}/d_e_t.cpp
        generated_src${suffix_q}/r_e_n.cpp
        generated_src${suffix_q}/Y.cpp
        generated_src${suffix_q}/d_e_a.cpp
        generated_src${suffix_q}/o_m.cpp
        generated_src${suffix_q}/i_n_a.cpp
        generated_src${suffix_q}/u_n_n.cpp
        generated_src${suffix_q}/d_i_v.cpp
        generated_src${suffix_q}/d_i_c.cpp
        generated_src${suffix_q}/p_a_c.cpp
        generated_src${suffix_q}/p_a_p.cpp
        generated_src${suffix_q}/c_a_u.cpp
        generated_src${suffix_q}/e_d.cpp
        generated_src${suffix_q}/e_f.cpp
        generated_src${suffix_q}/c_a_c.cpp
        generated_src${suffix_q}/s_u_c.cpp
        generated_src${suffix_q}/s_u_n.cpp
        generated_src${suffix_q}/d_e_b.cpp
        generated_src${suffix_q}/c_o_t.cpp
        generated_src${suffix_q}/c_o_o.cpp
        generated_src${suffix_q}/n_o_t.cpp
    )
    set(
        multi_9_srcs
        generated_src${suffix_q}/c_a_b.cpp
        generated_src${suffix_q}/o_d.cpp
        generated_src${suffix_q}/n_o_n.cpp
        generated_src${suffix_q}/o_t.cpp
        generated_src${suffix_q}/a_z.cpp
        generated_src${suffix_q}/o_o.cpp
        generated_src${suffix_q}/i_n_g.cpp
        generated_src${suffix_q}/d_i_g.cpp
        generated_src${suffix_q}/o_f.cpp
        generated_src${suffix_q}/p_t.cpp
        generated_src${suffix_q}/p_n.cpp
        generated_src${suffix_q}/n_o_r.cpp
        generated_src${suffix_q}/d_i_m.cpp
        generated_src${suffix_q}/u_m.cpp
        generated_src${suffix_q}/e_g.cpp
        generated_src${suffix_q}/u_t.cpp
        generated_src${suffix_q}/d_i_l.cpp
        generated_src${suffix_q}/s_t_y.cpp
        generated_src${suffix_q}/s_u_m.cpp
        generated_src${suffix_q}/a_w.cpp
        generated_src${suffix_q}/i_o.cpp
        generated_src${suffix_q}/r_e_h.cpp
        generated_src${suffix_q}/e_y.cpp
        generated_src${suffix_q}/u_s.cpp
        generated_src${suffix_q}/i_n_o.cpp
        generated_src${suffix_q}/g_n.cpp
        generated_src${suffix_q}/c_o_w.cpp
        generated_src${suffix_q}/c_o_g.cpp
        generated_src${suffix_q}/s_u_s.cpp
        generated_src${suffix_q}/c_o_i.cpp
        generated_src${suffix_q}/i_n_h.cpp
        generated_src${suffix_q}/d_e_g.cpp
        generated_src${suffix_q}/c_a_v.cpp
        generated_src${suffix_q}/c_a_d.cpp
        generated_src${suffix_q}/a_x.cpp
    )
    set(
        multi_10_srcs
        generated_src${suffix_q}/r_e_o.cpp
        generated_src${suffix_q}/d_i_n.cpp
        generated_src${suffix_q}/c_o_d.cpp
        generated_src${suffix_q}/r_e_w.cpp
        generated_src${suffix_q}/p_a_i.cpp
        generated_src${suffix_q}/g_h.cpp
        generated_src${suffix_q}/b_y.cpp
        generated_src${suffix_q}/v_u.cpp
        generated_src${suffix_q}/d_i_o.cpp
        generated_src${suffix_q}/d_i_f.cpp
        generated_src${suffix_q}/i_t.cpp
        generated_src${suffix_q}/a_q.cpp
        generated_src${suffix_q}/Q.cpp
        generated_src${suffix_q}/n_o_s.cpp
        generated_src${suffix_q}/n_o_v.cpp
        generated_src${suffix_q}/d_i_r.cpp
        generated_src${suffix_q}/n_o_m.cpp
        generated_src${suffix_q}/s_u_f.cpp
        generated_src${suffix_q}/i_n_n.cpp
        generated_src${suffix_q}/d_i_e.cpp
        generated_src${suffix_q}/d_e_e.cpp
        generated_src${suffix_q}/u_n_j.cpp
        generated_src${suffix_q}/p_a_u.cpp
        generated_src${suffix_q}/u_n_k.cpp
        generated_src${suffix_q}/c_o_v.cpp
        generated_src${suffix_q}/u_n_q.cpp
        generated_src${suffix_q}/Q.cpp
        generated_src${suffix_q}/o_i.cpp
        generated_src${suffix_q}/o_e.cpp
        generated_src${suffix_q}/u_n_u.cpp
        generated_src${suffix_q}/d_i_t.cpp
        generated_src${suffix_q}/i_g.cpp
        generated_src${suffix_q}/e_i.cpp
        generated_src${suffix_q}/d_e_h.cpp
        generated_src${suffix_q}/d_i_d.cpp
        generated_src${suffix_q}/c_o_b.cpp
        generated_src${suffix_q}/p_r_u.cpp
    )
    set(
        multi_11_srcs
        generated_src${suffix_q}/d_e_u.cpp
        generated_src${suffix_q}/o_a.cpp
        generated_src${suffix_q}/n_y.cpp
        generated_src${suffix_q}/i_n_i.cpp
        generated_src${suffix_q}/p_a_g.cpp
        generated_src${suffix_q}/r_e_j.cpp
        generated_src${suffix_q}/d_e_o.cpp
        generated_src${suffix_q}/d_e_i.cpp
        generated_src${suffix_q}/s_u_g.cpp
        generated_src${suffix_q}/a_k.cpp
        generated_src${suffix_q}/r_e_r.cpp
        generated_src${suffix_q}/d_e_d.cpp
        generated_src${suffix_q}/c_o_f.cpp
        generated_src${suffix_q}/n_o_c.cpp
        generated_src${suffix_q}/d_e_x.cpp
        generated_src${suffix_q}/d_h.cpp
        generated_src${suffix_q}/e_b.cpp
        generated_src${suffix_q}/r_e_q.cpp
        generated_src${suffix_q}/r_e_u.cpp
        generated_src${suffix_q}/t_r_y.cpp
        generated_src${suffix_q}/t_s.cpp
        generated_src${suffix_q}/d_e_w.cpp
        generated_src${suffix_q}/p_a_e.cpp
        generated_src${suffix_q}/i_n_k.cpp
        generated_src${suffix_q}/i_n_q.cpp
        generated_src${suffix_q}/p_a_m.cpp
        generated_src${suffix_q}/p_a_v.cpp
        generated_src${suffix_q}/o_w.cpp
        generated_src${suffix_q}/w_u.cpp
        generated_src${suffix_q}/s_u_d.cpp
        generated_src${suffix_q}/i_n_u.cpp
        generated_src${suffix_q}/n_o_u.cpp
        generated_src${suffix_q}/s_u_i.cpp
        generated_src${suffix_q}/n_o_b.cpp
        generated_src${suffix_q}/p_a_y.cpp
        generated_src${suffix_q}/n_o_d.cpp
        generated_src${suffix_q}/e_o.cpp
        generated_src${suffix_q}/n_o_o.cpp
        generated_src${suffix_q}/i_n_b.cpp
        generated_src${suffix_q}/i_n_o.cpp
        generated_src${suffix_q}/b_h.cpp
        generated_src${suffix_q}/c_o_x.cpp
        generated_src${suffix_q}/c_a_e.cpp
        generated_src${suffix_q}/e_e.cpp
    )
    set(
        multi_12_srcs
        generated_src${suffix_q}/a_h.cpp
        generated_src${suffix_q}/i_a.cpp
        generated_src${suffix_q}/o_z.cpp
        generated_src${suffix_q}/i_n_j.cpp
        generated_src${suffix_q}/i_n_l.cpp
        generated_src${suffix_q}/i_n_w.cpp
        generated_src${suffix_q}/d_i_h.cpp
        generated_src${suffix_q}/X.cpp
        generated_src${suffix_q}/o_g.cpp
        generated_src${suffix_q}/p_a_w.cpp
        generated_src${suffix_q}/c_a_f.cpp
        generated_src${suffix_q}/c_a_i.cpp
        generated_src${suffix_q}/w_y.cpp
        generated_src${suffix_q}/o_k.cpp
        generated_src${suffix_q}/d_i_b.cpp
        generated_src${suffix_q}/a_o.cpp
        generated_src${suffix_q}/d_w.cpp
        generated_src${suffix_q}/u_v.cpp
        generated_src${suffix_q}/e_j.cpp
        generated_src${suffix_q}/a_y.cpp
        generated_src${suffix_q}/n_o_i.cpp
        generated_src${suffix_q}/o_y.cpp
        generated_src${suffix_q}/c_t.cpp
        generated_src${suffix_q}/d_i_k.cpp
        generated_src${suffix_q}/c_a_g.cpp
        generated_src${suffix_q}/s_u_t.cpp
        generated_src${suffix_q}/c_z.cpp
        generated_src${suffix_q}/c_a_j.cpp
        generated_src${suffix_q}/u_g.cpp
        generated_src${suffix_q}/t_z.cpp
        generated_src${suffix_q}/s_u_a.cpp
        generated_src${suffix_q}/c_o_q.cpp
        generated_src${suffix_q}/c_o_y.cpp
        generated_src${suffix_q}/c_o_z.cpp
        generated_src${suffix_q}/r_y.cpp
        generated_src${suffix_q}/u_b.cpp
        generated_src${suffix_q}/m_n.cpp
        generated_src${suffix_q}/n_o_w.cpp
        generated_src${suffix_q}/i_p.cpp
        generated_src${suffix_q}/e_k.cpp
        generated_src${suffix_q}/i_v.cpp
        generated_src${suffix_q}/i_n_r.cpp
        generated_src${suffix_q}/d_e_k.cpp
        generated_src${suffix_q}/d_i_z.cpp
        generated_src${suffix_q}/i_n_p.cpp
        generated_src${suffix_q}/c_a_k.cpp
        generated_src${suffix_q}/r_e_k.cpp
        generated_src${suffix_q}/d_j.cpp
        generated_src${suffix_q}/a_a.cpp
        generated_src${suffix_q}/a_j.cpp
    )
    set(
        multi_13_srcs
        generated_src${suffix_q}/c_n.cpp
        generated_src${suffix_q}/u_d.cpp
        generated_src${suffix_q}/d_i_u.cpp
        generated_src${suffix_q}/o_h.cpp
        generated_src${suffix_q}/d_e_j.cpp
        generated_src${suffix_q}/c_o_h.cpp
        generated_src${suffix_q}/p_a_d.cpp
        generated_src${suffix_q}/s_v.cpp
        generated_src${suffix_q}/v_r.cpp
        generated_src${suffix_q}/s_u_e.cpp
        generated_src${suffix_q}/n_o_e.cpp
        generated_src${suffix_q}/t_c.cpp
        generated_src${suffix_q}/i_b.cpp
        generated_src${suffix_q}/u_n_y.cpp
        generated_src${suffix_q}/c_o_k.cpp
        generated_src${suffix_q}/c_a_y.cpp
        generated_src${suffix_q}/p_a_x.cpp
        generated_src${suffix_q}/c_o_y.cpp
        generated_src${suffix_q}/p_r_y.cpp
        generated_src${suffix_q}/b_d.cpp
        generated_src${suffix_q}/n_o_g.cpp
        generated_src${suffix_q}/i_n_m.cpp
        generated_src${suffix_q}/e_w.cpp
        generated_src${suffix_q}/p_a_b.cpp
        generated_src${suffix_q}/p_a_h.cpp
        generated_src${suffix_q}/u_n_z.cpp
        generated_src${suffix_q}/d_e_z.cpp
        generated_src${suffix_q}/c_a_h.cpp
        generated_src${suffix_q}/c_a_w.cpp
        generated_src${suffix_q}/u_k.cpp
        generated_src${suffix_q}/i_w.cpp
        generated_src${suffix_q}/f_y.cpp
        generated_src${suffix_q}/i_f.cpp
        generated_src${suffix_q}/s_f.cpp
        generated_src${suffix_q}/u_i.cpp
        generated_src${suffix_q}/g_w.cpp
        generated_src${suffix_q}/s_u_z.cpp
        generated_src${suffix_q}/i_z.cpp
        generated_src${suffix_q}/p_f.cpp
        generated_src${suffix_q}/i_k.cpp
        generated_src${suffix_q}/c_s.cpp
    )
    set(
        multi_14_srcs
        generated_src${suffix_q}/p_p.cpp
        generated_src${suffix_q}/u_x.cpp
        generated_src${suffix_q}/s_r.cpp
        generated_src${suffix_q}/l_l.cpp
        generated_src${suffix_q}/d_i_x.cpp
        generated_src${suffix_q}/n_o_y.cpp
        generated_src${suffix_q}/c_p.cpp
        generated_src${suffix_q}/i_x.cpp
        generated_src${suffix_q}/r_e_y.cpp
        generated_src${suffix_q}/p_a_j.cpp
        generated_src${suffix_q}/b_k.cpp
        generated_src${suffix_q}/p_a_a.cpp
        generated_src${suffix_q}/m_s.cpp
        generated_src${suffix_q}/c_c.cpp
        generated_src${suffix_q}/p_a_k.cpp
        generated_src${suffix_q}/m_t.cpp
        generated_src${suffix_q}/n_o_x.cpp
        generated_src${suffix_q}/n_o_l.cpp
        generated_src${suffix_q}/c_a_q.cpp
        generated_src${suffix_q}/d_i_i.cpp
        generated_src${suffix_q}/c_a_a.cpp
        generated_src${suffix_q}/c_a_o.cpp
        generated_src${suffix_q}/s_g.cpp
        generated_src${suffix_q}/w_l.cpp
        generated_src${suffix_q}/d_c.cpp
        generated_src${suffix_q}/r_c.cpp
        generated_src${suffix_q}/u_h.cpp
        generated_src${suffix_q}/m_b.cpp
        generated_src${suffix_q}/f_c.cpp
        generated_src${suffix_q}/g_p.cpp
        generated_src${suffix_q}/t_p.cpp
        generated_src${suffix_q}/d_s.cpp
        generated_src${suffix_q}/f_j.cpp
        generated_src${suffix_q}/e_h.cpp
        generated_src${suffix_q}/g_t.cpp
        generated_src${suffix_q}/m_p.cpp
        generated_src${suffix_q}/s_u_k.cpp
        generated_src${suffix_q}/d_e_y.cpp
        generated_src${suffix_q}/c_a_z.cpp
        generated_src${suffix_q}/u_f.cpp
        generated_src${suffix_q}/m_c.cpp
        generated_src${suffix_q}/s_d.cpp
        generated_src${suffix_q}/m_h.cpp
        generated_src${suffix_q}/s_s.cpp
        generated_src${suffix_q}/r_e_z.cpp
        generated_src${suffix_q}/s_t_h.cpp
    )
    set(
        multi_15_srcs
        generated_src${suffix_q}/d_z.cpp
        generated_src${suffix_q}/m_r.cpp
        generated_src${suffix_q}/l_h.cpp
        generated_src${suffix_q}/t_m.cpp
        generated_src${suffix_q}/l_c.cpp
        generated_src${suffix_q}/i_h.cpp
        generated_src${suffix_q}/c_w.cpp
        generated_src${suffix_q}/n_g.cpp
        generated_src${suffix_q}/r_f.cpp
        generated_src${suffix_q}/h_d.cpp
        generated_src${suffix_q}/p_k.cpp
        generated_src${suffix_q}/c_f.cpp
        generated_src${suffix_q}/n_o_p.cpp
        generated_src${suffix_q}/n_o_a.cpp
        generated_src${suffix_q}/t_j.cpp
        generated_src${suffix_q}/i_j.cpp
        generated_src${suffix_q}/d_v.cpp
        generated_src${suffix_q}/d_b.cpp
        generated_src${suffix_q}/f_t.cpp
        generated_src${suffix_q}/f_p.cpp
        generated_src${suffix_q}/b_t.cpp
        generated_src${suffix_q}/u_a.cpp
        generated_src${suffix_q}/t_l.cpp
        generated_src${suffix_q}/d_k.cpp
        generated_src${suffix_q}/m_g.cpp
        generated_src${suffix_q}/l_b.cpp
        generated_src${suffix_q}/h_w.cpp
        generated_src${suffix_q}/p_c.cpp
        generated_src${suffix_q}/c_m.cpp
        generated_src${suffix_q}/d_e_q.cpp
        generated_src${suffix_q}/r_e_x.cpp
        generated_src${suffix_q}/c_a_x.cpp
        generated_src${suffix_q}/p_a_o.cpp
        generated_src${suffix_q}/c_o_j.cpp
        generated_src${suffix_q}/d_i_w.cpp
        generated_src${suffix_q}/r_g.cpp
        generated_src${suffix_q}/s_b.cpp
        generated_src${suffix_q}/m_l.cpp
        generated_src${suffix_q}/t_b.cpp
        generated_src${suffix_q}/h_g.cpp
        generated_src${suffix_q}/d_t.cpp
        generated_src${suffix_q}/m_m.cpp
        generated_src${suffix_q}/l_w.cpp
        generated_src${suffix_q}/m_k.cpp
        generated_src${suffix_q}/b_s.cpp
        generated_src${suffix_q}/i_u.cpp
        generated_src${suffix_q}/u_c.cpp
        generated_src${suffix_q}/l_v.cpp
        generated_src${suffix_q}/h_c.cpp
        generated_src${suffix_q}/d_i_j.cpp
        generated_src${suffix_q}/v_l.cpp
        generated_src${suffix_q}/h_r.cpp
        generated_src${suffix_q}/b_b.cpp
        generated_src${suffix_q}/b_c.cpp
        generated_src${suffix_q}/m_f.cpp
        generated_src${suffix_q}/p_m.cpp
        generated_src${suffix_q}/r_t.cpp
        generated_src${suffix_q}/p_d.cpp
        generated_src${suffix_q}/c_d.cpp
        generated_src${suffix_q}/b_p.cpp
        generated_src${suffix_q}/n_o_z.cpp
        generated_src${suffix_q}/i_n_y.cpp
        generated_src${suffix_q}/i_n_z.cpp
        generated_src${suffix_q}/i_n_x.cpp
        generated_src${suffix_q}/d_i_y.cpp
        generated_src${suffix_q}/n_o_h.cpp
        generated_src${suffix_q}/v_y.cpp
        generated_src${suffix_q}/f_d.cpp
        generated_src${suffix_q}/u_z.cpp
        generated_src${suffix_q}/e_z.cpp
        generated_src${suffix_q}/p_a_z.cpp
        generated_src${suffix_q}/s_t_g.cpp
        generated_src${suffix_q}/t_r_p.cpp
        generated_src${suffix_q}/s_t_d.cpp
        generated_src${suffix_q}/p_a_q.cpp
    )
    set(
        multi_16_srcs
        generated_src${suffix_q}/s_j.cpp
        generated_src${suffix_q}/f_s.cpp
        generated_src${suffix_q}/r_m.cpp
        generated_src${suffix_q}/d_m.cpp
        generated_src${suffix_q}/n_r.cpp
        generated_src${suffix_q}/d_g.cpp
        generated_src${suffix_q}/f_w.cpp
        generated_src${suffix_q}/p_g.cpp
        generated_src${suffix_q}/g_d.cpp
        generated_src${suffix_q}/r_w.cpp
        generated_src${suffix_q}/r_p.cpp
        generated_src${suffix_q}/f_n.cpp
        generated_src${suffix_q}/d_d.cpp
        generated_src${suffix_q}/r_s.cpp
        generated_src${suffix_q}/p_w.cpp
        generated_src${suffix_q}/f_g.cpp
        generated_src${suffix_q}/l_s.cpp
        generated_src${suffix_q}/c_g.cpp
        generated_src${suffix_q}/d_l.cpp
        generated_src${suffix_q}/t_g.cpp
        generated_src${suffix_q}/l_n.cpp
        generated_src${suffix_q}/h_t.cpp
        generated_src${suffix_q}/h_l.cpp
        generated_src${suffix_q}/l_p.cpp
        generated_src${suffix_q}/s_u_o.cpp
        generated_src${suffix_q}/i_i.cpp
        generated_src${suffix_q}/o_q.cpp
        generated_src${suffix_q}/m_z.cpp
        generated_src${suffix_q}/g_m.cpp
        generated_src${suffix_q}/b_w.cpp
        generated_src${suffix_q}/m_w.cpp
        generated_src${suffix_q}/h_p.cpp
        generated_src${suffix_q}/h_s.cpp
        generated_src${suffix_q}/l_g.cpp
        generated_src${suffix_q}/w_k.cpp
        generated_src${suffix_q}/v_v.cpp
        generated_src${suffix_q}/n_t.cpp
        generated_src${suffix_q}/c_k.cpp
        generated_src${suffix_q}/b_x.cpp
        generated_src${suffix_q}/b_v.cpp
        generated_src${suffix_q}/n_s.cpp
        generated_src${suffix_q}/i_e.cpp
        generated_src${suffix_q}/l_x.cpp
        generated_src${suffix_q}/l_t.cpp
        generated_src${suffix_q}/i_q.cpp
        generated_src${suffix_q}/f_m.cpp
        generated_src${suffix_q}/f_f.cpp
        generated_src${suffix_q}/f_b.cpp
        generated_src${suffix_q}/d_p.cpp
        generated_src${suffix_q}/b_n.cpp
        generated_src${suffix_q}/w_m.cpp
        generated_src${suffix_q}/v_s.cpp
        generated_src${suffix_q}/t_n.cpp
        generated_src${suffix_q}/d_i_q.cpp
        generated_src${suffix_q}/s_u_w.cpp
        generated_src${suffix_q}/s_t_l.cpp
        generated_src${suffix_q}/t_r_f.cpp
        generated_src${suffix_q}/s_u_q.cpp
        generated_src${suffix_q}/p_r_s.cpp
        generated_src${suffix_q}/p_r_p.cpp
        generated_src${suffix_q}/p_r_n.cpp
        generated_src${suffix_q}/s_z.cpp
        generated_src${suffix_q}/p_a_f.cpp
        generated_src${suffix_q}/g_j.cpp
        generated_src${suffix_q}/l_r.cpp
        generated_src${suffix_q}/r_b.cpp
        generated_src${suffix_q}/g_c.cpp
        generated_src${suffix_q}/l_d.cpp
        generated_src${suffix_q}/n_p.cpp
        generated_src${suffix_q}/m_d.cpp
        generated_src${suffix_q}/r_l.cpp
        generated_src${suffix_q}/t_f.cpp
        generated_src${suffix_q}/h_h.cpp
        generated_src${suffix_q}/g_g.cpp
        generated_src${suffix_q}/r_d.cpp
        generated_src${suffix_q}/p_q.cpp
        generated_src${suffix_q}/n_v.cpp
        generated_src${suffix_q}/m_v.cpp
        generated_src${suffix_q}/l_m.cpp
        generated_src${suffix_q}/l_f.cpp
        generated_src${suffix_q}/h_m.cpp
        generated_src${suffix_q}/h_f.cpp
        generated_src${suffix_q}/h_b.cpp
        generated_src${suffix_q}/g_v.cpp
        generated_src${suffix_q}/f_v.cpp
        generated_src${suffix_q}/b_g.cpp
        generated_src${suffix_q}/b_f.cpp
        generated_src${suffix_q}/v_m.cpp
        generated_src${suffix_q}/w_b.cpp
        generated_src${suffix_q}/v_p.cpp
        generated_src${suffix_q}/v_g.cpp
        generated_src${suffix_q}/v_d.cpp
        generated_src${suffix_q}/v_b.cpp
        generated_src${suffix_q}/w_c.cpp
        generated_src${suffix_q}/w_d.cpp
        generated_src${suffix_q}/w_f.cpp
        generated_src${suffix_q}/o_j.cpp
        generated_src${suffix_q}/n_d.cpp
        generated_src${suffix_q}/d_n.cpp
        generated_src${suffix_q}/d_f.cpp
        generated_src${suffix_q}/p_b.cpp
        generated_src${suffix_q}/n_j.cpp
        generated_src${suffix_q}/i_y.cpp
        generated_src${suffix_q}/t_x.cpp
        generated_src${suffix_q}/t_k.cpp
        generated_src${suffix_q}/r_n.cpp
    )
    set(
        multi_17_srcs
        generated_src${suffix_q}/n_b.cpp
        generated_src${suffix_q}/h_v.cpp
        generated_src${suffix_q}/s_u_h.cpp
        generated_src${suffix_q}/p_r_l.cpp
        generated_src${suffix_q}/n_o_f.cpp
        generated_src${suffix_q}/s_t_c.cpp
        generated_src${suffix_q}/n_o_k.cpp
        generated_src${suffix_q}/s_u_u.cpp
        generated_src${suffix_q}/s_u_j.cpp
        generated_src${suffix_q}/s_t_b.cpp
        generated_src${suffix_q}/t_r_t.cpp
        generated_src${suffix_q}/t_r_s.cpp
        generated_src${suffix_q}/s_t_m.cpp
        generated_src${suffix_q}/s_t_k.cpp
        generated_src${suffix_q}/s_t_z.cpp
        generated_src${suffix_q}/s_t_x.cpp
        generated_src${suffix_q}/s_t_w.cpp
        generated_src${suffix_q}/s_t_v.cpp
        generated_src${suffix_q}/s_t_t.cpp
        generated_src${suffix_q}/s_t_s.cpp
        generated_src${suffix_q}/s_t_q.cpp
        generated_src${suffix_q}/s_t_p.cpp
        generated_src${suffix_q}/s_t_n.cpp
        generated_src${suffix_q}/s_t_j.cpp
        generated_src${suffix_q}/s_t_f.cpp
        generated_src${suffix_q}/p_r_f.cpp
        generated_src${suffix_q}/n_n.cpp
        generated_src${suffix_q}/r_v.cpp
        generated_src${suffix_q}/u_u.cpp
        generated_src${suffix_q}/r_r.cpp
        generated_src${suffix_q}/f_h.cpp
        generated_src${suffix_q}/u_j.cpp
        generated_src${suffix_q}/t_t.cpp
        generated_src${suffix_q}/t_d.cpp
        generated_src${suffix_q}/p_v.cpp
        generated_src${suffix_q}/n_c.cpp
        generated_src${suffix_q}/m_x.cpp
        generated_src${suffix_q}/h_n.cpp
        generated_src${suffix_q}/w_j.cpp
        generated_src${suffix_q}/t_v.cpp
        generated_src${suffix_q}/n_m.cpp
        generated_src${suffix_q}/n_l.cpp
        generated_src${suffix_q}/h_q.cpp
        generated_src${suffix_q}/g_s.cpp
        generated_src${suffix_q}/f_z.cpp
        generated_src${suffix_q}/d_x.cpp
        generated_src${suffix_q}/c_v.cpp
        generated_src${suffix_q}/c_q.cpp
        generated_src${suffix_q}/c_b.cpp
        generated_src${suffix_q}/b_z.cpp
        generated_src${suffix_q}/b_m.cpp
        generated_src${suffix_q}/w_p.cpp
        generated_src${suffix_q}/w_s.cpp
        generated_src${suffix_q}/w_t.cpp
        generated_src${suffix_q}/w_g.cpp
        generated_src${suffix_q}/v_t.cpp
        generated_src${suffix_q}/v_c.cpp
        generated_src${suffix_q}/u_n_x.cpp
        generated_src${suffix_q}/t_r_z.cpp
        generated_src${suffix_q}/t_r_x.cpp
        generated_src${suffix_q}/t_r_w.cpp
        generated_src${suffix_q}/t_r_v.cpp
        generated_src${suffix_q}/t_r_r.cpp
        generated_src${suffix_q}/t_r_q.cpp
        generated_src${suffix_q}/t_r_n.cpp
        generated_src${suffix_q}/t_r_m.cpp
        generated_src${suffix_q}/t_r_l.cpp
        generated_src${suffix_q}/t_r_k.cpp
        generated_src${suffix_q}/t_r_j.cpp
        generated_src${suffix_q}/t_r_h.cpp
        generated_src${suffix_q}/t_r_g.cpp
        generated_src${suffix_q}/t_r_d.cpp
        generated_src${suffix_q}/t_r_c.cpp
        generated_src${suffix_q}/t_r_b.cpp
        generated_src${suffix_q}/s_u_y.cpp
        generated_src${suffix_q}/s_u_x.cpp
        generated_src${suffix_q}/s_u_v.cpp
        generated_src${suffix_q}/p_r_z.cpp
        generated_src${suffix_q}/p_r_x.cpp
        generated_src${suffix_q}/p_r_w.cpp
        generated_src${suffix_q}/p_r_v.cpp
        generated_src${suffix_q}/p_r_t.cpp
        generated_src${suffix_q}/p_r_r.cpp
        generated_src${suffix_q}/p_r_q.cpp
        generated_src${suffix_q}/p_r_m.cpp
        generated_src${suffix_q}/p_r_k.cpp
        generated_src${suffix_q}/p_r_j.cpp
        generated_src${suffix_q}/p_r_h.cpp
        generated_src${suffix_q}/p_r_g.cpp
        generated_src${suffix_q}/p_r_d.cpp
        generated_src${suffix_q}/p_r_c.cpp
        generated_src${suffix_q}/p_r_b.cpp
        generated_src${suffix_q}/n_o_q.cpp
        generated_src${suffix_q}/n_o_j.cpp
        generated_src${suffix_q}/u_y.cpp
        generated_src${suffix_q}/u_w.cpp
        generated_src${suffix_q}/u_q.cpp
        generated_src${suffix_q}/u_o.cpp
        generated_src${suffix_q}/u_e.cpp
        generated_src${suffix_q}/t_q.cpp
        generated_src${suffix_q}/s_x.cpp
        generated_src${suffix_q}/r_z.cpp
        generated_src${suffix_q}/r_x.cpp
        generated_src${suffix_q}/r_q.cpp
        generated_src${suffix_q}/r_k.cpp
        generated_src${suffix_q}/r_j.cpp
        generated_src${suffix_q}/p_z.cpp
        generated_src${suffix_q}/p_x.cpp
        generated_src${suffix_q}/p_j.cpp
        generated_src${suffix_q}/n_z.cpp
        generated_src${suffix_q}/n_x.cpp
        generated_src${suffix_q}/n_w.cpp
        generated_src${suffix_q}/n_q.cpp
        generated_src${suffix_q}/n_k.cpp
        generated_src${suffix_q}/n_h.cpp
        generated_src${suffix_q}/n_f.cpp
        generated_src${suffix_q}/m_q.cpp
        generated_src${suffix_q}/m_j.cpp
        generated_src${suffix_q}/l_z.cpp
        generated_src${suffix_q}/l_q.cpp
        generated_src${suffix_q}/l_k.cpp
        generated_src${suffix_q}/l_j.cpp
        generated_src${suffix_q}/h_z.cpp
        generated_src${suffix_q}/h_x.cpp
        generated_src${suffix_q}/h_k.cpp
        generated_src${suffix_q}/h_j.cpp
        generated_src${suffix_q}/g_z.cpp
        generated_src${suffix_q}/g_x.cpp
        generated_src${suffix_q}/g_q.cpp
        generated_src${suffix_q}/g_k.cpp
        generated_src${suffix_q}/g_f.cpp
        generated_src${suffix_q}/g_b.cpp
        generated_src${suffix_q}/f_x.cpp
        generated_src${suffix_q}/f_q.cpp
        generated_src${suffix_q}/f_k.cpp
        generated_src${suffix_q}/d_q.cpp
        generated_src${suffix_q}/c_x.cpp
        generated_src${suffix_q}/c_j.cpp
        generated_src${suffix_q}/b_q.cpp
        generated_src${suffix_q}/b_j.cpp
        generated_src${suffix_q}/w_n.cpp
        generated_src${suffix_q}/v_z.cpp
        generated_src${suffix_q}/v_x.cpp
        generated_src${suffix_q}/v_w.cpp
        generated_src${suffix_q}/v_q.cpp
        generated_src${suffix_q}/v_n.cpp
        generated_src${suffix_q}/v_k.cpp
        generated_src${suffix_q}/v_j.cpp
        generated_src${suffix_q}/v_h.cpp
        generated_src${suffix_q}/v_f.cpp
        generated_src${suffix_q}/w_q.cpp
        generated_src${suffix_q}/w_v.cpp
        generated_src${suffix_q}/w_w.cpp
        generated_src${suffix_q}/w_x.cpp
        generated_src${suffix_q}/w_z.cpp
    )

    list(GET near_3 "0" near_3_first)
    list(LENGTH near_3 near_3_length)
    math(EXPR near_3_last_index "${near_3_length} - 1")
    list(GET near_3 ${near_3_last_index} near_3_last)
    foreach(multi_index RANGE 0 17)
        add_multi_lib(multi_${multi_index} "${multi_${multi_index}_srcs}")
        add_dependencies(matchmaker_multi_${multi_index} matchmaker_${near_3_first})
        add_dependencies(matchmaker_multi_${multi_index} matchmaker_${near_3_last})
    endforeach()

    foreach(multi_index_start RANGE 5 12)
        foreach(multi_index RANGE ${multi_index_start} 17 5)
            math(EXPR dep_index "${multi_index}-5")
            add_dependencies(matchmaker_multi_${multi_index} matchmaker_multi_${dep_index})
        endforeach()
    endforeach()

endif()


# matchmaker_srcs are single and two letter parents (no leaves)
set(matchmaker_srcs)

foreach(first_a_z_number RANGE 97 122)
    string(ASCII ${first_a_z_number} first_letter)

    if(q)
        if(NOT first_letter STREQUAL "q")
            continue()
        endif()
    else()
        if(first_letter STREQUAL "q")
            continue()
        endif()
    endif()

    if(
        first_letter STREQUAL "j" OR
        first_letter STREQUAL "k" OR
        first_letter STREQUAL "x" OR
        first_letter STREQUAL "y" OR
        first_letter STREQUAL "z"
    )
        continue()
    endif()

    # single letter parents
    list(APPEND matchmaker_srcs generated_src${suffix_q}/${first_letter}.cpp)

    # two letter parents
    foreach(second_a_z_number RANGE 97 122)
        string(ASCII ${second_a_z_number} second_letter)
        if(
            (q AND first_letter STREQUAL "q" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "n" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "o") OR
            (first_letter STREQUAL "r" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "i" AND second_letter STREQUAL "n") OR
            (first_letter STREQUAL "c" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "u") OR
            (first_letter STREQUAL "d" AND second_letter STREQUAL "i") OR
            (first_letter STREQUAL "d" AND second_letter STREQUAL "e") OR
            (first_letter STREQUAL "p" AND second_letter STREQUAL "a") OR
            (first_letter STREQUAL "s" AND second_letter STREQUAL "t") OR
            (first_letter STREQUAL "t" AND second_letter STREQUAL "r") OR
            (first_letter STREQUAL "u" AND second_letter STREQUAL "n")
        )
            list(APPEND matchmaker_srcs generated_src${suffix_q}/${first_letter}_${second_letter}.cpp)
        endif()
    endforeach()
endforeach()

list(APPEND matchmaker_srcs src/matchmaker.cpp)

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
