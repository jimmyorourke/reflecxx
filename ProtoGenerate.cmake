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

    # Get flags from target
    # By using generator expressions we can get recursive results
    set(FLAGS "-I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>, -I>")

    set(DEFS    "$<$<BOOL:$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>>:-D$<JOIN:$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>, -D>>")

    set(OPTS    "$<$<BOOL:$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>>:-D$<JOIN:$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>, -D>>")
        #$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>
    #)
    SET(SD $<TARGET_PROPERTY:${TARGET},CXX_STANDARD> $<TARGET_PROPERTY:${TARGET},CXX_STANDARD>)

    SET(ALL_FLAGS "${FLAGS};${DEFS};${OPTS};${CMAKE_CXX_FLAGS}")
    SET(ALL_FLAGS "${FLAGS};${SD}")


    get_target_property(STD ${TARGET} CXX_STANDARD)
    if (NOT ${STD} MATCHES "NOTFOUND")
        SET(ALL_FLAGS "${ALL_FLAGS};-std=c++${STD}")
    endif()
    message("STANDARD is ${STD}")

    add_custom_command(
        OUTPUT ${OUTPUT}
        COMMAND ${PYTHON_COMMAND} ${PROTO_GEN_BASE_DIR}/generator/parse.py
        --libclang-directory ${CLANG_SHARED_OBJECT_DIRECTORY}
        --input-files ${INPUT_FILES}
        --output-file ${OUTPUT}
        --flags="${ALL_FLAGS}"
        # so that source files can be provided with relative paths
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running PROTOGEN. Generating: ${OUTPUT}"
        DEPENDS ${INPUT} ${PROTOGEN_SOURCES}
        # Surprisingly, not using USES_TERMINAL gives better output because error messages show up at then end of the
        # output when generation fails.
        #USES_TERMINAL
    )

    add_custom_target(${TARGET}_PROTOGEN
        DEPENDS
            ${OUTPUT}
    )
    # Automatically set up a dependency for the target whose flags we used on the generated target. The consumer can
    # just use the generated sources.
    add_dependencies(${TARGET} ${TARGET}_PROTOGEN)
endmacro()
