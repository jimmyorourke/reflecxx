#include <gtest/gtest.h>

#include <libtest_types/classes.hpp>
#include <libtest_types/structs.hpp>
#include <reflecxx/struct_visitor.hpp>
#include <reflecxx/visit.hpp>

namespace {
struct InstanceTypeCounterVisitor {
    template <typename T>
    void operator()(std::string_view, const T&) {
        otherTypes++;
    }

    template <>
    void operator()<int>(std::string_view, const int&) {
        ints++;
    }

    template <>
    void operator()<double>(std::string_view, const double&) {
        doubles++;
    }

    int allTypes() { return ints + doubles + otherTypes; }

    int otherTypes{};
    int ints{};
    int doubles{};
};

struct TypeCounterVisitor {
    constexpr void operator()(std::string_view, const reflecxx::base_tag&) { otherTypes++; }

    constexpr void operator()(std::string_view, const reflecxx::type_tag<int>& tag) {
        // Example extracting type from tag type instance
        static_assert(std::is_same_v<typename std::remove_reference_t<decltype(tag)>::type, int>);
        ints++;
    }

    int otherTypes{};
    int ints{};
};

template <typename T>
constexpr int countAllTypes() {
    TypeCounterVisitor t{};
    reflecxx::forEachField<T>(t);
    return t.ints + t.otherTypes;
}
} // namespace

TEST(struct_visitor, visitStructMembers) {
    test_types::NestingStruct s{};
    InstanceTypeCounterVisitor v{};
    reflecxx::forEachField(s, v);

    EXPECT_EQ(v.otherTypes, 3);
    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.doubles, 1);

    static_assert(countAllTypes<test_types::NestingStruct>() == 5);
    static_assert(reflecxx::fieldCount<test_types::NestingStruct>() == 5);
}

TEST(struct_visitor, visitClassMembers) {
    test_types::BasicClass bc{};
    InstanceTypeCounterVisitor v{};
    reflecxx::forEachField(bc, v);

    EXPECT_EQ(v.otherTypes, 1);
    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.doubles, 1);

    static_assert(countAllTypes<test_types::BasicClass>() == 3);
    static_assert(reflecxx::fieldCount<test_types::BasicClass>() == 3);
}

TEST(struct_visitor, visitDerivedClass) {
    test_types::ChildClass c{};
    InstanceTypeCounterVisitor v{};
    // visit parent by default
    reflecxx::forEachField(c, v);

    EXPECT_EQ(v.ints, 2);
    EXPECT_EQ(v.allTypes(), 4);

    static_assert(countAllTypes<test_types::ChildClass>() == 4);
    static_assert(reflecxx::fieldCount<test_types::ChildClass>() == 4);

    // 2 level inheritance
    InstanceTypeCounterVisitor sv{};
    test_types::SecondLevelChildClass sc{};

    reflecxx::forEachField(sc, sv);

    EXPECT_EQ(sv.ints, 2);
    EXPECT_EQ(sv.allTypes(), 5);

    static_assert(countAllTypes<test_types::SecondLevelChildClass>() == 5);
    static_assert(reflecxx::fieldCount<test_types::SecondLevelChildClass>() == 5);
}

TEST(struct_visitor, visitDerivedClassUnreflectedBase) {
    test_types::ChildOfUnreflectedBaseClass cub{};
    InstanceTypeCounterVisitor v{};

    reflecxx::forEachField(cub, v);

    EXPECT_EQ(v.ints, 1);
    EXPECT_EQ(v.allTypes(), 1);

    static_assert(countAllTypes<test_types::ChildOfUnreflectedBaseClass>() == 1);
    static_assert(reflecxx::fieldCount<test_types::ChildOfUnreflectedBaseClass>() == 1);
}

TEST(struct_visitor, get) {
    static_assert(reflecxx::getName<2, test_types::BasicStruct>() == "d");

    test_types::BasicStruct bs{/*b=*/true, /*i=*/1, /*d=*/1.5};
    auto& dref = reflecxx::get<2>(bs);

    static_assert(std::is_same_v<decltype(dref), decltype(bs.d)&>);
    EXPECT_EQ(dref, bs.d);
    dref += 3;

    EXPECT_EQ(bs.d, 4.5);

    const test_types::BasicStruct bs2{/*b=*/true, /*i=*/1, /*d=*/1.5};
    auto& dref2 = reflecxx::get<2>(bs2);

    // constness is maintained
    static_assert(std::is_const_v<std::remove_reference_t<decltype(dref2)>>);
}

TEST(struct_visitor, equalTo) {
    test_types::BasicStruct bs1{/*b=*/true, /*i=*/1, /*d=*/1.5};
    test_types::BasicStruct bs2{/*b=*/true, /*i=*/1, /*d=*/1.5};

    EXPECT_TRUE(reflecxx::equalTo(bs1, bs2));

    test_types::BasicStruct bs3{/*b=*/false, /*i=*/1, /*d=*/1.5};

    EXPECT_FALSE(reflecxx::equalTo(bs1, bs3));

    test_types::NestingStruct ns1{1, 1.5, bs2, {bs2, bs2, bs2}, {bs3, bs3}};
    test_types::NestingStruct ns2{1, 1.5, bs2, {bs2, bs2, bs2}, {bs3, bs3}};

    EXPECT_TRUE(reflecxx::equalTo(ns1, ns2));

    test_types::NestingStruct ns3{1, 1.5, bs2, {bs2, bs3, bs2}, {bs3, bs3}};

    EXPECT_FALSE(reflecxx::equalTo(ns1, ns3));

    test_types::NestingStruct ns4{1, 1.5, bs2, {bs2, bs2, bs2}, {bs3, bs2}};

    EXPECT_FALSE(reflecxx::equalTo(ns1, ns4));
}

TEST(struct_visitor, greaterThanLessThan) {
    test_types::BasicStruct bs1{/*b=*/true, /*i=*/1, /*d=*/1.5};

    EXPECT_TRUE(reflecxx::equalTo(bs1, bs1));
    EXPECT_FALSE(reflecxx::lessThan(bs1, bs1));
    EXPECT_FALSE(reflecxx::greaterThan(bs1, bs1));

    test_types::BasicStruct bs2{/*b=*/true, /*i=*/1, /*d=*/0.5};
    test_types::BasicStruct bs3{/*b=*/true, /*i=*/2, /*d=*/1.5};

    EXPECT_TRUE(reflecxx::lessThan(bs2, bs1));
    EXPECT_FALSE(reflecxx::greaterThan(bs2, bs1));
    EXPECT_FALSE(reflecxx::lessThan(bs3, bs1));
    EXPECT_TRUE(reflecxx::greaterThan(bs3, bs1));

    test_types::BasicStruct bs4{/*b=*/false, /*i=*/2, /*d=*/0.5};

    EXPECT_FALSE(reflecxx::lessThan(bs4, bs1));
    EXPECT_FALSE(reflecxx::greaterThan(bs4, bs1));
}

TEST(generation, type_traits) {
    struct MyType {};

    static_assert(!reflecxx::is_reflecxx_visitable_v<MyType>);

    static_assert(reflecxx::is_reflecxx_visitable_v<test_types::BasicStruct>);
}
