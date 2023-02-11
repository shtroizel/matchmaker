cmake_minimum_required(VERSION 3.17)

project(data_reader_stage_0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

find_package(matchable 14.0.0 REQUIRED)
include_directories(
    ${matchable_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

##### install extracted data #####
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3201)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3202)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3203)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/51155)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/Crumbs)
add_custom_target(ex_3201 ALL)
add_custom_command(
    TARGET ex_3201
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3201/3201.zip
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3201/files.zip
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3201
    COMMENT "Unpacking 3201..."
    VERBATIM
)
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
add_custom_target(ex_3203 ALL)
add_custom_command(
    TARGET ex_3203
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3203/3203.zip
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/3203/files.zip
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/3203
    COMMENT "Unpacking 3203..."
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
add_custom_target(ex_Crumbs ALL)
add_custom_command(
    TARGET ex_Crumbs
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_SOURCE_DIR}/../data/Crumbs/Crumbs.tar.gz
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/matchmaker/${CMAKE_PROJECT_NAME}/data/Crumbs
    COMMENT "Unpacking Crumbs..."
    VERBATIM
)

set(
    srcs

    # compile these first
    src/SerialTask.h
    src/SerialTask.cpp
    src/Stage0Data.cpp
    src/Stage0Data.h
    src/data_reader_stage_0.cpp

    src/Prefix.cpp
    src/Prefix.h
    src/prepare_prefixes.cpp
    src/prepare_prefixes.h
    src/read_3201_acronyms.cpp
    src/read_3201_acronyms.h
    src/read_3201_common.cpp
    src/read_3201_common.h
    src/read_3201_compound.cpp
    src/read_3201_compound.h
    src/read_3201.cpp
    src/read_3201_crosswd.cpp
    src/read_3201_crosswd.h
    src/read_3201_crswd_d.cpp
    src/read_3201_crswd_d.h
    src/read_3201_fnames.cpp
    src/read_3201_fnames.h
    src/read_3201.h
    src/read_3201_mnames.cpp
    src/read_3201_mnames.h
    src/read_3201_names.cpp
    src/read_3201_names.h
    src/read_3201_places.cpp
    src/read_3201_places.h
    src/read_3201_single.cpp
    src/read_3201_single.h
    src/read_3202.cpp
    src/read_3202.h
    src/read_3203.cpp
    src/read_3203.h
    src/read_crumbs.cpp
    src/read_crumbs.h
    src/read_keys.cpp
    src/read_keys.h
    src/save_matchables.cpp
    src/save_matchables.h
)

add_executable(${CMAKE_PROJECT_NAME} ${srcs})
target_link_libraries(${CMAKE_PROJECT_NAME} matchablemaker_static)
include_directories(src)
add_dependencies(${CMAKE_PROJECT_NAME} ex_3201)
add_dependencies(${CMAKE_PROJECT_NAME} ex_3202)
add_dependencies(${CMAKE_PROJECT_NAME} ex_3203)
add_dependencies(${CMAKE_PROJECT_NAME} ex_51155-0)
add_dependencies(${CMAKE_PROJECT_NAME} ex_Crumbs)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
