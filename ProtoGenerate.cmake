
# ${PYTHON_COMMAND} can be set to an appropriate invocation of the python interpreter.
# E.g. if using CMake's FindPython, then ${Python_EXECUTABLE}, if using pipenv then "pipenv run python" or similar, etc.
if (NOT DEFINED PYTHON_COMMAND)
  # Reasonable guesses
  if (WIN32)
    set(PYTHON_COMMAND python)
  else()
    set(PYTHON_COMMAND python3)
  endif()
endif()

# ${CLANG_SHARED_OBJECT_DIRECTORY} should be set to the location of libclang.<so|dyld|dll>.
# We try to supply a reasonable default but it may need to be overridden, e.g. if clang is installed through conan.
if (NOT DEFINED CLANG_SHARED_OBJECT_DIRECTORY)
  # Reasonable guesses
  if (WIN32)
    set(CLANG_SHARED_OBJECT_DIRECTORY "C:\\Program Files\\LLVM\\bin")
  elseif (APPLE)
    # This is the command line tools path. For full xcode it might be
    # /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib.
    set(CLANG_SHARED_OBJECT_DIRECTORY "/Library/Developer/CommandLineTools/usr/lib/")
  else()
    set(CLANG_SHARED_OBJECT_DIRECTORY "/usr/lib/x86_64-linux-gnu/")
  endif()
endif()

set(PROTOGEN_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/generator/parse.py
  ${CMAKE_CURRENT_LIST_DIR}/generator/parse_types.py
  ${CMAKE_CURRENT_LIST_DIR}/generator/visitor_generator.py
)
set(PROTO_GEN_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

macro(get_compilation_flags TARGET FLAGS_OUT)
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

  SET(FF "${INCLUDE_DIRECTORIES};${COMPILE_DEFINITIONS};${COMPILE_OPTIONS}")

  # Convert cmake_cxx_flags into a list instead of a string
  string (REPLACE " " ";" GLOBAL_FLAGS "${CMAKE_CXX_FLAGS}")
  # Convert global MSVC style definitions such as /DWIN32 /D_WINDOWS to clang compatible form
  string (REPLACE "/D" "-D" GLOBAL_FLAGS "${GLOBAL_FLAGS}")
  SET(FLAGS "${FF};${GLOBAL_FLAGS}")

  # We've made assumptions that the flags will work on clang. This is definitely not true for MSVC flags. Rather than
  # trying to sort this all out, allow clang to ignore flags without warning.
  SET(FLAGS "${FLAGS};${GLOBAL_FLAGS};-Qunused-arguments")

  get_target_property(CXX_STD ${TARGET} CXX_STANDARD)
  if (NOT ${CXX_STD} MATCHES "NOTFOUND")
    SET(FLAGS "${FLAGS};-std=c++${CXX_STD}")
  else()
    # best guess
    SET(FLAGS "${FLAGS};-std=c++17")
  endif()

  list(REMOVE_DUPLICATES FLAGS)
  set(${FLAGS_OUT} "${FLAGS}" PARENT_SCOPE)
endmacro()

# TODO: test multiple headers
# Give directory for output, auto naming scheme
macro(proto_generate INPUT_FILES TARGET)
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_headers)
  # Since the generated output file names will be determined based on the input, we will use a trick to ensure cmake
  # doesn't run the custom_command on every build if it doesn't have to. We'll just touch an indicator file.
  # This wouldn't be necessary if cmake could track directories at outputs but it doesn't seem to be able to.
  set(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_headers/generated.txt)

  get_compilation_flags(${TARGET} FLAGS)

  add_custom_command(
    OUTPUT ${OUTPUT}
    COMMAND ${PYTHON_COMMAND} ${PROTO_GEN_BASE_DIR}/generator/parse.py
    --libclang-directory ${CLANG_SHARED_OBJECT_DIRECTORY}
    --input-files ${INPUT_FILES}
    --output-folder ${OUTPUT_DIR}
    --flags="${FLAGS}"
    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
    # so that source files can be provided with relative paths
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running PROTOGEN. Generating into: ${OUTPUT_DIR}"
    DEPENDS ${INPUT_FILES} ${PROTOGEN_SOURCES}
    # Surprisingly, not using USES_TERMINAL gives better output because error messages show up at the end of the
    # output when generation fails.
    #USES_TERMINAL
    #VERBATIM
  )
  # Have targets depend on the indicator file.
  add_custom_target(${TARGET}_PROTOGEN
    DEPENDS
      ${OUTPUT_DIR}/generated.txt
  )

  set_target_properties(${TARGET} PROPERTIES ADDITIONAL_CLEAN_FILES ${OUTPUT_DIR})

  # Automatically set up a dependency for the target whose flags we used on the generated target. The consumer can
  # just use the generated sources.
  add_dependencies(${TARGET} ${TARGET}_PROTOGEN)
endmacro()
