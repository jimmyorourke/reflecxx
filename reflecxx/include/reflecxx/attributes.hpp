// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

#ifdef REFLECXX_GENERATION
// During code generation

// Annotate a type with this macro to generate Reflecxx visitor acceptors.
#define REFLECXX_T __attribute__((annotate("REFLECXX_GEN: Reflection Visitor")))
// Include a null header during code generation.
#define REFLECXX_HEADER(include_file) <reflecxx/detail/empty.hpp>
// Null implementation of helper wrapper for defining comparison operators in header files that go through code
// generation.
#define REFLECXX_CMP(lhs, rhs, op) \
    {}

#else
// Outside of code generation

// Do nothing.
#define REFLECXX_T
// Include the generated file containing the Reflecxx acceptors.
#define REFLECXX_HEADER(include_file) <generated_headers/include_file>
// Helper wrapper for defining comparison operators in header files that go through code generation.
#define REFLECXX_CMP(lhs, rhs, op) reflecxx::compare(lhs, rhs, op)

#endif // REFLECXX_GENERATION
