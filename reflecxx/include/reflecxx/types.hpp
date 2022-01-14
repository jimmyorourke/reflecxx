// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

#include <reflecxx/detail/types.hpp>

#include <string_view>

namespace reflecxx {

// Tag structs for passing types as objects.
// Type visitors take as parameters the name of the field and a specialized type_tag corresponding to
// the field's type. The base_tag can be used with template functors for general cases to avoid the compiler generating
// template instantiations for each field type when not necessary.
struct base_tag {};
template <typename T>
struct type_tag : base_tag {
    using type = T;
};
template <typename T>
using type_tag_t = typename type_tag<T>::type;

// Type trait in dicating whether a type has reflecxx metadata and is reflecxx visitable.
// Visitable if there is a specialization defined for the type T. We checkd the Internal types to avoid getting tripped
// up by the static_assert in the base template. This actually allows this type trait to be used as the check in the
// static_assert! See the comment in detail/types.hpp for a detailed explanation of the Internal type machinery.
template <typename T>
struct is_reflecxx_visitable
: std::negation<std::conjunction<std::is_base_of<detail::Unspecialized, detail::MetaStructInternal<T>>,
                                 std::is_base_of<detail::Unspecialized, detail::MetaEnumInternal<T>>>> {};

// Typically a user should never have to directly make use of these types, but we expose them to allow custom extension.

// Base templates. Must be specialized.
template <typename T>
struct MetaStruct : detail::MetaStructInternal<T> {
    static_assert(is_reflecxx_visitable<T>::value, "MetaStructInternal must be specialized!");
    // Expected interface:
    // static constexpr std::string_view name{"T"};
    // static constexpr auto publicFields = std::make_tuple(/*std::tuple of ClassMembers*/);
    // static constexpr auto baseClasses = std::make_tuple(/*std::tuple of type_tag*/);
};
template <typename T>
struct MetaEnum : detail::MetaEnumInternal<T> {
    static_assert(is_reflecxx_visitable<T>::value, "MetaEnumInternal must be specialized!");
    // Expected interface:
    // static constexpr std::string_view name{"T"};
    // static constexpr std::array<Enumerator<test_types::Unscoped>, Size> enumerators;
};

template <typename Enum>
struct Enumerator {
    Enum enumerator;
    std::string_view name;
    std::underlying_type_t<Enum> value;
};

template <typename Class, typename MemberType>
struct ClassMember {
    using type = MemberType;
    MemberType Class::*ptr;
    std::string_view name;
};

} // namespace reflecxx
