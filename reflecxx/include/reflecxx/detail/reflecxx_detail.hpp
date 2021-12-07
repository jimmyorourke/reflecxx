#pragma once

#include <tuple>
#include <type_traits>

namespace reflecxx {
namespace detail {
// Type visitors require partially specialized template structs
template <typename T, typename Visitor>
struct Acceptor;
// Must specialize Acceptor for each type T
// Interface to implement in specializations:
// static constexpr void visitType([[maybe_unused]] Visitor&& visitor);
// Note: we don't define the above with a static assertion enforcing specialization so that we can test existence to
// define type traits.

} // namespace detail

// Backport from c++20
template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

// Tag struct containing a tuple type composed of the types of all public (ie visitable) members of T.
template <typename T>
struct tuple_type {
    using type = std::tuple<>;
};
template <typename T>
using tuple_type_t = typename tuple_type<T>::type;

template <typename T, typename SFINAE = void>
struct is_reflecxx_visitable : std::false_type {};
template <typename T>
struct is_reflecxx_visitable<T, std::void_t<decltype(detail::Acceptor<T, int>::visitType(std::declval<int>()))>>
: std::true_type {};

} // namespace reflecxx
