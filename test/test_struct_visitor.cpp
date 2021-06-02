#include <gtest/gtest.h>

#include "test_types.hpp"

#include <proto/proto_base.hpp>

#include <proto/struct_visitor.hpp>

// clean up
#include "../out.hpp"

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

    void clear() {
        otherTypes = 0;
        ints = 0;
        doubles = 0;
    }

    int otherTypes{};
    int ints{};
    int doubles{};
};

struct TypeCounterVisitor {
    constexpr void operator()(const char*, const proto::base_tag&) { otherTypes++; }

    constexpr void operator()(const char*, const proto::type_tag<int>& tag) {
        // Example extracting type from tag type instance
        // C++20 remove_cvref would be better
        static_assert(std::is_same_v<typename std::decay_t<decltype(tag)>::type, int>);
        ints++;
    }

    int otherTypes{};
    int ints{};
};

template <typename T>
constexpr int countInts(bool visitBaseClasses = true) {
    TypeCounterVisitor t{};
    proto::visit<T>(t, visitBaseClasses);
    return t.ints;
}

template <typename T>
constexpr int countAllTypes(bool visitBaseClasses = true) {
    TypeCounterVisitor t{};
    proto::visit<T>(t, visitBaseClasses);
    return t.ints + t.otherTypes;
}

TEST(struct_visitor, countTypesSimpleInstance) {
    // TODO: gtest parameterized test
    {
        static_assert(countAllTypes<Empty>() == 0);
        Empty e{};
        InstanceTypeCounterVisitor v{};
        proto::visit(e, v);

        EXPECT_EQ(v.otherTypes, 0);
        EXPECT_EQ(v.ints, 0);
        EXPECT_EQ(v.doubles, 0);
    }
    {
        static_assert(countAllTypes<Basics>() == 3);
        Basics b{};
        InstanceTypeCounterVisitor v{};
        proto::visit(b, v);

        EXPECT_EQ(v.otherTypes, 1);
        EXPECT_EQ(v.ints, 1);
        EXPECT_EQ(v.doubles, 1);
    }
    {
        static_assert(countAllTypes<Wrapper>() == 5);
        Wrapper w{};
        InstanceTypeCounterVisitor v{};
        proto::visit(w, v);

        EXPECT_EQ(v.otherTypes, 3);
        EXPECT_EQ(v.ints, 1);
        EXPECT_EQ(v.doubles, 1);
    }
    {
        static_assert(countAllTypes<BasicClass>() == 3);
        BasicClass bc{};
        InstanceTypeCounterVisitor v{};
        proto::visit(bc, v);

        EXPECT_EQ(v.otherTypes, 1);
        EXPECT_EQ(v.ints, 1);
        EXPECT_EQ(v.doubles, 1);
    }
}

TEST(struct_visitor, countTypesInheritanceInstance) {
    {
        static_assert(countAllTypes<ChildClass>() == 4);
        ChildClass c{};
        InstanceTypeCounterVisitor v{};
        // visit parent by default
        proto::visit(c, v);

        EXPECT_EQ(v.ints, 2);
        EXPECT_EQ(v.allTypes(), 4);
    }
    {
        static_assert(countAllTypes<ChildClass>(false) == 1);
        ChildClass c{};
        InstanceTypeCounterVisitor v{};
        proto::visit(c, v, /*visitBaseClasses=*/false);

        EXPECT_EQ(v.ints, 1);
        EXPECT_EQ(v.allTypes(), 1);
    }
    {
        static_assert(countAllTypes<ChildOfUnreflectedBaseClass>() == 1);
        ChildOfUnreflectedBaseClass cub{};
        InstanceTypeCounterVisitor v{};
        proto::visit(cub, v);

        EXPECT_EQ(v.ints, 1);
        EXPECT_EQ(v.allTypes(), 1);
    }
}

TEST(struct_visitor, tuplelikestuff) {
    static_assert(proto::fieldCount<Basics>() == 3);
    static_assert(proto::getName<2, Basics>() == "d");

    struct MyType {};

    static_assert(!proto::is_proto_visitable_v<MyType>);
    static_assert(proto::is_proto_visitable_v<Basics>);

    struct Basics b {
        true, 1, 1.5
    };
    auto& r = proto::get<2>(b);
    r += 3;
    EXPECT_EQ(b.d, 4.5);
}
