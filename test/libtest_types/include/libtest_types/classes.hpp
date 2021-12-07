#pragma once

#include <reflecxx/attributes.hpp>

// test types in their own namespace to ensure names get qualified properly!
namespace test_types {

class BasicClass {
 public:
    bool b;
    int i;
    double d;
} VISIT; // REFLECXX_GENERATE;

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
    int childField{};
} VISIT;

} // namespace test_types

#include REFLECXX_HEADER(classes_reflecxx_generated.hpp)
