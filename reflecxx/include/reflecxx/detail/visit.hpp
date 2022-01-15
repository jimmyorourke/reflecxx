// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

#include <reflecxx/types.hpp>

#include <array>
#include <tuple>
#include <type_traits>

namespace reflecxx {

template <typename T, typename V>
constexpr void visit(V&& visitor);
template <typename T, typename V>
constexpr auto visitAccummulate(V&& visitor);

namespace detail {

// When appending to a tuple, if the item being appended is itself a tuple, to match the semantics of the non-tuple case
// most of the time the desired result is a nested tuple, e.g. appending tuple<int, double> to tuple<char, bool> would
// give tuple<char, bool, tuple<int, double>>, similar to how appending a plain int would result in
// tuple<char, bool, int>. However sometimes unnesting is desired, e.g. resulting in tuple<char, bool, int, double>.
template <bool Unnest = false, typename... Ts, typename T>
constexpr auto tupleAppend(std::tuple<Ts...> t, T m) {
    return std::tuple_cat(std::move(t), std::make_tuple(std::move(m)));
}
template <bool Unnest, typename... Ts, typename... Tp, typename = std::enable_if_t<Unnest>>
constexpr auto tupleAppend(std::tuple<Ts...> t1, std::tuple<Tp...> t2) {
    return std::tuple_cat(std::move(t1), std::move(t2));
}

// Apply a visitor to each element of tuple, discarding return values.
template <typename... Ts, typename V>
constexpr void forEach(const std::tuple<Ts...>& t, V&& visitor) {
    std::apply([&visitor](const auto&... tupleElems) { (visitor(tupleElems), ...); }, t);
}

// Apply a visitor to each element of tuple, accumulating return values in a tuple.
// It would have been really nice to implement this as
//     return std::apply([&visitor](const auto&... tupleElems) { return std::make_tuple(visitor(tupleElems)...); }, t);
// however that doesn't guarantee the order of evaluation of the args to std::make_tuple, i.e. that the visitor will
// execute in order of the tuple elements.
// Instead, use a recursive call, chaining the tuple of previous results into the next call.
template <bool Unnest = false, size_t I = 0, typename... Tp, typename V, typename... Ts>
constexpr auto forEach(const std::tuple<Tp...>& t, V&& visitor, std::tuple<Ts...> prevResults) {
    const auto results = tupleAppend<Unnest>(std::move(prevResults), visitor(std::get<I>(t)));
    if constexpr (I + 1 != sizeof...(Tp)) {
        return forEach<Unnest, I + 1>(t, visitor, std::move(results));
    } else {
        return results;
    }
}
// Handle the empty tuple case.
template <bool = false, size_t = 0, typename V, typename... Ts>
constexpr auto forEach(const std::tuple<>&, V&&, std::tuple<Ts...> prevResults) {
    return prevResults;
}
// Wrapper so that empty tuples don't have to be provided for the starting case.
template <bool Unnest = false, typename... Ts, typename V>
constexpr auto forEachAccum(const std::tuple<Ts...>& t, V&& visitor) {
    return forEach<Unnest>(t, std::forward<V>(visitor), std::tuple<>{});
}

// Functor that wraps a visitor to perform binding between an instance and a ClassMember.
template <typename T, typename V>
struct MemberVisitor {
    template <typename M>
    constexpr auto operator()(const ClassMember<remove_cvref_t<T>, M>& member) const {
        return visitor(member.name, instance.*member.ptr);
    }

    T& instance;
    V& visitor;
};

// Functor that wraps a visitor to enable visitation of the members of all base classes of the instance.
template <typename T, typename V>
struct BaseClassMemberVisitor {
    template <typename B>
    constexpr void operator()(type_tag<B>) {
        // Fully recurse to handle multiple levels of inheritance and multiple base classes.
        visit(static_cast<B&>(instance), visitor);
    }

    T& instance;
    V& visitor;
};

// Wrapper for visitors that return values, such that results can be accumulated.
template <typename T, typename V>
struct BaseClassMemberChainVisitor {
    template <typename B>
    constexpr auto operator()(type_tag<B>) {
        // Fully recurse to handle multiple levels of inheritance and multiple base classes.
        return visitAccum(static_cast<B&>(instance), visitor);
    }

    T& instance;
    V& visitor;
};

// Functor that wraps a type visitor to perform ClassMember to expected visitor interface.
template <typename V>
struct MemberTypeVisitor {
    template <typename T, typename M>
    constexpr auto operator()(const ClassMember<T, M>& member) {
        return visitor(member.name, type_tag<M>{});
    }

    V& visitor;
};

// Functor that wraps a type visitor to enable visitation of the members of all base classes of a type.
template <typename V>
struct BaseClassMemberTypeVisitor {
    template <typename B>
    constexpr void operator()(type_tag<B>) {
        // Fully recurse to handle multiple levels of inheritance and multiple base classes.
        visit<B>(visitor);
    }

    V& visitor;
};

// Wrapper for visitors that return values, such that results can be accumulated.
template <typename V>
struct BaseClassMemberTypeChainVisitor {
    template <typename B>
    constexpr auto operator()(type_tag<B>) {
        // Fully recurse to handle multiple levels of inheritance and multiple base classes.
        return visitAccummulate<B>(visitor);
    }

    V& visitor;
};

} // namespace detail
} // namespace reflecxx

#endif // REFLECXX_GENERATION
