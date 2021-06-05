add_library(proto INTERFACE)
target_include_directories(proto INTERFACE include)

target_compile_definitions(proto INTERFACE
HELLO)
