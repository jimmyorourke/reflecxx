#pragma once

#include <tuple>

namespace proto {
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

// Tag structs to be able to pass types around as objects.
struct BaseTag {};
template <typename T>
struct TagType : BaseTag {
    using type = T;
};

// Tag struct containing a tuple type composed of the types of all public (ie visitable) members of T.
template <typename T>
struct TupleType {
    using type = std::tuple<>;
};
template <typename T>
using TupleTypeT = typename TupleType<T>::type;

template <typename T, typename SFINAE = void>
struct IsProtoVisitable : std::false_type {};
template <typename T>
struct IsProtoVisitable<T, std::void_t<decltype(detail::Acceptor<T, int>::visitType(std::declval<int>()))>>
: std::true_type {};

} // namespace proto
