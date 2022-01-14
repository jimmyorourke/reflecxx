# Copyright (c) 2021-2022 Jimmy O'Rourke
# Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
# Official repository: https://github.com/jimmyorourke/reflecxx

cmake_minimum_required(VERSION 3.12)
project(reflecxx)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

option(BUILD_TESTS "Build the tests" ${MAIN_PROJECT})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(reflecxx)

if(BUILD_TESTS)
    # enable CTEST
    enable_testing()
    # requires Conan
    add_subdirectory(test)
endif()
