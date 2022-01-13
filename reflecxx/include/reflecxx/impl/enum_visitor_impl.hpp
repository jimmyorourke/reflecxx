#pragma once

#include <reflecxx/visit.hpp>

namespace reflecxx {

// Returns the number of enumerators in an enum.
template <typename EnumType>
constexpr size_t enumSize() {
    return MetaEnum<EnumType>::enumerators.size();
}

// Returns the name of the enumerator as string.
template <typename EnumType>
constexpr std::string_view enumName(EnumType enumerator) {
    for (const auto& e : MetaEnum<EnumType>::enumerators) {
        if (e.enumerator == enumerator) {
            return e.name;
        }
    }
    // should not be possible
    throw std::runtime_error{"Invalid enumerator."};
}

// Converts a name to a matching enumerator.
template <typename EnumType>
constexpr EnumType fromName(std::string_view enumeratorName) {
    for (const auto& e : MetaEnum<EnumType>::enumerators) {
        // Can compare string_view with ==, unlike const char*.
        if (e.name == enumeratorName) {
            return e.enumerator;
        }
    }
    // will cause compilation error in constexpr context
    throw std::runtime_error{"No enumerator for name " + std::string{enumeratorName}};
}

// Returns an array containing the names of all enumerators.
template <typename EnumType>
constexpr auto enumNames() -> std::array<std::string_view, enumSize<EnumType>()> {
    std::array<std::string_view, enumSize<EnumType>()> names{};
    for (auto i = 0u; i < enumSize<EnumType>(); ++i) {
        names[i] = MetaEnum<EnumType>::enumerators[i].name;
    }
    return names;
}

// Returns an array of all enumerators.
template <typename EnumType>
constexpr auto enumerators() -> std::array<EnumType, enumSize<EnumType>()> {
    std::array<EnumType, enumSize<EnumType>()> arr{};
    for (auto i = 0u; i < enumSize<EnumType>(); ++i) {
        arr[i] = MetaEnum<EnumType>::enumerators[i].enumerator;
    }
    return arr;
}

// Returns true if there is an enumerator corresponding to the underlying type value provided.
// Can be used to determine if a static_cast to the enum type is safe.
template <typename EnumType>
constexpr bool enumContains(std::underlying_type_t<EnumType> targetValue) {
    for (auto i = 0u; i < enumSize<EnumType>(); ++i) {
        if (MetaEnum<EnumType>::enumerators[i].value == targetValue) {
            return true;
        }
    }
    return false;
}

} // namespace reflecxx
