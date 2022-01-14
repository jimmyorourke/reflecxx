// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

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

    bool operator==(const NestingStruct& rhs) const { return REFLECXX_CMP(*this, rhs, std::equal_to<>{}); }
} REFLECXX_T;

} // namespace test_types

#include REFLECXX_HEADER(structs_reflecxx_generated.hpp)
