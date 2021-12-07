#pragma once

#include <array>
#include <tuple>

#include <reflecxx/attributes.hpp>
#include <reflecxx/struct_visitor.hpp>

// test types in their own namespace to ensure names get qualified properly!
namespace test_types {

struct BasicStruct {
// Demonstrate that the generated code respects compile flags and definitions
#ifdef BASIC_STRUCT_HAS_B
    bool b;
#endif
    int i;
    double d;

    bool operator==(const BasicStruct& rhs) const;
} REFLECXX_T;

struct NestingStruct {
    int i;
    double d;
    BasicStruct bs;
    BasicStruct basicsArr[3];
    std::array<BasicStruct, 2> basicsStdarr;

    auto tied() const {
        // Nasty laziness hack to be able to compare C-style arrays. This is only used for equality checks in the test.s
        std::array<BasicStruct, 3> arr;
        std::copy(std::begin(basicsArr), std::end(basicsArr), std::begin(arr));
        return std::tie(i, d, bs, arr, basicsStdarr);
    }
    PROTO_EQL_OP(NestingStruct);
    // bool operator==(const NestingStruct& rhs) const { return tied() == rhs.tied(); }
} REFLECXX_T;

} // namespace test_types

#include REFLECXX_HEADER(structs_reflecxx_generated.hpp)
