#pragma once

#include <proto/proto_base.hpp>

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef PROTO_GENERATION

namespace proto {
namespace detail {
// Functor that can extract a field of a specific type and index from an instance of T.
// Using a generic lambda doesn't work as the compiler won't allow the assignment to the capture arg
template <typename T>
struct Extractor {
    constexpr Extractor(T** t, int target)
    : _t{t}
    , _target{target} {}

    // The !std::is_const_v<S> is not strictly required but helps avoid subtle bugs where the caller uses a const object
    // resulting in this template resolving as S = const T or const T&, resulting in this function being called instead
    // of the specialization on our target type T, without the compiler telling us.
    template <typename S>//, typename = std::enable_if_t<!std::is_const_v<S>>>
    constexpr void operator()(const char* name, S& member) {
        _count++;
    }

    template <>
    constexpr void operator()<T>(const char* name, T& member) {
        if (_count == _target) {
            *_t = &member;
        }
        _count++;
    }

    T** _t;
    int _target{};
    int _count{};
};

} // namespace detail

// Declares an alias to the type of the i'th visitable field of T.
template <size_t i, typename T>
using typeAt = typename std::tuple_element<i, typename TupleType<T>::type>::type;

// Returns a reference to the i'th field in an instance of T.
template <size_t i, typename T>
constexpr auto& get(T& obj) {
    using rawT = std::decay_t<T>;
    // This gives a more obvious error than when typeAt fails to compile
    static_assert(i < fieldCount<rawT>(), "Index out of range!");
    typeAt<i, rawT>* memberPtr = nullptr;

    detail::Extractor e{&memberPtr, i};
    visit(obj, std::move(e));
    // this should be impossible
    assert(memberPtr);
    return *memberPtr;
}
// Any way to not need this const overload and be able to declare membPtr properly?
template <size_t i, typename T>
constexpr const auto& get(const T& obj) {
    // This gives a more obvious error than when typeAt fails to compile
    static_assert(i < fieldCount<T>(), "Index out of range!");
    const typeAt<i, T>* memberPtr = nullptr;

    detail::Extractor e{&memberPtr, i};
    visit(obj, std::move(e));
    // this should be impossible
    assert(memberPtr);
    return *memberPtr;
}

// Returns the number of fields in T.
template <typename T>
constexpr size_t fieldCount() {
    // alternatively could use std::tuple_size.
    size_t count = 0;
    auto v = [&count](const char*, const auto&) constexpr { count++; };
    visit<T>(std::move(v));
    return count;
}

// Returns the name of the i'th field of T.
template <size_t i, typename T>
constexpr const char* getName() {
    static_assert(i < fieldCount<T>(), "Index out of range!");

    size_t count = 0;
    const char* out = nullptr;
    auto v = [&count, &out ](const char* name, const auto&) constexpr {
        if (count == i) {
            out = name;
        }
        count++;
    };
    visit<T>(std::move(v));

    // should not be possible
    assert(out != nullptr);
    return out;
}
// Returns the name of the i'th field of T.
// template <typename T>
// constexpr auto eql(const T& t1) {
//     typename TupleType<T>::type tup1;

//     size_t count = 0;
//     const char* out = nullptr;
//     auto v = [&count, &tup1 ](const char*, const auto& val) constexpr {
//         std::get<count>(tup1) = val;

//     };
//     visit<T>(std::move(v));

//     return tup1;
// }

template<size_t I = 0, typename T, typename... Ts, typename F, typename=std::enable_if_t<(std::is_same_v<T, Ts> && ...)>>
void for_each_apply( F&& f, T&& t1, Ts &&... ts) {
    f(get<I>(t1), get<I>(ts)...);
    // if constexpr makes recursive templates so much easier!
    if constexpr(I+1 < fieldCount<std::decay_t<T>>()) {
        for_each_apply<I+1>(std::forward<F>(f), std::forward<T>(t1), std::forward<T>(ts)...);
    }
}

template <typename T>
constexpr auto eql1(const T& t1, const T& t2) {

    // size_t count = 0;
    bool eq = true;
    auto v = [&eq](const auto& val1, const auto& val2) {
    //auto v = [&eq](auto... param) {
        //static_assert(std::is_same_v<decltype(val1), decltype(val2)>);
        std::cout << val1 << " " << val2 << "\n";
        eq = (eq && (val1 == val2));
    };
    for_each_apply(std::move(v), t1, t2);
   // auto eq =false;
    //get<0>(t1);
    return eq;
}

} // namespace proto

#endif // PROTO_GENERATION
