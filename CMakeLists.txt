cmake_minimum_required(VERSION 3.13)

project(matchmaker VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "sparc64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=medany")
endif()


option(ATOMIC_LIBS "builds a library for every prefix" OFF)
option(q "build q (and Q) only" OFF)

option(MEM_127 "limit RAM usage to 127GB")
option(MEM_63 "limit RAM usage to 63GB")
option(MEM_31_14 "try limit RAM usage to 31GB, 14GB swap available")
option(MEM_15_30 "try limit RAM usage to 15GB, 30GB swap available")

set(mem_limit "140608")
if(MEM_127)
    set(mem_limit "127")
elseif(MEM_63)
    set(mem_limit "63")
elseif(MEM_31_14)
    set(mem_limit "31")
elseif(MEM_15_30)
    set(mem_limit "15")
endif()

if(q AND NOT ATOMIC_LIBS)
    message(FATAL_ERROR "ATOMIC_LIBS required when building q only")
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/matchmaker")

set(suffix_q "")
if(q)
    add_definitions(-DQ_ONLY)
    set(suffix_q "_q")
endif()

find_package(matchable 10.0.4 REQUIRED)
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


macro(add_single_prefix_libs_with_deps prefixes dependencies)
    foreach(prefix ${prefixes})
        if(NOT ATOMIC_LIBS)
            add_single_prefix_lib_without_deps(${prefix})
        endif()
        foreach(dependency ${dependencies})
            add_dependencies(matchmaker_${prefix} matchmaker_${dependency})
        endforeach()
    endforeach()
endmacro()


