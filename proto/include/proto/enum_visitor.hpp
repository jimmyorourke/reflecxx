#pragma once

#include <proto/proto_base.hpp>

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef PROTO_GENERATION

namespace proto {

// Returns the number of enumerators in an enum.
template <typename EnumType>
constexpr size_t enumSize() {
    size_t count = 0;
    auto v = [&count](const EnumType& e, const char* name, std::underlying_type_t<EnumType>) constexpr { count++; };
    visit<EnumType>(std::move(v));
    return count;
}

// Converts the name of the enumerator as string.
template <typename EnumType>
constexpr const char* enumName(EnumType target) {
    const char* targetName = nullptr;
    auto v = [&targetName,
              target ](const EnumType& enumerator, const char* name, std::underlying_type_t<EnumType>) constexpr {
        if (enumerator == target) {
            targetName = name;
        }
    };
    visit<EnumType>(std::move(v));
    return targetName;
}

// Converts a name to a matching enumerator.
template <typename EnumType>
constexpr EnumType fromName(const char* targetName) {
    EnumType e{};
    bool found = false;
    auto v = [&e, targetName, &
              found ](const EnumType& enumerator, const char* name, std::underlying_type_t<EnumType>) constexpr {
        if (name == targetName) {
            e = enumerator;
            found = true;
        }
    };
    visit<EnumType>(std::move(v));
    if (!found) {
        // will cause compilation error in constexpr context
        throw std::runtime_error{"No enumerator for name " + std::string{targetName}};
    }
    return e;
}

// Returns an array containing the names of all enumerators.
template <typename EnumType>
constexpr auto enumNames() {
    std::array<const char*, enumSize<EnumType>()> names{};
    auto count = 0;
    auto v = [&names, &count ](const EnumType&, const char* name, std::underlying_type_t<EnumType>) constexpr {
        names[count] = name;
        count++;
    };
    visit<EnumType>(std::move(v));
    return names;
}

// Returns an array of all enumerators.
template <typename EnumType>
constexpr auto enumerators() {
    std::array<EnumType, enumSize<EnumType>()> arr{};
    auto count = 0;
    auto v = [&arr, &count ](const EnumType& e, const char*, std::underlying_type_t<EnumType>) constexpr {
        arr[count] = e;
        count++;
    };
    visit<EnumType>(std::move(v));
    return arr;
}

// Returns true if there is an enumerator corresponding to the underlying type value provided.
// Can be used to determine if a static_cast to the enum type is safe.
template <typename EnumType>
constexpr bool enumContains(std::underlying_type_t<EnumType> targetValue) {
    bool contains = false;
    auto v = [&contains, targetValue ](const EnumType&, const char*, std::underlying_type_t<EnumType> value) constexpr {
        if (value == targetValue) {
            contains = true;
        }
    };
    visit<EnumType>(std::move(v));
    return contains;
}

} // namespace proto

#endif // PROTO_GENERATION
