# Copyright (c) 2021-2022 Jimmy O'Rourke
# Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
# Official repository: https://github.com/jimmyorourke/reflecxx

set(CONANFILE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
include(cmake/ConanSetup.cmake)

add_library(libtest_types
  libtest_types/type_helpers.cpp
)
target_link_libraries(libtest_types
  PUBLIC
    reflecxx
)
target_include_directories(libtest_types
  PUBLIC
    libtest_types/include
)
target_compile_definitions(libtest_types
  # testing that compile definitions are respected in generate code
  PUBLIC
    BASIC_STRUCT_HAS_B
)
# Generate code based on any annotations in these headers, using the compilation flags from libtypes target.
set(REFLECXX_HEADERS
  libtest_types/include/libtest_types/classes.hpp
  libtest_types/include/libtest_types/enums.hpp
  libtest_types/include/libtest_types/structs.hpp
)
reflecxx_generate("${REFLECXX_HEADERS}" libtest_types)


set(TESTS
  test_enum_visitor
  test_struct_visitor
  test_json_visitor
)

foreach(TEST ${TESTS})
  add_executable(${TEST}
    ${TEST}.cpp
  )
  target_link_libraries(${TEST}
    PRIVATE
      reflecxx
      libtest_types
      CONAN_PKG::gtest
  )
  add_test(NAME ${TEST}
    COMMAND ${TEST}
  )
endforeach()

target_link_libraries(test_json_visitor
  PRIVATE
    CONAN_PKG::nlohmann_json
)
