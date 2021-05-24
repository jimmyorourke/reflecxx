if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
else()
    message(FATAL_ERROR "Error: could not find conanbuildinfo.cmake.
To fix this, do the following:
* Delete CMakeCache.txt:
    rm CMakeCache.txt (macOS, Linux)
    del CMakeCache.txt (Windows)
* Run conan to install needed dependencies:
    conan install <path to source dir containing conanfile.txt>
* Run CMake generation again:
    cmake -G <Generator> [any other desired options] ..")
endif()

conan_basic_setup(TARGETS NO_OUTPUT_DIRS KEEP_RPATHS)
