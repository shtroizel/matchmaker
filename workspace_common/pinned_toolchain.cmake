# The pinned toolchain: Clang, lld and libc++ from one LLVM install, named by MM_TOOLCHAIN (a cache entry,
# or the environment when a build directory is first configured).
#
# libc++, libc++abi and libunwind are linked into each library and program, so an install needs no C++
# runtime beside it and loads into programs built with any compiler and standard library.

if(NOT MM_TOOLCHAIN AND DEFINED ENV{MM_TOOLCHAIN})
    set(MM_TOOLCHAIN "$ENV{MM_TOOLCHAIN}" CACHE PATH "LLVM install holding the pinned toolchain")
endif()
if(NOT MM_TOOLCHAIN)
    message(FATAL_ERROR "MM_TOOLCHAIN must name the LLVM install holding the pinned toolchain")
endif()
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES MM_TOOLCHAIN)

set(CMAKE_C_COMPILER ${MM_TOOLCHAIN}/bin/clang)
set(CMAKE_CXX_COMPILER ${MM_TOOLCHAIN}/bin/clang++)

file(GLOB mm_runtime_dirs LIST_DIRECTORIES true ${MM_TOOLCHAIN}/lib ${MM_TOOLCHAIN}/lib/*)
set(mm_runtime "")
foreach(name c++ c++abi unwind)
    find_file(mm_lib${name} lib${name}.a PATHS ${mm_runtime_dirs} NO_DEFAULT_PATH)
    if(NOT mm_lib${name})
        message(FATAL_ERROR "no lib${name}.a under ${MM_TOOLCHAIN}/lib")
    endif()
    string(APPEND mm_runtime "${mm_lib${name}} ")
endforeach()

set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "${mm_runtime}-lpthread")
foreach(kind EXE SHARED MODULE)
    set(CMAKE_${kind}_LINKER_FLAGS_INIT "-fuse-ld=lld -nostdlib++")
endforeach()
