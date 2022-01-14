// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace reflecxx {

// Returns a tuple of type_tags representing the types of the visitable fields of T.
template <typename T>
constexpr auto getVisitableTypes();

// Returns a type_tag representing the type of the I'th visitable fields of T.
template <size_t I, typename T>
constexpr auto getType();
// Declares a type alias to the type of the i'th visitable field of T.
template <size_t I, typename T>
using typeAt = typename decltype(getType<I, T>())::type;

// Returns a reference to the I'th field in an instance of T.
template <size_t I, typename T>
constexpr auto& get(T& obj);

// Returns the number of fields in T.
template <typename T>
constexpr size_t fieldCount();

// Returns the name of the type T. The returned view never expires.
template <typename T>
constexpr std::string_view getName();

// Returns the name of the I'th field of T. The returned view never expires.
template <size_t I, typename T>
constexpr std::string_view getName();

// Apply visitor to each field of each T
// The variadic template args need to be last or type deduction doesn't work properly.
template <size_t I = 0, typename Visitor, typename T, typename... Ts,
          typename = std::enable_if_t<(std::is_same_v<T, Ts> && ...)>>
constexpr void applyForEach(Visitor&& v, T&& t1, Ts&&... ts);

// Helper to allow the visitor to be the last argument when there are only 2 Ts, such as for binary comparisons.
template <typename T, typename Visitor>
constexpr void forEachApply(T&& t1, T&& t2, Visitor&& visitor) {
    applyForEach(std::forward<Visitor>(visitor), std::forward<T>(t1), std::forward<T>(t2));
}

// Returns the result of applying Operation to each field of lhs and rhs, pairwise, AND'ing the results.
template <typename T, typename Operation>
constexpr bool compare(const T& lhs, const T& rhs, const Operation& op);

// Returns true if each field of lhs is equal to each field of rhs.
template <typename T>
constexpr bool equalTo(const T& lhs, const T& rhs) {
    return compare(lhs, rhs, std::equal_to<>{});
}

template <typename T>
constexpr bool lessThan(const T& lhs, const T& rhs) {
    // Caution! We can't just use std::less, in case just a single field is less and the rest are equal.
    return compare(lhs, rhs, std::less_equal<>{}) && !equalTo(lhs, rhs);
}

template <typename T>
constexpr bool greaterThan(const T& lhs, const T& rhs) {
    // Caution! We can't just use std::greater in case just a single field is greater and the rest are equal.
    return compare(lhs, rhs, std::greater_equal<>{}) && !equalTo(lhs, rhs);
}

} // namespace reflecxx

#include "impl/struct_visitor_impl.hpp"

#endif // REFLECXX_GENERATION
