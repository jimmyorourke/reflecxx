```
               __ _
     _ __ ___ / _| | ___  _____  ____  __
    | '__/ _ \ |_| |/ _ \/ __\ \/ /\ \/ /
    | | |  __/  _| |  __/ (__ >  <  >  <
    |_|  \___|_| |_|\___|\___/_/\_\/_/\_\
```
A static reflection framework for C++.

## Features

* libclang driven code generation
* Class memory layout is maintained
* No intrusive macros
* No repeated declarations of class members
* Support for inheritance, including multiple inheritance and multi-level inheritance
* Compiler flags respected during code generation
* No additional template file language and tooling dependencies
* CMake integration, no additional build steps
* Enum support
* Selective reflection, using `REFLECXX_T` annotation to denote which types should be reflected
* Library of refle
* Extensible in C++
* Foreach
* Visit multiple instances at once
* implementation of comparison operators
* Visit instances or types
* Straightforward syntax for visitors
* constexpr for compile-time meta programming
* type traits for metaprogramming and partial template specializations
* automatic nlohmann json serializaton/deserialization

## Dependencies
* libclang
* Python 3.6+
* C++17 compiler
* CMake

For the tests: Conan

## Build and integration instructions

The tests will build automatically when the `-DREFLECXX_BUILD_TESTS=1` as part of the CMake generate command.
The tests require Conan to install gtest and nlohmann json.
```
mkdir build && cd build
conan install ../test
cmake -G <your favourite generator> ..
cmake --build .
ctest -V
```

Integration

target_link_libraries(my_target
  PUBLIC
    reflecxx
)
set(REFLECXX_HEADERS
  reflect_me.hpp
)
reflecxx_generate("${REFLECXX_HEADERS}" my_target)

#include REFLECXX_HEADER(reflect_me.hpp)


ReflecxxGen.cmake needs to know the location of libclang.<so|dyld|dll>. It tries some reasonable guesses, but if they don't match your system configuration, you can specify the location by setting CMake variable `REFLECXX_LIBCLANG_DIR`.
The same is true of the Python 3 interpreter. By default it's assumed that it is in the Path, but if that's not the case, or you want to use a different invocation (say if you use pipenv, or pyenv, or CMake's FindPython), you can set CMake variable `REFLECXX_PYTHON_CMD`.

Comparison
Most reflection libraries fall under 2 major categories: macro based and libclang based.

Macro based libraries are either verbose and repetitive, requiring redefinition of structs, or intrusive, altering the struct's memory layout and obfuscating the code.

The libclang based tools and libraries require a separate pass through the clang frontend to generate the reflection code, in whatever form it may take. In many cases this isn't integrated with a build system, and generated files need to be manually added for the final compilation. Compiler flags and include directories are often not handled during code generation. Many of the tools use template files for the code generation, which adds additional tooling dependencies, and requires learning the template language to add or modify reflection features or capabilities.

framework for static reflection/introspection
user design reflection implementation - reduces dependies, decouples use cases for flexibility. Allows the meta structures to geow seperately from the uses
no runtime type information needed
compilation database support - no need to specify flags, include dirs, all manually
no template files or languages needed
can be run in a single pass as part of the build - no need to run generation script first, add generated includes, then compile
hard to integrate into build systems
python or cppast depending on depencies

no dependencies on boost, etc

tell what's being reflected, don't reflect all by deafult
Marking Primitives for Reflection
By default, running clscan on your files will output no reflection information. Any primitives that you want to be reflected need to be marked in your source code

nned to create empty files to satisfy build system
not normal workflow

compared to non libclang:
unobtrusive