macro(add_single_prefix_libs_dep_on_prev prefixes)
    foreach(prefix ${prefixes})
        if(NOT ATOMIC_LIBS)
            add_single_prefix_lib_without_deps(${prefix})
        endif()
        if(prev_lib_group)
            foreach(dependency ${prev_lib_group})
                add_dependencies(matchmaker_${prefix} matchmaker_${dependency})
            endforeach()
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
        add_single_prefix_lib_without_deps(v)
        add_single_prefix_lib_without_deps(x)
        add_single_prefix_lib_without_deps(y)
        add_single_prefix_lib_without_deps(z)
    endif()

    foreach(first_a_z_number RANGE 97 122)
        string(ASCII ${first_a_z_number} first_letter)

        if(
            first_letter STREQUAL "j" OR
            first_letter STREQUAL "k" OR
            first_letter STREQUAL "v" OR
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
                (first_letter STREQUAL "u" AND second_letter STREQUAL "n")
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
    set(near_31)
    list(APPEND near_31 c_o)
    list(APPEND near_31 r_e)

    set(near_20)
    list(APPEND near_20 i_n)
    list(APPEND near_20 c_a)
    list(APPEND near_20 s_u)

    set(near_15)
    list(APPEND near_15 d_i)
    list(APPEND near_15 d_e)
    list(APPEND near_15 v)
    list(APPEND near_15 p_a)
    list(APPEND near_15 s_t)
    list(APPEND near_15 t_r)
    list(APPEND near_15 p_r_e)
    list(APPEND near_15 o_v)

    set(near_12)
    list(APPEND near_12 m_a)
    list(APPEND near_12 a_n)
    list(APPEND near_12 c_h)
    list(APPEND near_12 s_e)
    list(APPEND near_12 p_e)

    set(near_10)
    list(APPEND near_10 m_i)
    list(APPEND near_10 p_o)
    list(APPEND near_10 m_e)
    list(APPEND near_10 b_a)
    list(APPEND near_10 h_e)

    set(near_8_0)
    list(APPEND near_8_0 h_a)
    list(APPEND near_8_0 b_e)

    set(near_8_1)
    list(APPEND near_8_1 k)
    list(APPEND near_8_1 A)

    set(near_8_2)
    list(APPEND near_8_2 p_h)
    list(APPEND near_8_2 m_o)

    set(near_8_3)
    list(APPEND near_8_3 u_n_d)
    list(APPEND near_8_3 p_r_o)

    set(near_8_4)
    list(APPEND near_8_4 t_h)
    list(APPEND near_8_4 S)

    set(near_8_5)
    list(APPEND near_8_5 u_n_s)
    list(APPEND near_8_5 l_a)

    set(near_8_6)
    list(APPEND near_8_6 e_x)
    list(APPEND near_8_6 C)

    set(near_8_7)
    list(APPEND near_8_7 j)
    list(APPEND near_8_7 s_p)

    set(near_8_8)
    list(APPEND near_8_8 c_r)
    list(APPEND near_8_8 M)

    set(near_8_9)
    list(APPEND near_8_9 h_o)
    list(APPEND near_8_9 B)

    set(near_8_10)
    list(APPEND near_8_10 f_o)
    list(APPEND near_8_10 s_c)

    set(near_8_11)
    list(APPEND near_8_11 b_r)
    list(APPEND near_8_11 e_n)

    set(near_8_12)
    list(APPEND near_8_12 t_e)
    list(APPEND near_8_12 t_a)

    set(near_8_13)
    list(APPEND near_8_13 s_a)
    list(APPEND near_8_13 h_y)

    set(near_8)
    foreach(n_8_index RANGE 0 13)
        foreach(n_8_lib ${near_8_${n_8_index}})
            list(APPEND near_8 ${n_8_lib})
        endforeach()
    endforeach()

    set(near_7)
    list(APPEND near_7 s_h)
    list(APPEND near_7 a_r)
    list(APPEND near_7 a_l)
    list(APPEND near_7 b_o)
    list(APPEND near_7 P)
    list(APPEND near_7 l_i)
    list(APPEND near_7 L)
    list(APPEND near_7 H)
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
    list(APPEND near_7 D)
    list(APPEND near_7 T)
    list(APPEND near_7 b_u)
    list(APPEND near_7 m_u)
    list(APPEND near_7 d_o)
    list(APPEND near_7 n_e)
    list(APPEND near_7 i_m)
    list(APPEND near_7 q)
    list(APPEND near_7 K)
    list(APPEND near_7 c_l)
    list(APPEND near_7 w_a)

    set(near_5)
    list(APPEND near_5 l_e)
    list(APPEND near_5 R)
    list(APPEND near_5 f_a)
    list(APPEND near_5 f_i)
    list(APPEND near_5 u_n_p)
    list(APPEND near_5 a_m)
    list(APPEND near_5 E)
    list(APPEND near_5 l_o)
    list(APPEND near_5 f_r)
    list(APPEND near_5 r_o)
    list(APPEND near_5 b_l)
    list(APPEND near_5 p_u)
    list(APPEND near_5 a_p)
    list(APPEND near_5 c_u)
    list(APPEND near_5 a_s)
    list(APPEND near_5 c_e)
    list(APPEND near_5 p_s)
    list(APPEND near_5 g_e)
    list(APPEND near_5 f_e)

    set(near_4)
    list(APPEND near_4 F)
    list(APPEND near_4 a_u)
    list(APPEND near_4 d_a)
    list(APPEND near_4 a_d)
    list(APPEND near_4 u_n_a)
    list(APPEND near_4 w_i)
    list(APPEND near_4 u_n_r)
    list(APPEND near_4 s_y)
    list(APPEND near_4 g_l)
    list(APPEND near_4 u_n_i)
    list(APPEND near_4 o_r)
    list(APPEND near_4 N)
    list(APPEND near_4 g_o)
    list(APPEND near_4 t_i)
    list(APPEND near_4 z)
    list(APPEND near_4 t_u)
    list(APPEND near_4 e_l)
    list(APPEND near_4 d_r)
    list(APPEND near_4 a_b)
    list(APPEND near_4 e_p)
    list(APPEND near_4 s_l)
    list(APPEND near_4 J)
    list(APPEND near_4 W)
    list(APPEND near_4 c_i)

    set(near_3)
    list(APPEND near_3 w_h)
    list(APPEND near_3 e_m)
    list(APPEND near_3 n_a)
    list(APPEND near_3 f_u)
    list(APPEND near_3 u_n_e)
    list(APPEND near_3 y)
    list(APPEND near_3 u_n_f)
    list(APPEND near_3 g_u)
    list(APPEND near_3 r_i)
    list(APPEND near_3 w_o)
    list(APPEND near_3 u_n_t)
    list(APPEND near_3 h_i)
    list(APPEND near_3 d_u)
    list(APPEND near_3 c_y)
    list(APPEND near_3 u_n_m)
    list(APPEND near_3 u_n_b)
    list(APPEND near_3 O)
    list(APPEND near_3 w_e)
    list(APPEND near_3 i_s)
    list(APPEND near_3 r_u)
    list(APPEND near_3 I)
    list(APPEND near_3 n_i)
    list(APPEND near_3 m_y)
    list(APPEND near_3 s_w)
    list(APPEND near_3 h_u)
    list(APPEND near_3 u_p)
    list(APPEND near_3 a_t)

    macro(add_near_group near_suffix)
        math(EXPR group_size "${mem_limit} / ${near_suffix}")
        list(LENGTH near_${near_suffix} near_length)
        set(index "0")
        while(1)
            if(index GREATER_EQUAL ${near_length})
                break()
            endif()

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

            list(GET near_${near_suffix} ${index} cur)
            list(APPEND group ${cur})
            add_single_prefix_libs_dep_on_prev("${group}")
            math(EXPR index "${index} + 1")
        endwhile()
    endmacro()

    if(MEM_127)
        add_single_prefix_libs_without_deps("${near_31}")
        add_single_prefix_libs_without_deps("${near_20}")
        add_single_prefix_libs_without_deps("${near_15}")

        foreach(index RANGE 0 4)
            list(GET near_15 ${index} lib_15)
            list(GET near_12 ${index} lib_12)
            list(GET near_10 ${index} lib_10)
            add_single_prefix_libs_with_deps("${lib_12}" "${lib_15}")
            add_single_prefix_libs_with_deps("${lib_10}" "${lib_12}")
        endforeach()

        foreach(n_8_x RANGE 0 13)
            list(GET near_8_${n_8_x} 0 lib_0)
            add_single_prefix_libs_with_deps("${lib_0}" "${near_20}")
            list(GET near_8_${n_8_x} 1 lib_1)
            add_single_prefix_libs_with_deps("${lib_1}" "${lib_0}")
        endforeach()

        # first time adding near group so prev_lib_group not yet set
        # manually depend on near_8 after adding...
        add_near_group(7)
        foreach(n_7 ${near_7})
            foreach(n_8 ${near_8})
                add_dependencies("matchmaker_${n_7}" "matchmaker_${n_8}")
            endforeach()
        endforeach()

        add_near_group(5)
        add_near_group(4)
        add_near_group(3)

    elseif(MEM_63 OR MEM_31_14 OR MEM_15_30)
        add_near_group(31)
        add_near_group(20)
        add_near_group(15)
        add_near_group(12)
        add_near_group(10)
        add_near_group(8)
        add_near_group(7)
        add_near_group(5)
        add_near_group(4)
        add_near_group(3)
    else()
        add_single_prefix_lib_without_deps("${near_31}")
        add_single_prefix_lib_without_deps("${near_20}")
        add_single_prefix_lib_without_deps("${near_15}")
        add_single_prefix_lib_without_deps("${near_12}")
        add_single_prefix_lib_without_deps("${near_8}")
        add_single_prefix_lib_without_deps("${near_7}")
        add_single_prefix_lib_without_deps("${near_5}")
        add_single_prefix_lib_without_deps("${near_4}")
        add_single_prefix_lib_without_deps("${near_3}")
    endif()

endif()


if(NOT ATOMIC_LIBS)
    set(
        multi_0_srcs
        generated_src${suffix_q}/s_n.cpp
        generated_src${suffix_q}/l_u.cpp
        generated_src${suffix_q}/V.cpp
        generated_src${suffix_q}/a_g.cpp
    )
    set(
        multi_1_srcs
        generated_src${suffix_q}/s_k.cpp
        generated_src${suffix_q}/o_p.cpp
        generated_src${suffix_q}/o_b.cpp
        generated_src${suffix_q}/g_i.cpp
        generated_src${suffix_q}/e_s.cpp
    )
    set(
        multi_2_srcs
        generated_src${suffix_q}/p_y.cpp
        generated_src${suffix_q}/u_r.cpp
        generated_src${suffix_q}/i_r.cpp
        generated_src${suffix_q}/r_h.cpp
        generated_src${suffix_q}/u_n_h.cpp
        generated_src${suffix_q}/n_u.cpp
    )
    set(
        multi_3_srcs
        generated_src${suffix_q}/u_n_g.cpp
        generated_src${suffix_q}/p_r_i.cpp
        generated_src${suffix_q}/e_r.cpp
        generated_src${suffix_q}/e_u.cpp
        generated_src${suffix_q}/a_f.cpp
        generated_src${suffix_q}/u_n_l.cpp
        generated_src${suffix_q}/o_s.cpp
    )
    set(
        multi_4_srcs
        generated_src${suffix_q}/e_c.cpp
        generated_src${suffix_q}/o_c.cpp
        generated_src${suffix_q}/s_q.cpp
        generated_src${suffix_q}/s_m.cpp
        generated_src${suffix_q}/u_n_w.cpp
        generated_src${suffix_q}/a_e.cpp
        generated_src${suffix_q}/e_t.cpp
        generated_src${suffix_q}/x.cpp
    )
    set(
        multi_5_srcs
        generated_src${suffix_q}/e_v.cpp
        generated_src${suffix_q}/u_l.cpp
        generated_src${suffix_q}/i_d.cpp
        generated_src${suffix_q}/u_n_v.cpp
        generated_src${suffix_q}/t_w.cpp
        generated_src${suffix_q}/o_x.cpp
        generated_src${suffix_q}/t_y.cpp
        generated_src${suffix_q}/Z.cpp
        generated_src${suffix_q}/u_n_o.cpp
    )
    set(
        multi_6_srcs
        generated_src${suffix_q}/w_r.cpp
        generated_src${suffix_q}/g_y.cpp
        generated_src${suffix_q}/i_l.cpp
        generated_src${suffix_q}/U.cpp
        generated_src${suffix_q}/a_v.cpp
        generated_src${suffix_q}/p_r_a.cpp
        generated_src${suffix_q}/d_y.cpp
        generated_src${suffix_q}/a_i.cpp
        generated_src${suffix_q}/l_y.cpp
        generated_src${suffix_q}/o_l.cpp
    )
    set(
        multi_7_srcs
        generated_src${suffix_q}/e_a.cpp
        generated_src${suffix_q}/e_q.cpp
        generated_src${suffix_q}/o_n.cpp
        generated_src${suffix_q}/i_c.cpp
        generated_src${suffix_q}/Y.cpp
        generated_src${suffix_q}/o_m.cpp
        generated_src${suffix_q}/u_n_n.cpp
        generated_src${suffix_q}/e_d.cpp
        generated_src${suffix_q}/e_f.cpp
        generated_src${suffix_q}/n_o_t.cpp
        generated_src${suffix_q}/o_d.cpp
        generated_src${suffix_q}/n_o_n.cpp
    )
    set(
        multi_8_srcs
        generated_src${suffix_q}/o_t.cpp
        generated_src${suffix_q}/a_z.cpp
        generated_src${suffix_q}/o_o.cpp
        generated_src${suffix_q}/o_f.cpp
        generated_src${suffix_q}/p_t.cpp
        generated_src${suffix_q}/p_n.cpp
        generated_src${suffix_q}/n_o_r.cpp
        generated_src${suffix_q}/u_m.cpp
        generated_src${suffix_q}/e_g.cpp
        generated_src${suffix_q}/u_t.cpp
        generated_src${suffix_q}/a_w.cpp
        generated_src${suffix_q}/i_o.cpp
    )
    set(
        multi_9_srcs
        generated_src${suffix_q}/e_y.cpp
        generated_src${suffix_q}/u_s.cpp
        generated_src${suffix_q}/g_n.cpp
        generated_src${suffix_q}/a_x.cpp
        generated_src${suffix_q}/g_h.cpp
        generated_src${suffix_q}/b_y.cpp
        generated_src${suffix_q}/i_t.cpp
        generated_src${suffix_q}/a_q.cpp
        generated_src${suffix_q}/Q.cpp
        generated_src${suffix_q}/n_o_s.cpp
        generated_src${suffix_q}/n_o_v.cpp
        generated_src${suffix_q}/n_o_m.cpp
        generated_src${suffix_q}/u_n_j.cpp
        generated_src${suffix_q}/u_n_k.cpp
    )
    set(
        multi_10_srcs
        generated_src${suffix_q}/u_n_q.cpp
        generated_src${suffix_q}/Q.cpp
        generated_src${suffix_q}/o_i.cpp
        generated_src${suffix_q}/o_e.cpp
        generated_src${suffix_q}/u_n_u.cpp
        generated_src${suffix_q}/i_g.cpp
        generated_src${suffix_q}/e_i.cpp
        generated_src${suffix_q}/p_r_u.cpp
        generated_src${suffix_q}/o_a.cpp
        generated_src${suffix_q}/n_y.cpp
        generated_src${suffix_q}/a_k.cpp
        generated_src${suffix_q}/n_o_c.cpp
        generated_src${suffix_q}/d_h.cpp
        generated_src${suffix_q}/e_b.cpp
        generated_src${suffix_q}/t_s.cpp
        generated_src${suffix_q}/o_w.cpp
    )
    set(
        multi_11_srcs
        generated_src${suffix_q}/w_u.cpp
        generated_src${suffix_q}/n_o_u.cpp
        generated_src${suffix_q}/n_o_b.cpp
        generated_src${suffix_q}/n_o_d.cpp
        generated_src${suffix_q}/e_o.cpp
        generated_src${suffix_q}/n_o_o.cpp
        generated_src${suffix_q}/b_h.cpp
        generated_src${suffix_q}/e_e.cpp
        generated_src${suffix_q}/a_h.cpp
        generated_src${suffix_q}/i_a.cpp
        generated_src${suffix_q}/o_z.cpp
        generated_src${suffix_q}/X.cpp
        generated_src${suffix_q}/o_g.cpp
        generated_src${suffix_q}/w_y.cpp
        generated_src${suffix_q}/o_k.cpp
        generated_src${suffix_q}/a_o.cpp
        generated_src${suffix_q}/d_w.cpp
        generated_src${suffix_q}/u_v.cpp
    )
    set(
        multi_12_srcs
        generated_src${suffix_q}/e_j.cpp
        generated_src${suffix_q}/a_y.cpp
        generated_src${suffix_q}/n_o_i.cpp
        generated_src${suffix_q}/o_y.cpp
        generated_src${suffix_q}/c_t.cpp
        generated_src${suffix_q}/c_z.cpp
        generated_src${suffix_q}/u_g.cpp
        generated_src${suffix_q}/t_z.cpp
        generated_src${suffix_q}/r_y.cpp
        generated_src${suffix_q}/u_b.cpp
        generated_src${suffix_q}/m_n.cpp
        generated_src${suffix_q}/n_o_w.cpp
        generated_src${suffix_q}/i_p.cpp
        generated_src${suffix_q}/e_k.cpp
        generated_src${suffix_q}/i_v.cpp
        generated_src${suffix_q}/d_j.cpp
        generated_src${suffix_q}/a_a.cpp
        generated_src${suffix_q}/a_j.cpp
        generated_src${suffix_q}/c_n.cpp
        generated_src${suffix_q}/u_d.cpp
        generated_src${suffix_q}/o_h.cpp
    )
    set(
        multi_13_srcs
        generated_src${suffix_q}/s_v.cpp
        generated_src${suffix_q}/n_o_e.cpp
        generated_src${suffix_q}/t_c.cpp
        generated_src${suffix_q}/i_b.cpp
        generated_src${suffix_q}/u_n_y.cpp
        generated_src${suffix_q}/p_r_y.cpp
        generated_src${suffix_q}/b_d.cpp
        generated_src${suffix_q}/n_o_g.cpp
        generated_src${suffix_q}/e_w.cpp
        generated_src${suffix_q}/u_n_z.cpp
        generated_src${suffix_q}/u_k.cpp
        generated_src${suffix_q}/i_w.cpp
        generated_src${suffix_q}/f_y.cpp
        generated_src${suffix_q}/i_f.cpp
        generated_src${suffix_q}/s_f.cpp
        generated_src${suffix_q}/u_i.cpp
        generated_src${suffix_q}/g_w.cpp
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
        generated_src${suffix_q}/n_o_y.cpp
        generated_src${suffix_q}/c_p.cpp
        generated_src${suffix_q}/i_x.cpp
        generated_src${suffix_q}/b_k.cpp
        generated_src${suffix_q}/m_s.cpp
        generated_src${suffix_q}/c_c.cpp
        generated_src${suffix_q}/m_t.cpp
        generated_src${suffix_q}/n_o_x.cpp
        generated_src${suffix_q}/n_o_l.cpp
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
        generated_src${suffix_q}/u_f.cpp
        generated_src${suffix_q}/m_c.cpp
        generated_src${suffix_q}/s_d.cpp
        generated_src${suffix_q}/m_h.cpp
        generated_src${suffix_q}/s_s.cpp
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
        generated_src${suffix_q}/n_o_h.cpp
        generated_src${suffix_q}/f_d.cpp
        generated_src${suffix_q}/u_z.cpp
        generated_src${suffix_q}/e_z.cpp
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
        generated_src${suffix_q}/t_n.cpp
        generated_src${suffix_q}/p_r_s.cpp
        generated_src${suffix_q}/p_r_p.cpp
        generated_src${suffix_q}/p_r_n.cpp
        generated_src${suffix_q}/s_z.cpp
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
        generated_src${suffix_q}/w_b.cpp
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
        generated_src${suffix_q}/p_r_l.cpp
        generated_src${suffix_q}/n_o_f.cpp
        generated_src${suffix_q}/n_o_k.cpp
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
        generated_src${suffix_q}/u_n_x.cpp
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
        generated_src${suffix_q}/w_q.cpp
        generated_src${suffix_q}/w_v.cpp
        generated_src${suffix_q}/w_w.cpp
        generated_src${suffix_q}/w_x.cpp
        generated_src${suffix_q}/w_z.cpp
    )

    foreach(multi_index RANGE 0 17)
        add_multi_lib(multi_${multi_index} "${multi_${multi_index}_srcs}")
        add_dependencies(matchmaker_multi_${multi_index} matchmaker_a_t)
    endforeach()

    if(NOT MEM_127)
        foreach(multi_index_start RANGE 5 12)
            foreach(multi_index RANGE ${multi_index_start} 17 5)
                math(EXPR dep_index "${multi_index}-5")
                add_dependencies(matchmaker_multi_${multi_index} matchmaker_multi_${dep_index})
            endforeach()
        endforeach()
    endif()

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
        first_letter STREQUAL "v" OR
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
