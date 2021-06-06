#pragma once

#include<tuple>

namespace proto {
namespace detail {
// Type visitors require partially specialized template structs
template <typename T, typename Visitor>
struct Acceptor;
// Must specialize Acceptor for each type T
// Interface to implement in specializations:
// static constexpr void visit([[maybe_unused]] Visitor&& visitor);
// Note: we don't define the above with a static assertion so that we can test existence to define type traits.

} // namespace detail


struct BaseTag {};
template <typename T>
struct TagType : BaseTag {
    using type = T;
};

template <typename T>
struct TupleType {
    using type = std::tuple<>;
};

// wrapper to support template type deduction for type visitors (ie no instance argument passed to visit())
template <typename T, typename Visitor>
constexpr void visit(Visitor&& visitor) {
    detail::Acceptor<T, Visitor>::visitd(std::forward<Visitor>(visitor));
}

// Aliases for type and instance versions of visit function.
template <typename T, typename Visitor>
constexpr void forEachField(Visitor&& visitor) {
    visit<T>(std::forward<Visitor>(visitor));
}
template <typename ...Args>
constexpr void forEachField(Args&&... args) {
    visit(std::forward<Args>(args)...);
}

// Determined by the existance of a type visitor.
template <typename T, typename SFINAE=void>
struct IsProtoVisitable : std::false_type {};
template<typename T>
// Interestingly, we can't just test on the existance of the wrapper visit function like std::void_t<decltype(visit<T, int>(int()))>>. Always valid for some reason.
struct IsProtoVisitable<T, std::void_t<decltype(detail::Acceptor<T, int>::visitd(std::declval<int>()))>> : std::true_type {};
template <typename T>
inline constexpr bool IsProtoVisitableV = IsProtoVisitable<T>::value;

} // namespace proto
