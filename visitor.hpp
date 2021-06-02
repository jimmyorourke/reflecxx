#pragma once

#ifdef CODEGEN

#define GENERATE_VISITOR __attribute__((annotate("generate: visitor")))
#else
///
/// Use GENERATE_VISITOR after a struct/class to generate a visitor for it
#define GENERATE_VISITOR

#endif // CODEGEN
