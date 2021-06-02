#pragma once

namespace proto {
namespace detail {
template <typename T>
struct always_false : std::false_type {};

// Type visitors require partially specialized tempalte structs
template <typename T, typename Visitor>
struct Acceptor {
    static_assert(always_false<T>::value, "Must specialize Acceptor for each type T");
    // Interface to implement in specializations
    static constexpr void visit([[maybe_unused]] Visitor&& visitor);
};

} // namespace detail
} // namespace proto

namespace proto {
struct base_tag {};

template <typename T>
struct type_tag : base_tag {
    using type = T;
};

template <typename T>
struct TupleType {
    using type = std::tuple<>;
};

// Default
template <typename T>
struct is_proto_visitable : std::false_type {};
template <typename T>
inline constexpr bool is_proto_visitable_v = is_proto_visitable<T>::value;

// wrapper to support template type deduction for type visitors (ie no instance argument passed to visit())
template <typename T, typename Visitor>
constexpr void visit(Visitor&& visitor, bool visitBaseClasses = true) {
    detail::Acceptor<T, Visitor>::visitd(std::forward<Visitor>(visitor), visitBaseClasses);
}

} // namespace proto
