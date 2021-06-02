#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

enum class MyEnum { val1, val2, val3 };

enum class MyEnum2 { val1, val2 };

// template <typename Visitor>
// constexpr void visit(const MyEnum&, Visitor& visitor) {
//   visitor(MyEnum::val1, "val1", 0);
//   visitor(MyEnum::val2, "val2", 2);
//   visitor(MyEnum::val3, "val3", 3);
// }

// template <typename Visitor>
// constexpr void visit(const MyEnum2&, Visitor& visitor) {
//   visitor(MyEnum2::val1, "val1", 0);
//   visitor(MyEnum2::val2, "val2", 2);
// }

template <typename SFINAE_ENUM_t, typename Visitor>
struct Acceptor {};

template <typename Visitor>
struct Acceptor<MyEnum, Visitor> {
    static constexpr void visit(Visitor&& visitor) {
        visitor(MyEnum::val1, "val1", 0);
        visitor(MyEnum::val2, "val2", 2);
        visitor(MyEnum::val3, "val3", 3);
    }
};

template <typename Visitor>
struct Acceptor<MyEnum2, Visitor> {
    static constexpr void visit(Visitor&& visitor) {
        visitor(MyEnum2::val1, "val1", 0);
        visitor(MyEnum2::val2, "val2", 2);
    }
};

template <typename EnumType, typename VisitorType>
constexpr void visit(VisitorType&& visitor) {
    Acceptor<EnumType, VisitorType>::visit(std::forward<VisitorType>(visitor));
}

template <typename EnumType>
constexpr int enumCount2() {
    auto count = 0;
    auto l = [&count](const EnumType& e, const char* name, std::underlying_type_t<EnumType> val) { count++; };
    visit<EnumType>(l);
    return count;
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
// C++17 specific stuff here
#endif

template <typename EnumType>
struct CountFunctor {
    int& _capturedCount;
    constexpr CountFunctor(int& capturedCount)
    : _capturedCount{capturedCount} {}

    constexpr void operator()(const EnumType& e, const char* name, std::underlying_type_t<EnumType> val) {
        _capturedCount++;
    }
};

template <typename EnumType>
constexpr int enumCountFunctor2() {
    int count = 0;
    CountFunctor<EnumType> f{count};
    visit<EnumType>(f);
    return count;
}

struct base {};
template <typename T>
struct tag : base {
    using type = T;
};

constexpr auto run() {
    int count = 0;
    return tag<decltype(count)>{};
}

template <typename Visitor, typename Predicate>
constexpr auto visitr(Visitor&& visitor, Predicate&& p) {
    if (p("a", tag<int>{})) {
        return visitor("a", tag<int>{});
    }
    if (p("b", tag<double>{})) {
        return visitor("b", tag<double>{});
    }
}

template <typename Visitor>
constexpr decltype(auto) visitr2(Visitor&& visitor) {
    bool b = false;
    {
        auto r = visitor("a", tag<int>{}, b);
        if (b) {
            return r;
        }
    }
    {
        // auto r = visitor("b", tag<double>{}, b);
        // if (b) {
        //     return r;
        // }
    }
}

constexpr auto ru2n() {
    base t;
    auto l = [&](const char* name, const auto& tagv) constexpr {
        typename std::decay_t<decltype(tagv)>::type u = 4;
        // b=true;
        return tagv;
    };
    auto p = [&](const char* name, const auto& tagv) constexpr {
        typename std::decay_t<decltype(tagv)>::type u = 4;
        return true;
    };
    visitr(l, p);
    // visitr2(l);
    // typename decltype(t)::type u = 4;
}

std::tuple<int, double, char> t;

constexpr int f(int n) {
    if (n <= 0)
        throw std::runtime_error("");
    return 1;
}
int f2(int n) {
    if (n <= 0)
        throw std::runtime_error("");
    return 1;
}

int main() {
    char a1[f(1)];
    std::cout << rand() % 10 << "\n";
    //   static_assert(enumCount2<MyEnum>() == 3);
    //   std::cout << enumCount2<MyEnum>() << "\n";
    //   static_assert(enumCount2<MyEnum2>() == 2);
    //   std::cout << enumCount2<MyEnum2>() << "\n";
    //   static_assert(enumCountFunctor2<MyEnum2>() == 2);
    //   std::cout << enumCountFunctor2<MyEnum2>() << "\n";
}
