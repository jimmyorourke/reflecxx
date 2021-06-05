# TEMP
set(PYTHON_COMMAND python)

set(PROTOGEN_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/generator/parse.py
    ${CMAKE_CURRENT_LIST_DIR}/generator/parse_types.py
    ${CMAKE_CURRENT_LIST_DIR}/generator/visitor_generator.py
)
set(PROTO_GEN_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

# TODO: test multiple headers
# Give directory for output, auto naming scheme
macro(proto_generate INPUT_FILES TARGET)
    set(CLANG_SHARED_OBJECT_DIRECTORY "C:\\Program Files\\LLVM\\bin")
    set(OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/generated_headers
    )

    # Use generator expressions to get the target's include directories and compilation options and flags. Using
    # generator expressions over get_target_property() as the gen expressions will have flags inherited from linked
    # targets. Otherwise we would have to recursively use get_target_property() on every library the target links.
    # Additionally, if any flags are set using generator expressions they won't have resolved yet at the time
    # get_target_property() is called.
    set(INC_DIR_GEN_EXPR "$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>")
    set(INCLUDE_DIRECTORIES "$<$<BOOL:${INC_DIR_GEN_EXPR}>:-I$<JOIN:${INC_DIR_GEN_EXPR}, -I>>")

    set(DEFS_GEN_EXPR "$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>")
    set(COMPILE_DEFINITIONS "$<$<BOOL:${DEFS_GEN_EXPR}>:-D$<JOIN:${DEFS_GEN_EXPR}, -D>>")

    set(COMPILE_OPTIONS "$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>")

    SET(ALL_FLAGS "${INCLUDE_DIRECTORIES};${COMPILE_DEFINITIONS};${COMPILE_OPTIONS}")

    # Convert cmake_cxx_flags into a list instead of a string
    string (REPLACE " " ";" GLOBAL_FLAGS "${CMAKE_CXX_FLAGS}")
    # Convert global MSVC definitions such as /DWIN32 /D_WINDOWS to clang compatible form
    string (REPLACE "/D" "-D" GLOBAL_FLAGS "${GLOBAL_FLAGS}")
    SET(ALL_FLAGS "${ALL_FLAGS};${GLOBAL_FLAGS}")

    # We've made assumptions that the flags will work on clang. This is definitely not true for MSVC flags. Rather than
    # trying to sort this all out, allow cland to ignore flags without warning.
    SET(ALL_FLAGS "${ALL_FLAGS};${GLOBAL_FLAGS};-Qunused-arguments")

    get_target_property(CXX_STD ${TARGET} CXX_STANDARD)
    if (NOT ${CXX_STD} MATCHES "NOTFOUND")
        SET(ALL_FLAGS "${ALL_FLAGS};-std=c++${CXX_STD}")
    else()
        SET(ALL_FLAGS "${ALL_FLAGS};-std=c++17")
    endif()

    list(REMOVE_DUPLICATES TARGET_FLAGS)

    add_custom_command(
        OUTPUT ${OUTPUT}
        COMMAND ${PYTHON_COMMAND} ${PROTO_GEN_BASE_DIR}/generator/parse.py
        --libclang-directory ${CLANG_SHARED_OBJECT_DIRECTORY}
        --input-files ${INPUT_FILES}
        --output-folder ${CMAKE_CURRENT_BINARY_DIR}/generated
        --flags="${ALL_FLAGS}"
        # so that source files can be provided with relative paths
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running PROTOGEN. Generating: ${OUTPUT}"
        DEPENDS ${INPUT} ${PROTOGEN_SOURCES}
        # Surprisingly, not using USES_TERMINAL gives better output because error messages show up at then end of the
        # output when generation fails.
        #USES_TERMINAL
        #VERBATIM
    )

    add_custom_target(${TARGET}_PROTOGEN
        DEPENDS
            ${OUTPUT}
    )
    # Automatically set up a dependency for the target whose flags we used on the generated target. The consumer can
    # just use the generated sources.
    add_dependencies(${TARGET} ${TARGET}_PROTOGEN)
endmacro()
