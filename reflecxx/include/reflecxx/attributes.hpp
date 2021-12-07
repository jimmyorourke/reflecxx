#pragma once

#ifdef PROTO_GENERATION
// Use VISIT after a struct/class or enum declaration to generate a visitor for it.
#define VISIT __attribute__((annotate("PROTO_GEN: Reflection Visitor")))

#define REFLECXX_HEADER(include_file) <reflecxx/detail/empty.hpp>

#define PROTO_EQL(rhs) false;

#else
// Outside of code generation, do nothing.
#define VISIT

#define REFLECXX_HEADER(include_file) <generated_headers/include_file>

#define PROTO_EQL(rhs) reflecxx::equalTo(*this, rhs);

#endif

#define PROTO_EQL_OP(type) \
    bool operator==(const type& rhs) const { return PROTO_EQL(rhs); }

// #define PROTO_EQL1(rhs) \
//     #ifndef PROTO_GENERATION \
//         return reflecxx::eql1(*this, rhs); \
//     #else \
//         return false; \
//     #endif

// bool operator==(const BasicStruct& rhs) const;
//     // {
//     //     //return tied() == rhs.tied();
//     //     #ifndef PROTO_GENERATION
//     //     return reflecxx::eql1(*this, rhs);
//     //     #else
//     //     return false;
//     //     #endif

//     // }
// } VISIT;

// RCXX_GENERATE
// RX_REFLECT
// REFLECXX
// REFLECXX_T

// is_reflecxx_type
// namespace reflecxx {

// }
