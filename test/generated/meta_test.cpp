// Type your code here, or load an example.
#include <tuple>
#include <cstddef>
#include <iostream>

namespace test_types {
struct BasicStruct {
    bool b;
    int i;
    double d;

    bool operator==(const BasicStruct& rhs) const;
};
}

template <typename Class, typename MemberType>
struct ClassMember {
    using type = MemberType;
    MemberType Class::* ptr;
    const char* name;
};

template <typename T>
struct MetaStruct;

template <>
struct MetaStruct<test_types::BasicStruct> {
    static constexpr auto publicFields = std::make_tuple(
        ClassMember<test_types::BasicStruct, bool>{&test_types::BasicStruct::b,"b"},
        ClassMember<test_types::BasicStruct, int>{&test_types::BasicStruct::i, "i"},
        ClassMember<test_types::BasicStruct, double>{&test_types::BasicStruct::d, "d"}
    );
    static constexpr auto baseClasses = std::make_tuple();
};

template<typename... Ts, typename V>
void tupleVisit(const std::tuple<Ts...>& t, V&& visitor)
{
    std::apply([&visitor](const auto&... tupleElems){(visitor(tupleElems), ...);}, t);
}

template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<typename T, typename V>
struct MemberVisitor {
    template<typename M>
    constexpr void operator()(const ClassMember<remove_cvref_t<T>, M>& member) const {
        visitor(member.name, instance.*member.ptr);
    }

    T& instance;
    V& visitor;
};

template <typename T>
struct type_tag {
    using type = T;
};

template<typename T, typename V>
struct BaseClassMemberVisitor {
    // BaseClassMemberVisitor(T&& t, V&& v) : instance{t}, visitor{v} {}
    template<typename B>
    constexpr void operator()(type_tag<B>){
        // fully recurse to handle multiple levels of inheritance
        visit(static_cast<B&>(instance), visitor);
        //tuplevisit(MetaStruct<B>::publicFields, MemberVisitor{static_cast<B&>(instance), visitor});
    }

    T& instance;
    V& visitor;
};


template<typename T, typename V>
constexpr void visit(T&& instance, V&& visitor) {
    // wrap visitor in something that binds member pointers
    tupleVisit(MetaStruct<remove_cvref_t<T>>::publicFields, MemberVisitor<T, V>{instance, visitor});
    tupleVisit(MetaStruct<remove_cvref_t<T>>::baseClasses, BaseClassMemberVisitor<T, V>{instance, visitor});
}

template<typename V>
struct MemberTypeVisitor {
    template<typename T, typename M>
    constexpr void operator()(ClassMember<T, M> member) const {
        visitor(member.name, type_tag<M>{});
    }
    template<typename T, typename M, typename... Ts>
    constexpr auto operator()(ClassMember<T, M>, std::tuple<Ts...> t){
        return visitor(t, std::make_tuple(type_tag<M>{}));
    }

    V& visitor;
};

template<typename V>
struct BaseClassMemberTypeVisitor;

template <typename T, typename Visitor, typename... Tp>
constexpr auto chainVisit(Visitor&& visitor, std::tuple<Tp...> t=std::tuple<>{}) {
    auto t1 = chainvisit(MetaStruct<T>::publicFields, MemberTypeVisitor{visitor, t});
    return chainvisit(MetaStruct<T>::baseClasses, BaseClassMemberTypeVisitor{visitor, t});
}

template<typename V>
struct BaseClassMemberTypeVisitor {
    template<typename B>
    constexpr void operator()(type_tag<B>){
        // fully recurse to handle multiple levels of inheritance
        visit<B>(visitor);
    }
    template<typename B, typename... Ts>
    constexpr auto operator()(type_tag<B>, std::tuple<Ts...> t){
        return chainVisit<B>(visitor, t);
    }

    V& visitor;
};

// prints every element of a tuple
template<size_t I = 0, typename... Tp, typename V, typename... Ts>
auto chainvisit(std::tuple<Tp...>& t, V&& visitor, std::tuple<Tp...>& b) {
    auto bnext = visitor(b, std::get<I>(t));
    // do things
    if constexpr(I+1 != sizeof...(Tp)) {
        return chainvisit<I+1>(t, visitor, bnext);
    } else {
        return bnext;
    }
}

template<typename T, typename V>
constexpr void visit(V&& visitor) {
    // wrap visitor in something that binds member pointers
    tupleVisit(MetaStruct<T>::publicFields, MemberTypeVisitor<V>{visitor});
    tupleVisit(MetaStruct<T>::baseClasses, BaseClassMemberTypeVisitor<V>{visitor});
}

template <typename T>
constexpr size_t fieldCount() {
    size_t count = 0;
    auto v = [&count](const char*, const auto&) constexpr { count++; };
    visit<T>(std::move(v));
    return count;
}

template <typename T>
constexpr void print(T&& inst) {
    auto v = [](const char* n, const auto& v) constexpr {
        std::cout << n <<": " << v << "\n"; };
    visit(std::move(inst), std::move(v));
}

template <typename T>
constexpr void flip(T&& inst) {
    auto v = [](const char* n, auto& v) constexpr {
        std::cout << n <<": " << v << "\n";
        v = !v;
        std::cout << n <<": " << v << "\n";
    };
    visit(std::move(inst), std::move(v));
}

int main() {
    std::cout << fieldCount<test_types::BasicStruct>() << "\n";
    print(test_types::BasicStruct{false, 3, 4.5});
    test_types::BasicStruct b{false, 3, 4.5};
    print(b);
    const test_types::BasicStruct c{false, 3, 4.5};
    print(c);
    auto& d = b;
    print(d);
    const auto& e = c;
    print(e);
    flip(b);
    print(b);
}
