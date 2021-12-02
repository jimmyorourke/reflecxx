#pragma once

#include "detail/proto_detail.hpp"

namespace proto {

// Wrapper to support template type deduction for type visitors (ie no instance argument passed to visit()).
// The specialized Acceptor struct will be autogenerated.
template <typename T, typename Visitor>
constexpr void visit(Visitor&& visitor) {
    detail::Acceptor<T, Visitor>::visitType(std::forward<Visitor>(visitor));
}

// Tag structs to be able to pass types around as objects.
// if you want to write a visitor to operate on members of a specific type, the base tag avoids the compiler generating
// template instantiations for all types
struct base_tag {};
template <typename T>
struct type_tag : base_tag {
    using type = T;
};
template <typename T>
using type_tag_t = typename type_tag<T>::type;

// Aliases for type and instance versions of visit function.
template <typename T, typename Visitor>
constexpr void forEachField(Visitor&& visitor) {
    visit<T>(std::forward<Visitor>(visitor));
}
template <typename T, typename Visitor>
constexpr void forEachField(T&& toVisit, Visitor&& visitor) {
    // The visit function overload will be autogenerated.
    visit(std::forward<T>(toVisit), std::forward<Visitor>(visitor));
}

// Type trait to identify visitable types (for which a visit function has been autogenerated).
template <typename T, typename SFINAE>
struct is_proto_visitable;
template <typename T>
inline constexpr bool is_proto_visitable_v = is_proto_visitable<T>::value;

} // namespace proto
