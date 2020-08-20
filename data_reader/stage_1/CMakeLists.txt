cmake_minimum_required(VERSION 3.10)

project(data_reader_stage_1 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

find_package(matchable 10.2.0 REQUIRED)
include_directories(${matchable_DIR}/../../../include)

get_filename_component(rpath_dir "${matchmaker_DIR}/.." REALPATH)
set(CMAKE_INSTALL_RPATH "${rpath_dir}")
find_package(matchmaker 1.2.1 REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} src/${CMAKE_PROJECT_NAME}.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} matchablemaker_static matchmaker stdc++fs)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

##### install extracted data #####
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3202)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/51155)
add_custom_target(ex_3202 ALL)
add_custom_command(
    TARGET ex_3202
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3202/3202.zip
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3202/files.zip
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3202
    COMMENT "Unpacking 3202..."
    VERBATIM
)
add_custom_target(ex_51155-0 ALL)
add_custom_command(
    TARGET ex_51155-0
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/51155/51155-0.zip
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/51155
    COMMENT "Unpacking 5115-0..."
    VERBATIM
)