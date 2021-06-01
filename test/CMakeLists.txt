set(TESTS
    test_enum_visitor
    test_struct_visitor
    test_json_visitor
)

foreach(TEST ${TESTS})
    add_executable(${TEST}
        ${TEST}.cpp
    )
    target_include_directories(${TEST} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
    )
    target_link_libraries(${TEST}
        PRIVATE
            proto
            CONAN_PKG::gtest
            CONAN_PKG::nlohmann_json
    )

    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()
