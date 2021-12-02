
set(CONANFILE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
include(cmake/ConanSetup.cmake)
include(../ProtoGenerate.cmake)

add_library(libtypes
  types.cpp
)
target_link_libraries(libtypes
  reflecxx
)
target_include_directories(libtypes
  PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
)
target_compile_definitions(libtypes
  PUBLIC
    BASIC_STRUCT_HAS_B
)
# Generate code based on any annotations in these headers, using the compilation flags from libtypes target.
set(PROTOGEN_HEADERS
  classes.hpp
  enums.hpp
  structs.hpp
)
reflecxx_generate("${PROTOGEN_HEADERS}" libtypes)

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
      libtypes
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
