#pragma once

//#include <codegen/visitor.hpp>

#include <array>
#include <list>
#include <set>
#include <stdarg.h>
#include <stdbool.h>
#include <string>
#include <vector>
//#include <features.h>

#ifdef PROTO_GENERATION
#define VISIT __attribute__((annotate("my annotation")))
#else
#define VISIT
#endif

//#define VISIT

enum Unscoped { First = 2, Second, Third, Fourth } VISIT;
enum class Scoped { First, Second, Third } VISIT;

struct Empty {
} VISIT;

struct Basics {
    bool b;
    int i;
    double d;

    auto tied() const { return std::tie(b, i, d); }
    bool operator==(const Basics& rhs) const { return tied() == rhs.tied(); }
} VISIT;

struct Wrapper {
    int i;
    double d;
    Basics b;
    Basics basicsArr[3];
    std::array<Basics, 2> basicsStdarr;

    auto tied() const {
        // Nasty laziness hack to be able to compare C-style arrays. This is only used for equality checks in the test.s
        std::array<Basics, 3> arr;
        std::copy(std::begin(basicsArr), std::end(basicsArr), std::begin(arr));
        return std::tie(i, d, b, arr, basicsStdarr);
    }
    bool operator==(const Wrapper& rhs) const { return tied() == rhs.tied(); }
} VISIT;

class BasicClass {
 public:
    bool b;
    int i;
    double d;
} VISIT;

class ChildClass : public BasicClass {
 public:
    int publicField{};
    int avoidUnreachable() { return privateField; }

 protected:
    int protectedField{};

 private:
    int privateField{};
} VISIT;

class UnreflectedBaseClass {
 public:
    int baseField{};
};

class ChildOfUnreflectedBaseClass : public UnreflectedBaseClass {
 public:
    int chieldField{};
} VISIT;

// Set of functions to initialize the test types.
// Default values could be defined in the structs/classes, but we do this separately instead to properly represent
// non-default instantiations, ensuring the test do not depend on the code generation relying on the default values in
// any way.

inline Basics defaultBasics() { return {true, -3, 5.5}; }

inline Wrapper defaultWrapper() {
    return {2,
            5.1,
            defaultBasics(),
            {defaultBasics(), defaultBasics(), defaultBasics()},
            {defaultBasics(), defaultBasics()}};
}

inline BasicClass defaultBasicClass() { return {false, -4, -10.1}; }
