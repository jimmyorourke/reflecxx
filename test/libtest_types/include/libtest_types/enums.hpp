// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

#include <reflecxx/attributes.hpp>

// test types in their own namespace to ensure names get qualified properly!
namespace test_types {

enum Unscoped { First = 2, Second, Third, Fourth } REFLECXX_T;
enum class Scoped { First, Second, Third } REFLECXX_T;

} // namespace test_types

#include REFLECXX_HEADER(enums_reflecxx_generated.hpp)
