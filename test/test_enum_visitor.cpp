#include <gtest/gtest.h>

#include <proto/enum_visitor.hpp>
#include <test_types.hpp>

TEST(enum_visitor, enumSize) {
    // put static_asserts in a TEST because why not
    static_assert(proto::enumSize<test_types::Unscoped>() == 4);
    static_assert(proto::enumSize<test_types::Scoped>() == 3);
}

TEST(enum_visitor, toName) {
    static_assert(proto::enumName(test_types::Fourth) == "Fourth");
    static_assert(proto::enumName(test_types::Scoped::Third) == "Third");
}

TEST(enum_visitor, fromName) {
    static_assert(proto::fromName<test_types::Unscoped>("Fourth") == test_types::Fourth);
    static_assert(proto::fromName<test_types::Scoped>("Third") == test_types::Scoped::Third);

    // not expected to compile
    // static_assert(proto::fromName<test_types::Scoped>("fifth") == test_types::Scoped::Third);

    EXPECT_ANY_THROW(proto::fromName<test_types::Scoped>("fifth"));
}

TEST(enum_visitor, contains) {
    // unscoped has an offset of 2 in values
    static_assert(proto::enumContains<test_types::Unscoped>(4));
    static_assert(!proto::enumContains<test_types::Unscoped>(0));
    static_assert(proto::enumContains<test_types::Scoped>(0));
    static_assert(!proto::enumContains<test_types::Scoped>(3));
}

TEST(enum_visitor, iterators) {
    std::array<test_types::Scoped, 3> scopedEs{test_types::Scoped::First, test_types::Scoped::Second,
                                               test_types::Scoped::Third};
    EXPECT_EQ(scopedEs, proto::enumerators<test_types::Scoped>());
    // or expected use case:
    size_t i = 0;
    for (const auto& e : proto::enumerators<test_types::Scoped>()) {
        EXPECT_EQ(e, scopedEs[i]);
        ++i;
    }

    std::array<const char*, 3> names{"First", "Second", "Third"};
    i = 0;
    for (const auto& e : proto::enumNames<test_types::Scoped>()) {
        EXPECT_EQ(e, names[i]);
        ++i;
    }
}
