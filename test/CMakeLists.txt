
set(CONANFILE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
include(cmake/ConanSetup.cmake)
include(../ProtoGenerate.cmake)

add_library(test_types
  test_types.cpp
)
target_link_libraries(test_types
  proto
)
target_include_directories(test_types
  PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
)
# Generate code based on any annotations in test_types.hpp, using the compilation flags from test_types library.
set(PROTOGEN_HEADERS
  test_classes.hpp
  test_enums.hpp
  test_structs.hpp
)
proto_generate("${PROTOGEN_HEADERS}" test_types)

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
      proto
      test_types
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
