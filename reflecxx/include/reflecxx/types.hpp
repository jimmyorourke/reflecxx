#pragma once

#include "detail/types.hpp"

namespace reflecxx {

// Tag structs for passing types as objects.
// Type visitors are expected to take as parameters the name of the field and a specialized type_tag corresponding to
// the field's type. This allows the use of inheritance such that the base_tag can be used for general cases to avoid
// the compiler generating template instantiations for each field type when not necessary.
struct base_tag {};
template <typename T>
struct type_tag : base_tag {
    using type = T;
};
template <typename T>
using type_tag_t = typename type_tag<T>::type;


// visitable if there is a specialization defined. We checkd the Internal types to avoid getting tripped up by the
// static_assert. This actually allows this type trait to be used as the check in the static_assert!
template <typename T>
struct is_reflecxx_visitable : std::negation<std::conjunction<std::is_base_of<detail::Unspecialized, detail::MetaStructInternal<T>>,
                                                              std::is_base_of<detail::Unspecialized, detail::MetaEnumInternal<T>>>> {};


// Typically a user should never have to directly make use of these types, but we expose them to allow custom extension.

template <typename T>
struct MetaStruct : detail::MetaStructInternal<T> {
    static_assert(is_reflecxx_visitable<T>::value, "MetaStructInternal must be specialized!");
};
template <typename T>
struct MetaEnum : detail::MetaEnumInternal<T> {
    static_assert(is_reflecxx_visitable<T>::value, "MetaEnumInternal must be specialized!");
};

template <typename Enum>
struct Enumerator {
    Enum enumerator;
    const char* name;
    std::underlying_type_t<Enum> value;
};

template <typename Class, typename MemberType>
struct ClassMember {
    using type = MemberType;
    MemberType Class::*ptr;
    const char* name;
};

}
