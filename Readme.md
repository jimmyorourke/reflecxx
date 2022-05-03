```
           __ _
 _ __ ___ / _| | ___  _____  ____  __
| '__/ _ \ |_| |/ _ \/ __\ \/ /\ \/ /
| | |  __/  _| |  __/ (__ >  <  >  <
|_|  \___|_| |_|\___|\___/_/\_\/_/\_\
```

A static reflection framework for C++.

## Features

* libclang driven code generation of reflection meta objects
    * Class memory layout is maintained
    * No intrusive macros or repeated declarations of class members
* Support for inheritance, including multiple and multi-level
* Enum support, including
    * To/from string and to/from index
    * Enum size
    * Enumerator list and name list generation
* Selective reflection, using `REFLECXX_T` annotation to denote which types should be reflected
* Namespace scoping handled
* Compiler flags respected during code generation
* Extensible helper library featuring:
    * Foreach-style iteration over class/enum members
    * Tuple-style access to class/enum members
    * Iteration over object instances or types
    * Simultaneous iteration over multiple instances
    * Automatically implemented comparison operators
    * Automatic [nlohmann JSON](https://github.com/nlohmann/json) serializaton/deserialization (opt-in dependency)
    * Largely constexpr for compile-time meta programming
    * Type traits for metaprogramming and partial template specializations
    * Straightforward syntax for visitor pattern visitors
* CMake integration

## Examples

```cpp
//** MyTypes.hpp
#include <reflecxx/attributes.hpp>

enum class MyEnum {Item1 = 1, Item2 = 2, Item3 = 3} REFLECXX_T;

struct Parent {
    int a;
    double b;
} REFLECXX_T;

struct Child : Parent {
    bool c;
} REFLECXX_T;

#include REFLECXX_HEADER(MyTypes.hpp)

---------------------------------------
//** main.cpp

#include <reflecxx/reflecxx.hpp>
#include <reflecxx/json_visitor.hpp>

#include <MyTypes.hpp>

int main() {
    static_assert(reflecxx::getName<MyEnum>() == "MyEnum");
    static_assert(reflecxx::enumSize<MyEnum>() == 3);
    static_assert(reflecxx::enumName(MyEnum::Item2) == "Item2");
    static_assert(reflecxx::fromName<MyEnum>("Item2") == MyEnum::Item2);
    static_assert(reflecxx::enumContains<MyEnum>(1));
    static_assert(!reflecxx::enumContains<MyEnum>(0));

    for (const auto& e : reflecxx::enumerators<MyEnum>()) {
        std::cout << reflecxx::enumName(e) << ": " << static_cast<int>(e) << "\n";
    }
    for (const auto& name : reflecxx::enumNames<MyEnum>()) {
        std::cout << name << ": " << static_cast<int>(reflecxx::fromName<MyEnum>(name)) << "\n";
    }


    static_assert(reflecxx::is_reflecxx_visitable_v<Child>);
    static_assert(reflecxx::getName<Child>() == "Child");
    static_assert(reflecxx::getName<0, Child>() == "c");
    static_assert(reflecxx::fieldCount<Child>() == 3);
    static_assert(reflecxx::getVisitableTypes<Child>() ==
                  std::make_tuple(reflecxx::type_tag<bool>{}, reflecxx::type_tag<int>{}, reflecxx::type_tag<double>{}));
    static_assert(reflecxx::getBases<Child>() == std::make_tuple(reflecxx::type_tag<Parent>{}));
    static_assert(std::is_same_v<reflecxx::typeAt<0, Child>, bool>)

    Parent p_inst = {1, 1.5};
    Parent p_inst2 = {1, 2.6};

    double& b_ref = reflecxx::get<1>(p_inst);
    b_ref += 1.1;
    assert(p_inst.b == 2.6);
    assert(reflecxx::equalTo(p_inst, p_inst2));

    // to/from json just work
    nlohmann::json j = p_inst;
    p_inst2 = j;

    // Custom visitors

    auto naive_print = [](std::string_view member_name, const auto& value) {
        std::cout << member_name << ": " << value << "\n";
    };
    reflecxx::forEachField(p_inst, std::move(naive_print));

    struct TypeCounter {
        constexpr void operator()(std::string_view, const reflecxx::base_tag&) { otherTypes++; }
        constexpr void operator()(std::string_view, const reflecxx::type_tag<int>& tag) {
            static_assert(std::is_same_v<typename std::remove_reference_t<decltype(tag)>::type, int>);
            ints++;
        }

        int otherTypes{};
        int ints{};
    };
    TypeCounter counter;
    reflecxx::forEachField<Parent>(counter);
    assert(counter.ints == 1 && counter.otherTypes == 1);
}

```

For more examples see the [tests](test/).

## Architecture

The `reflecxx` library's reflection utilities are centered around the [visitor pattern](https://en.wikipedia.org/wiki/Visitor_pattern).

The libclang driven code generation tooling generates meta-object definitions for each structure being reflected, including aspects such as the name of the structure, tuples of struct members, tuples of base classes, etc. Examples of the generated definitions from the tests can be found [here](/test/generated).

The `reflecxx` library utilities are largely built upon the core foreach-style iteration over struct members implemented by the `forEachField` acceptor function (also aliased with the name `visit`) which accepts a visitor to apply.

```cpp
template <typename T, typename Visitor>
constexpr void forEachField(T&& toVisit, Visitor&& visitor);
```

or fields of a type without an instance:

```cpp
template <typename T, typename Visitor>
constexpr void forEachField(Visitor&& visitor);
```

The visitor is a functor or lambda with the following call operator signature:

```cpp
template <typename T>
void operator()(std::string_view field_name, T& value);

```

or

```cpp
template <typename T>
void operator()(std::string_view field_name, const T& value);

```

For type visitors without an instance,

```cpp
template <typename T>
void operator()(std::string_view field_name, const reflecxx::type_tag<T>& tag)
```

where `reflecxx::type_tag<T>` is a [simple tag struct](reflecxx/include/reflecxx/types.hpp#L13).

The `applyForEach` variants take this a step further, taking multiple instances as input and performing simultaneous iteration. This is used to implement such functionality as automatic comparison operators.

Multi-level and multiple inheritance are supported where members of base classes will also be visited.

See the [API headers](reflecxx/include/reflecxx) for more.

## Dependencies

* libclang
* Python 3.6+
* C++17 compiler
* CMake

For the tests: Conan

## Build and integration instructions

The tests will build automatically when the `reflecxx` is the top level CMake project, or when `-DREFLECXX_BUILD_TESTS=1` is part of the CMake generate command.
The tests require Conan to install gtest and nlohmann json.

```
mkdir build && cd build
conan install ../test
cmake -G <your favourite generator> ..
cmake --build .
ctest -V
```

You may need to follow the steps in the next section regarding locating libclang and Python.

### Integration With Your Project

In the file containing structures to be reflected, annotate the definition (or full class declaration) with the `REFLECXX_T` macro from `reflecxx/attributes.hpp`. At the bottom of the file, to include the generated code automatically (post-generation), use the `REFLECXX_HEADER` include helper, passing it the file name.

E.g. For a file named `reflect_me.hpp`:

```cpp
#include <reflecxx/attributes.hpp>

class MyClassToReflect {
  ...
} REFLECXX_T;


#include REFLECXX_HEADER(reflect_me.hpp) // The name of this current file must be reflect_me.hpp to match.

```

The `reflecxx_generate` CMake function will initiate the code generation. It takes a list of source files containing annotated structure declarations, as well as a base target (typically the target making use of the aforementioned sources) whose compilation willbe used.  You will typically also be linking this target against the `reflecxx` library target.

E.g.

```
target_link_libraries(my_target
  PUBLIC
    reflecxx
)
set(REFLECXX_HEADERS
  reflect_me.hpp # File containing annotated full structure declarations to be reflected.
)
reflecxx_generate("${REFLECXX_HEADERS}" my_target)
```

For more examples see the [build for the tests](test/CMakeLists.txt).

[ReflecxxGen.cmake](ReflecxxGen.cmake) needs to know the location of `libclang.<so|dyld|dll>`. It tries some reasonable guesses, but if they don't match your system configuration, you can specify the location by setting CMake variable `REFLECXX_LIBCLANG_DIR`.
The same is true of the Python 3 interpreter. By default it's assumed that it is in the Path, but if that's not the case, or you want to use a different invocation (say if you use pipenv, or pyenv, or CMake's FindPython), you can set CMake variable `REFLECXX_PYTHON_CMD`.

## Comparison With Similar Projects

Most reflection libraries fall under 2 major categories: macro based or libclang based.

Macro based libraries are either verbose and repetitive, requiring redefinition of structs, or intrusive, altering the struct's memory layout and obfuscating the code. The build process is typically much simpler and more intuitive than libclang based projects, however understanding what reflection code is being generated is often much more difficult with preprocessor macros.

The libclang based tools and libraries require a separate pass through the clang frontend to generate the reflection code, in whatever form it may take. In several comparable projects this isn't integrated with a build system, and generated files need to be manually added for the final compilation. Compiler flags and include directories are often not handled during code generation. Many of the tools use template files for the code generation, which adds additional tooling dependencies, and requires learning the template language to add or modify reflection features or capabilities.

`reflecxx` aims to minimize the pain points common in libclang based projects, though at present it depends on Python. Rather than reflecting all structures by default, `reflecxx` uses an opt-in method where structures must be denoted as `reflecxx` types, by annotating the declaration with the `REFLECXX_T` macro. The `reflecxx` generated code aims to be generic structure metadata, and not tied to a specific usecase; e.g. serialization/deserialization code is not autogenerated, rather meta object code (in standard C++) is generated such that a serialization/deserialization function could be written to handle any `reflecxx` meta object. The [nlohmann json visitor](reflecxx/include/reflecxx/json_visitor.hpp) demonstrates this. This design reduces dependencies and decouples use cases from internal code generation implementation details, and allows new use cases to be added without requiring new code to be generated.

## Note about libclang bindings

The [clang_reference folder](generator/clang_reference) contains a patched `cindex.py` libclang bindings file, used by the code generator. This file is patched with cursor definitions missing from the official bindings. Since this local version is used, there are no additional dependencied on a clang Python package.
