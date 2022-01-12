#pragma once

#include <array>
#include <functional>

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

    auto tied() const { return std::tie(i, d, bs, basicsStdarr); }

    bool operator==(const NestingStruct& rhs) const {
        std::array a1{basicsArr[0], basicsArr[1], basicsArr[2]};
        std::array a2{rhs.basicsArr[0], rhs.basicsArr[1], rhs.basicsArr[2]};
        return this->tied() == rhs.tied() && a1 == a2;
        // return REFLECXX_CMP(*this, rhs, std::equal_to<>{});
    }
} REFLECXX_T;

} // namespace test_types

#include REFLECXX_HEADER(structs_reflecxx_generated.hpp)
