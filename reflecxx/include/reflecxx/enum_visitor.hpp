#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

#include <array>
#include <cstddef>
#include <type_traits>

namespace reflecxx {

// Returns the number of enumerators in an enum.
template <typename EnumType>
constexpr size_t enumSize();

// Returns the name of the enumerator as string.
template <typename EnumType>
constexpr std::string_view enumName(EnumType enumerator);

// Converts a name to a matching enumerator.
template <typename EnumType>
constexpr EnumType fromName(std::string_view enumeratorName);

// Returns an array containing the names of all enumerators.
template <typename EnumType>
constexpr auto enumNames() -> std::array<std::string_view, enumSize<EnumType>()>;

// Returns an array of all enumerators.
template <typename EnumType>
constexpr auto enumerators() -> std::array<EnumType, enumSize<EnumType>()>;

// Returns true if there is an enumerator corresponding to the underlying type value provided.
// Can be used to determine if a static_cast to the enum type is safe.
template <typename EnumType>
constexpr bool enumContains(std::underlying_type_t<EnumType> targetValue);

} // namespace reflecxx

#include "impl/enum_visitor_impl.hpp"

#endif // REFLECXX_GENERATION
