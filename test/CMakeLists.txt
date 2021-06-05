# enable CTEST
enable_testing()

include(cmake/ConanSetup.cmake)
include(../ProtoGenerate.cmake)

set(TESTS
    test_enum_visitor
    test_struct_visitor
    test_json_visitor
)


add_executable(main
    main.cpp
)
target_link_libraries(main
    proto
)
target_include_directories(main
    PRIVATE
    cmake
)
target_compile_features(main PRIVATE cxx_std_17)
proto_generate(test_types.hpp main)


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
    target_compile_features(${TEST} PRIVATE cxx_std_17)

    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()
