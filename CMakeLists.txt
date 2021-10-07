cmake_minimum_required(VERSION 3.12)
project(proto_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(proto)

set(BUILD_PROTO_TESTS 1)

if(${BUILD_PROTO_TESTS})
    # enable CTEST
    enable_testing()
    # requires Conan
    add_subdirectory(test)
endif()
