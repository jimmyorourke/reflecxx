#pragma once

#include <proto/proto_base.hpp>

//get<i>(s);
//get_name<i, S>();
//field_count<S>();
//visit_struct::type_at<i, S>
//isvisitable trait!
namespace proto {

// return tag type
template<size_t i, typename T>
constexpr auto get_type_tag() {
    return type_tag<std::tuple_element<i, TupleType<T>::type>::type>{};
}
template<size_t i, typename T>
using type_at = typename std::tuple_element<i, typename TupleType<T>::type>::type;

template<size_t i, typename T>
auto& get(const T& obj) {
    // ideally this would be a reference
    type_at<i, T>* tptr = nullptr;

    size_t count = 0;
    auto v = [&count, &tptr](const char*, const auto& member) constexpr {
        if (count == i) {
            tptr = &member;
        }
        count++;
    };
    visit(obj, std::move(v));
    return *tptr;
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
