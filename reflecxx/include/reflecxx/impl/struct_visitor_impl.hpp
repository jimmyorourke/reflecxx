#pragma once

#include <reflecxx/reflecxx_base.hpp>

#include <cassert>

namespace reflecxx {
namespace detail {

// Functor that can extract a pointer to the field at a given index in an instance of a struct.
// The type of the field, T, can be determined with the typeAt<> helper.
// Using a generic lambda doesn't work as the compiler won't allow the assignment to the capture arg.
template <typename T>
struct Extractor {
    constexpr Extractor(T** t, int target)
    : _t{t}
    , _target{target} {}

    template <typename S>
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

// Tag struct where the const-ness of type T matches that of S.
template <typename T, typename S, bool = std::is_const_v<S>>
struct ConstMatch;

template <typename T, typename S>
struct ConstMatch<T, S, false> {
    using type = std::remove_const_t<T>;
};

template <typename T, typename S>
struct ConstMatch<T, S, true> {
    using type = const std::remove_const_t<T>;
};

template <typename T, typename S>
using ConstMatchT = typename ConstMatch<T, S>::type;

} // namespace detail

// Returns a reference to the i'th field in an instance of T.
template <size_t i, typename T>
constexpr auto& get(T& obj) {
    using rawT =  reflecxx::remove_cvref_t<T>;
    // This gives a more obvious error than when typeAt fails to compile
    static_assert(i < fieldCount<rawT>(), "Index out of range!");

    // The const-ness of the pointer to member must match the const-ness of T to avoid segfaults and other runtime
    // issues! The compiler doesn't catch this!
    detail::ConstMatchT<typeAt<i, rawT>, T>* memberPtr = nullptr;
    detail::Extractor e{&memberPtr, i};
    visit(obj, std::move(e));
    // this should be impossible
    assert(memberPtr);
    return *memberPtr;
}

template <typename T>
constexpr size_t fieldCount() {
    // alternatively could use std::tuple_size, since we also define a tuple for each visitable struct type.
    size_t count = 0;
    auto v = [&count](const char*, const auto&) constexpr { count++; };
    visit<T>(std::move(v));
    return count;
}

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

// The variadic template args need to be last or type deduction doesn't work properly.
template <size_t I = 0, typename F, typename T, typename... Ts, typename>
constexpr void applyForEach(F&& f, T&& t1, Ts&&... ts) {
    using cleanT = reflecxx::remove_cvref_t<T>;//std::remove_reference_t<T>>;//std::decay_t<T>;
    f(getName<I, cleanT>(), get<I>(t1), get<I>(ts)...);
    // if constexpr makes recursive templates so much easier! And no integer sequences.
    if constexpr (I + 1 < fieldCount<cleanT>()) {
        applyForEach<I + 1>(std::forward<F>(f), std::forward<T>(t1), std::forward<T>(ts)...);
    }
}

template <typename T, typename Visitor>
constexpr void forEachApply(T&& t1, T&& t2, Visitor&& visitor) {
    applyForEach(std::forward<Visitor>(visitor), std::forward<T>(t1), std::forward<T>(t2));
}

template <typename T, typename O>
constexpr bool compare(const T& t1, const T& t2, const O& op) {
    //std::cout <<"eql1\n";
    bool res = true;
    auto v = [&res, &op](const char* n, const auto& val1, const auto& val2) {
        static_assert(std::is_same_v<decltype(val1), decltype(val2)>);
        //std::cout << val1 << " " << val2 << "\n";
        using typet = std::remove_cv_t<std::remove_reference_t<decltype(val1)>>;

        if constexpr(std::is_array_v<typet>) {
            //std::cout << n << " is_array_v, size: " <<  std::extent_v<decltype(val1)> << " " << \
            //std::extent_v<typet> << " " << sizeof(val1)/sizeof(val1[0])<< "\n";
            const auto size1 = sizeof(val1)/sizeof(val1[0]);
            const auto size2 = sizeof(val2)/sizeof(val2[0]);

            res = (res && op(size1, size2));
            if (!res) {
                // break early
                return;
            }
            for (auto i =0u; i < std::extent_v<typet>; ++i) {
                // Recurse to handle whatever type the array members are
                res = (res && compare(val1[i], val2[i], op));
                //std::cout << n << " is_array_v " << i << res << "\n";
                if (!res) {
                    // break early
                    return;
                }
            }
        }
        else {
            //std::cout << n << " basecase\n";
            res = (res && op(val1, val2));
        }
       // std::cout << n << " " << res << "\n";
    };
    applyForEach(std::move(v), t1, t2);
    return res;
}

} // namespace reflecxx
