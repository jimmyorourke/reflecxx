

include(cmake/ConanSetup.cmake)
include(../ProtoGenerate.cmake)

set(TESTS
    test_enum_visitor
    test_struct_visitor
    test_json_visitor
)


add_library(test_types
    test_types.cpp
)
target_link_libraries(test_types
    proto
)
target_include_directories(test_types
    PUBLIC
    .
    cmake
    ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
)
# this is a bit of a hack. Really the test targets should run the generation.
proto_generate(test_types.hpp test_types)


foreach(TEST ${TESTS})
    add_executable(${TEST}
        ${TEST}.cpp
    )
    target_link_libraries(${TEST}
        PRIVATE
            proto
            test_types
            CONAN_PKG::gtest
            CONAN_PKG::nlohmann_json
    )
    target_compile_features(${TEST} PRIVATE cxx_std_17)

    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()
