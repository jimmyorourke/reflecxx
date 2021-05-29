#pragma once

#include <proto/proto_base.hpp>

namespace proto {
namespace detail {
// Using an anonymous lambda doesn't work as the compiler won't allow the assignment from the generic lambda parameter
template<typename T>
struct Extractor {
    constexpr Extractor(T** t, int target) : _t{t}, _target{target} {}

    // The !std::is_const_v<S> is not strictly required but helps avoid subtle bugs where the caller uses a const object
    // resulting in this template resolving as S = const T or const T&, resulting in this function being called instead
    // of the specialization on our target type T, without the compiler telling us.
    template<typename S, typename=std::enable_if_t<!std::is_const_v<S>>>
    constexpr void operator()(const char* name, S& member) {
        _count++;
    }

    template<>
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

}

// TODO: base classes?
template<size_t i, typename T>
using type_at = typename std::tuple_element<i, typename TupleType<T>::type>::type;

template<size_t i, typename T>
constexpr auto& get(T& obj) {
    type_at<i, T>* memberPtr = nullptr;
    detail::Extractor e{&memberPtr, i};
    visit(obj, std::move(e));
    // this should be impossible
    assert(memberPtr);
    return *memberPtr;
}

template<typename T>
constexpr size_t fieldCount(bool visitBaseClasses=true) {
    size_t count = 0;
    auto v = [&count](const char*, const auto&) constexpr {
        count++;
    };
    visit<T>(std::move(v), visitBaseClasses);
    return count;
}

template<size_t i, typename T>
constexpr const char* getName(bool visitBaseClasses=true) {
    size_t count = 0;
    const char * out = nullptr;
    auto v = [&count, &out](const char* name, const auto&) constexpr {
        if (count == i) {
            out = name;
        }
        count++;
    };
    visit<T>(std::move(v), visitBaseClasses);

    // We can't static_assert on fieldCount because we don't necessarily know the value for visitBaseClasses at compile time
    if (i >= count) {
        // will cause compilation error in constexpr context
        throw std::runtime_error{"Index out of range!"};
    }
    return out;
}



}
