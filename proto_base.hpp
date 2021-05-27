#pragma once

namespace proto {
namespace detail {
template <typename T>
struct always_false : std::false_type {};

} // namespace detail

struct base_tag {};

template<typename T>
struct type_tag : base_tag {
    using type = T;
};

template <typename T, typename Visitor>
struct Acceptor {
    static_assert(detail::always_false<T>::value, "Must specialize Acceptor for each type T");
    // Interface to implement in specializations
    static constexpr void visit([[maybe_unused]] Visitor&& visitor);
};

// wrapper to support template type deduction for the visitor
template <typename T, typename Visitor>
constexpr void visit(Visitor&& visitor) {
    Acceptor<T, Visitor>::visit(std::forward<Visitor>(visitor));
}

} // namespace proto
