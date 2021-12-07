#pragma once

#include <reflecxx/attributes.hpp>

// test types in their own namespace to ensure names get qualified properly!
namespace test_types {

class BasicClass {
 public:
    bool b;
    int i;
    double d;
} REFLECXX_T;

class ChildClass : public BasicClass {
 public:
    int publicField{};
    int avoidUnreachable() { return privateField; }

 protected:
    int protectedField{};

 private:
    int privateField{};
} REFLECXX_T;

class UnreflectedBaseClass {
 public:
    int baseField{};
};

class ChildOfUnreflectedBaseClass : public UnreflectedBaseClass {
 public:
    int childField{};
} REFLECXX_T;

} // namespace test_types

#include REFLECXX_HEADER(classes_reflecxx_generated.hpp)
