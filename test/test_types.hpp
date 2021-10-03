#pragma once

#include <array>
#include <tuple>

#include <proto/attributes.hpp>



// test types in their own namespace to ensure names get qualified properly
namespace test_types {

enum Unscoped { First = 2, Second, Third, Fourth } VISIT;
enum class Scoped { First, Second, Third } VISIT;

struct BasicStruct {
    bool b;
    int i;
    double d;

    auto tied() const { return std::tie(b, i, d); }
    bool operator==(const BasicStruct& rhs) const { return tied() == rhs.tied(); }
} VISIT;

struct NestingStruct {
    int i;
    double d;
    BasicStruct b;
    BasicStruct basicsArr[3];
    std::array<BasicStruct, 2> basicsStdarr;

    auto tied() const {
        // Nasty laziness hack to be able to compare C-style arrays. This is only used for equality checks in the test.s
        std::array<BasicStruct, 3> arr;
        std::copy(std::begin(basicsArr), std::end(basicsArr), std::begin(arr));
        return std::tie(i, d, b, arr, basicsStdarr);
    }
    bool operator==(const NestingStruct& rhs) const { return tied() == rhs.tied(); }
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

} // namespace test_types

#include CODEGENENERATED_INCLUDE(<generated_headers/test_types_proto_generated.hpp>)
