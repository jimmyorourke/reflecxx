# Copyright (c) 2021-2022 Jimmy O'Rourke
# Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
# Official repository: https://github.com/jimmyorourke/reflecxx

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
else()
    if(NOT DEFINED CONANFILE_LOCATION)
        # We don't know
        set(CONANFILE_LOCATION "<path to source dir containing conanfile.txt>")
    endif()
    message(FATAL_ERROR "Error: could not find conanbuildinfo.cmake.
To fix this, do the following:
* Delete CMakeCache.txt:
    rm CMakeCache.txt (macOS, Linux)
    del CMakeCache.txt (Windows)
* Run conan to install needed dependencies:
    conan install ${CONANFILE_LOCATION}
* Run CMake generation again:
    cmake -G <Generator> [any other desired options] ..
")
endif()

conan_basic_setup(TARGETS NO_OUTPUT_DIRS KEEP_RPATHS)
