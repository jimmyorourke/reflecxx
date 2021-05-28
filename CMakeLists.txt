cmake_minimum_required(VERSION 3.12)
project(proto_test)

# enable CTEST
enable_testing()

include(test/cmake/ConanSetup.cmake)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(proto)
add_subdirectory(test)
