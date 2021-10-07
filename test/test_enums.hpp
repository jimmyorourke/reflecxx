#pragma once

#include <proto/attributes.hpp>

// test types in their own namespace to ensure names get qualified properly!
namespace test_types {

enum Unscoped { First = 2, Second, Third, Fourth } VISIT;
enum class Scoped { First, Second, Third } VISIT;

} // namespace test_types

#include CODEGENENERATED_INCLUDE(<generated_headers/test_enums_proto_generated.hpp>)
