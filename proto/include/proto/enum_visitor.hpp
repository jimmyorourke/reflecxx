#pragma once

#include <proto/proto_base.hpp>

namespace proto {

template <typename EnumType>
constexpr int enumSize() {
    auto count = 0;
    auto v = [&count](const EnumType& e, const char* name, std::underlying_type_t<EnumType>) constexpr {
        count++;
    };
    visit<EnumType>(std::move(v));
    return count;
}

template <typename EnumType>
constexpr const char* toString(EnumType target) {
    const char* targetName = nullptr;
    auto v = [&targetName, target](const EnumType& enumerator, const char* name, std::underlying_type_t<EnumType>) constexpr {
        if (enumerator == target) {
            targetName = name;
        }
    };
    visit<EnumType>(std::move(v));
    return targetName;
}

template <typename EnumType>
constexpr EnumType fromString(const char* targetName) {
    EnumType e{};
    bool found = false;
    auto v = [&e, targetName, &found](const EnumType& enumerator, const char* name, std::underlying_type_t<EnumType>) constexpr {
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

template <typename EnumType>
constexpr auto enumNames() {
    std::array<const char*, enumSize<EnumType>()> names{};
    auto count = 0;
    auto v = [&names, &count](const EnumType&, const char* name, std::underlying_type_t<EnumType>) constexpr {
        names[count] = name;
        count++;
    };
    visit<EnumType>(std::move(v));
    return names;
}

template <typename EnumType>
constexpr auto enumerators() {
    std::array<EnumType, enumSize<EnumType>()> arr{};
    auto count = 0;
    auto v = [&arr, &count](const EnumType& e, const char*, std::underlying_type_t<EnumType>) constexpr {
        arr[count] = e;
        count++;
    };
    visit<EnumType>(std::move(v));
    return arr;
}

// Contains an enumerator with matching underlying type value
// Tells you if static_cast is safe
template <typename EnumType>
constexpr bool enumContains(std::underlying_type_t<EnumType> targetValue) {
    bool contains = false;
    auto v = [&contains, targetValue](const EnumType&, const char*, std::underlying_type_t<EnumType> value) constexpr {
        if (value == targetValue) {
            contains = true;
        }
    };
    visit<EnumType>(std::move(v));
    return contains;
}

}
