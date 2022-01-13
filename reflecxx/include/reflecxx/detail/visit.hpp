#pragma once

#include <array>
#include <tuple>
#include <type_traits>

#include "../types.hpp"

namespace reflecxx {

// Backport from c++20
template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T, typename V>
constexpr void visit(V&& visitor);

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

namespace detail {

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

template <typename T, typename V>
constexpr auto visitAccummulate(V&& visitor);

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
template <size_t I = 0, typename... Tp, typename V, typename... Ts>
constexpr auto forEach(const std::tuple<Tp...>& t, V&& visitor, std::tuple<Ts...> prevResults) {
    const auto results = std::tuple_cat(std::move(prevResults), std::make_tuple(visitor(std::get<I>(t))));
    if constexpr (I + 1 != sizeof...(Tp)) {
        return forEach<I + 1>(t, visitor, std::move(results));
    } else {
        return results;
    }
}
// Handle the empty tuple case separately for organization.
template <typename V, typename... Ts>
constexpr auto forEach(const std::tuple<>&, V&&, std::tuple<Ts...> prevResults) {
    return prevResults;
}

}

#endif // REFLECXX_GENERATION

} // namespace reflecxx
