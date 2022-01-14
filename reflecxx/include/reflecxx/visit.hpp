// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

#include <reflecxx/detail/visit.hpp>
#include <reflecxx/types.hpp>

namespace reflecxx {

template <typename T, typename V>
constexpr void visit(T&& instance, V&& visitor) {
    // wrap visitor in something that binds member pointers
    using CleanT = detail::remove_cvref_t<T>;
    detail::forEach(MetaStruct<CleanT>::publicFields, detail::MemberVisitor<T, V>{instance, visitor});
    detail::forEach(MetaStruct<CleanT>::baseClasses, detail::BaseClassMemberVisitor<T, V>{instance, visitor});
}

template <typename T, typename V>
constexpr void visit(V&& visitor) {
    using CleanT = detail::remove_cvref_t<T>;
    detail::forEach(MetaStruct<CleanT>::publicFields, detail::MemberTypeVisitor<V>{visitor});
    detail::forEach(MetaStruct<CleanT>::baseClasses, detail::BaseClassMemberTypeVisitor<V>{visitor});
}

template <typename T, typename V>
constexpr auto visitAccummulate(T&& instance, V&& visitor) {
    using CleanT = detail::remove_cvref_t<T>;
    const auto results = detail::forEach(MetaStruct<CleanT>::publicFields,
                                         detail::MemberVisitor<T, V>{instance, visitor}, std::tuple<>{});
    return detail::forEach(MetaStruct<CleanT>::baseClasses,
                           detail::BaseClassMemberChainVisitor<T, V>{instance, visitor}, std::move(results));
}

template <typename T, typename V>
constexpr auto visitAccummulate(V&& visitor) {
    using CleanT = detail::remove_cvref_t<T>;
    const auto results =
        detail::forEach(MetaStruct<CleanT>::publicFields, detail::MemberTypeVisitor<V>{visitor}, std::tuple<>{});
    return forEach(MetaStruct<CleanT>::baseClasses, detail::BaseClassMemberTypeChainVisitor<V>{visitor},
                   std::move(results));
}

// Aliases for the visit functions.
template <typename T, typename Visitor>
constexpr void forEachField(Visitor&& visitor) {
    visit<T>(std::forward<Visitor>(visitor));
}
template <typename T, typename Visitor>
constexpr void forEachField(T&& toVisit, Visitor&& visitor) {
    visit(std::forward<T>(toVisit), std::forward<Visitor>(visitor));
}
template <typename T, typename Visitor>
constexpr auto forEachFieldAcccumulate(Visitor&& visitor) {
    return visitAccummulate<T>(std::forward<Visitor>(visitor));
}
template <typename T, typename Visitor>
constexpr auto forEachFieldAcccumulate(T&& toVisit, Visitor&& visitor) {
    return visitAccummulate(std::forward<T>(toVisit), std::forward<Visitor>(visitor));
}

// Type trait to identify Reflecxx visitable types (for which a visit function has been autogenerated).
template <typename T>
struct is_reflecxx_visitable;
template <typename T>
inline constexpr bool is_reflecxx_visitable_v = is_reflecxx_visitable<T>::value;

} // namespace reflecxx

#endif // REFLECXX_GENERATION
