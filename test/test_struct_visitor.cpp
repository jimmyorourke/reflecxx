#include <gtest/gtest.h>

#include "test_types.hpp"
#include <type_traits>

#include <proto/proto_base.hpp>
#include <proto/struct_visitor.hpp>

#include <generated/test_types_proto_generated.hpp>

struct InstanceTypeCounterVisitor {
    template <typename T>
    void operator()(const char*, const T&) {
        otherTypes++;
    }

    template <>
    void operator()<int>(const char*, const int&) {
        ints++;
    }

    template <>
    void operator()<double>(const char*, const double&) {
        doubles++;
    }

    int allTypes() { return ints + doubles + otherTypes; }

    int otherTypes{};
    int ints{};
    int doubles{};
};

struct TypeCounterVisitor {
    constexpr void operator()(const char*, const proto::BaseTag&) { otherTypes++; }

    constexpr void operator()(const char*, const proto::TagType<int>& tag) {
        // Example extracting type from tag type instance
        // C++20 remove_cvref would be better
        static_assert(std::is_same_v<typename std::decay_t<decltype(tag)>::type, int>);
        ints++;
    }

    int otherTypes{};
    int ints{};
};

template <typename T>
constexpr int countInts() {
    TypeCounterVisitor t{};
    proto::visit<T>(t);
    return t.ints;
}

template <typename T>
constexpr int countAllTypes() {
    TypeCounterVisitor t{};
    proto::forEachField<T>(t);
    return t.ints + t.otherTypes;
}

TEST(struct_visitor, visitStructMembers) {
    test_types::Wrapper w{};
    InstanceTypeCounterVisitor v{};
    proto::forEachField(w, v);

    EXPECT_EQ(v.otherTypes, 3);
    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.doubles, 1);

    static_assert(countAllTypes<test_types::Wrapper>() == 5);
}

TEST(struct_visitor, visitClassMembers) {
    test_types::BasicClass bc{};
    InstanceTypeCounterVisitor v{};
    proto::visit(bc, v);

    EXPECT_EQ(v.otherTypes, 1);
    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.doubles, 1);

    static_assert(countAllTypes<test_types::BasicClass>() == 3);
}

TEST(struct_visitor, visitDerivedClass) {
    test_types::ChildClass c{};
    InstanceTypeCounterVisitor v{};
    // visit parent by default
    proto::visit(c, v);

    EXPECT_EQ(v.ints, 2);
    EXPECT_EQ(v.allTypes(), 4);

    static_assert(countAllTypes<test_types::ChildClass>() == 4);
}

TEST(struct_visitor, visitDerivedClassUnreflectedBase) {
    test_types::ChildOfUnreflectedBaseClass cub{};
    InstanceTypeCounterVisitor v{};
    proto::visit(cub, v);

    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.allTypes(), 1);

    static_assert(countAllTypes<test_types::ChildOfUnreflectedBaseClass>() == 1);
}

TEST(struct_visitor, tupleCalls) {
    static_assert(proto::fieldCount<test_types::Basics>() == 3);

    static_assert(proto::getName<2, test_types::Basics>() == "d");

    test_types::Basics bs{/*b=*/true, /*i=*/1, /*d=*/1.5};
    auto& dref = proto::get<2>(bs);

    static_assert(std::is_same_v<std::remove_reference_t<decltype(dref)>, decltype(bs.d)>);

    dref += 3;

    EXPECT_EQ(bs.d, 4.5);

    test_types::Basics bs2{/*b=*/true, /*i=*/1, /*d=*/1.5};
    test_types::Basics bs3{/*b=*/true, /*i=*/1, /*d=*/1.5};
    EXPECT_TRUE(proto::eql1(bs3, bs2));
    bs2.b=false;
    EXPECT_FALSE(proto::eql1(bs3, bs2));



}

TEST(generation, type_traits) {
    struct MyType {};

    static_assert(!proto::IsProtoVisitableV<MyType>);
    static_assert(proto::IsProtoVisitableV<test_types::Basics>);
}
