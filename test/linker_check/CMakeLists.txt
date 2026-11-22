# configures only: refuses MM_LINKER when the compiler can't link with it
cmake_minimum_required(VERSION 3.16.3)
project(linker_check CXX)
include(${MATCHMAKER_ROOT}/workspace_common/link_pool.cmake)
mm_check_linker("${MM_LINKER}")
