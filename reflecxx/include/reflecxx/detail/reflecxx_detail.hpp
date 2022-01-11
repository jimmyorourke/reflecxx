#pragma once

#include <tuple>
#include <type_traits>

namespace reflecxx {

// Backport from c++20
template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T>
struct type_tag;


template <typename Enum>
struct Enumerator {
    Enum enumerator;
    const char* name;
    std::underlying_type_t<Enum> value;
};

template <class... T>
constexpr bool always_false = false;

template <typename T>
struct MetaEnum;//{
    //static_assert(always_false<T>, "must use correct specialization");
//};

template <typename Class, typename MemberType>
struct ClassMember {
    using type = MemberType;
    MemberType Class::* ptr;
    const char* name;
};

template <typename T>
struct MetaStruct;//{
//     static_assert(always_false<T>, "must use correct specialization");
// };

namespace detail {
// Type visitors require partially specialized template structs
template <typename T, typename Visitor>
struct Acceptor;
// The code generator must specialize Acceptor for each type T.
// Interface to implement in specializations:
// static constexpr void visitType([[maybe_unused]] Visitor&& visitor);
// Note: we don't define the above with a static assertion enforcing specialization so that we can test existence to
// define type traits.



template<typename T, typename V>
struct MemberVisitor {
    template<typename M>
    constexpr auto operator()(const ClassMember<remove_cvref_t<T>, M>& member) const {
        return visitor(member.name, instance.*member.ptr);
    }

    T& instance;
    V& visitor;
};

template<typename T, typename V>
struct BaseClassMemberVisitor {
    template<typename B>
    constexpr void operator()(type_tag<B>){
        // fully recurse to handle multiple levels of inheritance
        visit(static_cast<B&>(instance), visitor);
    }

    T& instance;
    V& visitor;
};

template<typename T, typename V>
struct BaseClassMemberChainVisitor {
    template<typename B>
    constexpr auto operator()(type_tag<B>){
        // fully recurse to handle multiple levels of inheritance
        return visitAccum(static_cast<B&>(instance), visitor);
    }

    T& instance;
    V& visitor;
};

template<typename V>
struct MemberTypeVisitor {
    // constexpr MemberTypeVisitor(V& v): visitor{v}{}
    // template<typename T, typename M>
    // constexpr void operator()(const ClassMember<T, M>& member) {
    //     visitor(member.name, type_tag<M>{});
    // }
    template<typename T, typename M>
    constexpr auto operator()(const ClassMember<T, M>& member){
        return visitor(member.name, type_tag<M>{});
    }

    V& visitor;
};

template<typename V>
struct BaseClassMemberTypeVisitor {
    template<typename B>
    constexpr void operator()(type_tag<B>){
        // fully recurse to handle multiple levels of inheritance
        visit<B>(visitor);
    }

    V& visitor;
};

template<typename V>
struct BaseClassMemberTypeChainVisitor {
    template<typename B>
    constexpr auto operator()(type_tag<B>){
        return visitAccum<B>(visitor);
    }

    V& visitor;
};

template<typename... Ts, typename V>
constexpr void tupleVisit(const std::tuple<Ts...>& t, V&& visitor)
{
    std::apply([&visitor](const auto&... tupleElems){(visitor(tupleElems), ...);}, t);
}

// template<size_t I = 0, typename... Tp, typename V, typename... Ts>
// auto chainvisit(std::tuple<Tp...>& t, V&& visitor, std::tuple<Tp...>& b) {
//     auto bnext = visitor(b, std::get<I>(t));
//     // do things
//     if constexpr(I+1 != sizeof...(Tp)) {
//         return chainvisit<I+1>(t, visitor, bnext);
//     } else {
//         return bnext;
//     }
// }

} // namespace detail

template<size_t I = 0,  typename... Tp, typename V, typename... Ts>
constexpr auto chainvisit(const std::tuple<Tp...>& t, V&& visitor, const std::tuple<Ts...>& b) {
    if constexpr(sizeof...(Tp) == 0) {
        return b;
    }
    // required else!
    else {
        auto bnext = std::tuple_cat(b, std::make_tuple(visitor(std::get<I>(t))));
        // do things
        if constexpr(I+1 != sizeof...(Tp)) {
            return chainvisit<I+1>(t, visitor, bnext);
        } else {
            return bnext;
        }
    }
}

template<typename T, typename V>
constexpr auto visitAccum(V&& visitor) {
    using namespace detail;
    // clean t in case callers mess up
    auto r1 = chainvisit(MetaStruct<remove_cvref_t<T>>::publicFields, MemberTypeVisitor<V>{visitor}, std::tuple<>{});
    //return r1;
    return chainvisit(MetaStruct<remove_cvref_t<T>>::baseClasses, BaseClassMemberTypeChainVisitor<V>{visitor}, r1);
}

template<typename T, typename V>
constexpr auto visitAccum(T&& instance, V&& visitor) {
    using namespace detail;
    auto r1 = chainvisit(MetaStruct<remove_cvref_t<T>>::publicFields, MemberVisitor<T, V>{instance, visitor}, std::tuple<>{});
    return chainvisit(MetaStruct<remove_cvref_t<T>>::baseClasses, BaseClassMemberChainVisitor<T, V>{instance, visitor}, r1);
}



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
struct is_reflecxx_visitable<T, std::void_t<decltype(MetaStruct<T>::publicFields)>>
: std::true_type {};

} // namespace reflecxx
