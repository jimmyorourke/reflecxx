#pragma once

#ifdef PROTO_GENERATION
// Use VISIT after a struct/class or enum declaration to generate a visitor for it.
#define VISIT __attribute__((annotate("PROTO_GEN: Reflection Visitor")))

#define CODEGENENERATED_INCLUDE(include_file) <proto/detail/empty.hpp>

#else
// Outside of code generation, do nothing.
#define VISIT

#define CODEGENENERATED_INCLUDE(include_file) include_file

#endif
