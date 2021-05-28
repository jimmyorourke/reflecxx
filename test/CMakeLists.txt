


add_executable(test_enum_visitor
    test_enum_visitor.cpp
)
target_include_directories(test_enum_visitor PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
)
target_link_libraries(test_enum_visitor
    PRIVATE
        proto
        CONAN_PKG::gtest
)

add_test(NAME test_enum_visitor
    COMMAND test_enum_visitor
)

add_executable(test_struct_visitor
    test_struct_visitor.cpp
)
target_include_directories(test_struct_visitor PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # to find the generated headers
)
target_link_libraries(test_struct_visitor
    PRIVATE
        proto
        CONAN_PKG::gtest
)

add_test(NAME test_struct_visitor
    COMMAND test_struct_visitor
)
