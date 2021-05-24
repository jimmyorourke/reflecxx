#pragma once

#include <codegen/visitor.hpp>

#include <array>
#include <list>
#include <set>
#include <string>
#include <vector>

enum Uscoped{First, Second, Third};
enum class Scoped{Scoped::First, Scoped::Second, Scoped::Third};
// underlying types, offsets

struct Empty {
};

struct Basics {
    bool b;
    int i;
    unsigned int u;
    double d;
    float f;
    char c;
};

struct Containers {
    int icarr[3];
    std::array<double, 5> dstdarr;
    std::set<float> fset;
    std::vector<int> ivec;
    std::string s;
};

struct Wrapper {
    int i;
    double d;
    Basics b;
    Containers c;
    Basics basicsArr[3];
    std::array<Containers, 2> containersStdarr;
};


class BasicClass {
public:
    bool b;
    int i;
    double d;
};

class ChildClass : public BasicClass {
public:
    int publicField;
protected:
    int protectedField;
private:
    int privateField;
};

class UnreflectedBaseClass {
public:
    int baseField;
};

class ChildOfUnreflectedBaseClass : public UnreflectedBaseClass {
public:
    int chieldField;
};

// Set of functions to initialize the test types.
// Default values could be defined in the structs/classes, but we do this separately instead to properly represent
// non-default instantiations, ensuring the test do not depend on the code generation relying on the default values in
// any way.

inline Basics defaultBasics()
{
    return {true, -3, 4, 5.5, 0.4f, 'h'};
}

inline Containers defaultContainers()
{
    return {{3, 2, 1}, {1.0, 1.5, 2.0, 2.5, 3.0}, {1.f, -0.4f, 0.3f}, {1, 2, 3}, "hello world"};
}

struct Wrapper {
    int i;
    double d;
    Basics b;
    Containers c;
    Basics basicsArr[3];
    std::array<Containers, 2> containersStdarr;
}

inline Wrapper defaulWrapper()
{
    return {2, 5.1, defaultBasics(), defaultContainers(), {defaultBasics(), defaultBasics(), defaultBasics()}, {defaultContainers(), defaultContainers()}};
}

inline BasicClass defaultBasicClass()
{
    return {false, -4, -10.1};
}
