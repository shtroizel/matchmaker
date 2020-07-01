cmake_minimum_required(VERSION 3.13)

project(test_program VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")


get_filename_component(rpath_dir "${matchmaker_DIR}/.." REALPATH)
set(CMAKE_INSTALL_RPATH "${rpath_dir}")

find_package(matchmaker 1 REQUIRED)


add_executable(test_program src/test_program.cpp)
target_link_libraries(test_program matchmaker)
target_include_directories(test_program PRIVATE include)
install(TARGETS test_program DESTINATION bin)

find_program(ld_gold ld.gold)
if(NOT "${ld_gold}" STREQUAL "ld_gold-NOTFOUND")
    message(STATUS "found gold linker!")
    target_link_options(test_program PUBLIC -fuse-ld=gold)
endif()
