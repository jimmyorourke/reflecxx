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
    template <typename S, typename = std::enable_if_t<!std::is_const_v<S>>>
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
    // This gives a more obvious error than when typeAt fails to compile
    static_assert(i < fieldCount<T>(), "Index out of range!");

    typeAt<i, T>* memberPtr = nullptr;
    detail::Extractor e{&memberPtr, i};
    visit(obj, std::move(e));
    // this should be impossible
    assert(memberPtr);
    return *memberPtr;
}

// Returns the number of fields in T.
template <typename T>
constexpr size_t fieldCount() {
    // alternatively could use a tuple function.
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

} // namespace proto

#endif // PROTO_GENERATION
