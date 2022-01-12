#pragma once

#include <reflecxx/reflecxx_base.hpp>

#include <cassert>

namespace reflecxx {
namespace detail {

// Tag struct where the const-ness of type T matches that of S.
// An alternative implementation could use std::conditional.
template <typename T, typename S, bool = std::is_const_v<S>>
struct match_const;

template <typename T, typename S>
struct match_const<T, S, false> {
    using type = std::remove_const_t<T>;
};
template <typename T, typename S>
struct match_const<T, S, true> {
    using type = const std::remove_const_t<T>;
};
template <typename T, typename S>
using match_const_t = typename match_const<T, S>::type;

} // namespace detail

template <typename T>
constexpr auto getVisitableTypes() {
    // build a tuple of type tags representing the visitable types
    constexpr auto v = [](const char*, const auto& tag) constexpr { return tag; };
    return visitAccummulate<T>(std::move(v));
}

template <size_t I, typename T>
constexpr auto getType() {
    auto t = getVisitableTypes<T>();
    return std::get<I>(t);
}
template <size_t I, typename T>
using typeAt = typename decltype(getType<I, T>())::type;

template <size_t I, typename T>
constexpr auto& get(T& obj) {
    // This gives a more obvious error than when typeAt fails to compile
    static_assert(I < fieldCount<T>(), "Index out of range!");

    // The const-ness of the pointer to member must match the const-ness of T
    detail::match_const_t<typeAt<I, remove_cvref_t<T>>, T>* ptr = nullptr;
    auto count = 0;
    auto v = [&count, &ptr ](const char*, auto& member) constexpr {
        if constexpr (std::is_same_v<remove_cvref_t<decltype(*ptr)>, remove_cvref_t<decltype(member)>>) {
            if (count == I)
                ptr = &member;
        }
        count++;
    };
    visit(obj, std::move(v));
    assert(ptr);
    return *ptr;
}

template <typename T>
constexpr size_t fieldCount() {
    size_t count = 0;
    constexpr auto v = [&count](const char*, const auto&) constexpr { count++; };
    visit<T>(std::move(v));
    return count;
}

template <size_t I, typename T>
constexpr const char* getName() {
    static_assert(I < fieldCount<T>(), "Index out of range!");

    size_t count = 0;
    const char* out = nullptr;
    auto v = [&count, &out ](const char* name, const auto&) constexpr {
        if (count == I) {
            out = name;
        }
        count++;
    };
    visit<T>(std::move(v));
    assert(out != nullptr);
    return out;
}

// The variadic template args need to be last or type deduction doesn't work properly.
template <size_t I, typename Visitor, typename T, typename... Ts, typename>
constexpr void applyForEach(Visitor&& v, T&& t1, Ts&&... ts) {
    v(getName<I, T>(), get<I>(t1), get<I>(ts)...);
    if constexpr (I + 1 < fieldCount<T>()) {
        applyForEach<I + 1>(std::forward<Visitor>(v), std::forward<T>(t1), std::forward<T>(ts)...);
    }
}

template <typename T, typename O>
constexpr bool compare(const T& t1, const T& t2, const O& op) {
    bool res = true;
    auto v = [&res, &op](const char* n, const auto& val1, const auto& val2) {
        static_assert(std::is_same_v<decltype(val1), decltype(val2)>);

        if constexpr (std::is_array_v < remove_cvref_t<decltype(val1)>) {
            // c-style array
            const auto size1 = sizeof(val1) / sizeof(val1[0]);
            const auto size2 = sizeof(val2) / sizeof(val2[0]);

            res = (res && op(size1, size2));
            if (!res) {
                // break early
                return;
            }
            for (auto i = 0u; i < size1; ++i) {
                // Recurse to handle whatever type the array members are
                res = (res && compare(val1[i], val2[i], op));
                if (!res) {
                    // break early
                    return;
                }
            }
        } else {
            res = (res && op(val1, val2));
        }
    };

    applyForEach(std::move(v), t1, t2);
    return res;
}

} // namespace reflecxx
