#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef PROTO_GENERATION

namespace proto {

// Returns the number of enumerators in an enum.
template <typename EnumType>
constexpr size_t enumSize();

// Returns the name of the enumerator as string.
template <typename EnumType>
constexpr const char* enumName(EnumType enumerator);

// Converts a name to a matching enumerator.
template <typename EnumType>
constexpr EnumType fromName(const char* enumeratorName);

// Returns an array containing the names of all enumerators.
template <typename EnumType>
constexpr auto enumNames();

// Returns an array of all enumerators.
template <typename EnumType>
constexpr auto enumerators();

// Returns true if there is an enumerator corresponding to the underlying type value provided.
// Can be used to determine if a static_cast to the enum type is safe.
template <typename EnumType>
constexpr bool enumContains(std::underlying_type_t<EnumType> targetValue);

} // namespace proto

#include "impl/enum_visitor_impl.hpp"

#endif // PROTO_GENERATION
