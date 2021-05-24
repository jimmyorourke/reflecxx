cmake_minimum_required(VERSION 3.12)

project(simple_math)

# Set C/C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable CTEST
enable_testing()

include(cmake/ConanSetup.cmake)



add_subdirectory(libsimple_math)

add_executable(do_simple_math
    main.cpp
)
target_link_libraries(do_simple_math
    PRIVATE
        simple_math
)

install(TARGETS do_simple_math DESTINATION bin)
