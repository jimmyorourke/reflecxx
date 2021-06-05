#pragma once

#ifdef PROTO_GENERATION
// Use VISIT after a struct/class or enum declaration to generate a visitor for it.
#define VISIT __attribute__((annotate("PROTO_GEN: Reflection Visitor")))

#define INCLUDE_PROTOGEN(include_file)

#else
// Outside of code generation, do nothing.
#define VISIT

//#define INCLUDE_PROTOGEN(include_file) \#include include_file

#endif

