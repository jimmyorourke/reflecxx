// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#include <gtest/gtest.h>

#include <array>

#include <libtest_types/enums.hpp>
#include <reflecxx/enum_visitor.hpp>

TEST(enum_visitor, getName) {
    // put static_asserts in a TEST simply for organization
    static_assert(reflecxx::getName<test_types::Unscoped>() == "Unscoped");
    static_assert(reflecxx::getName<test_types::Scoped>() == "Scoped");
}

TEST(enum_visitor, enumSize) {
    static_assert(reflecxx::enumSize<test_types::Unscoped>() == 4);
    static_assert(reflecxx::enumSize<test_types::Scoped>() == 3);
}

TEST(enum_visitor, toName) {
    static_assert(reflecxx::enumName(test_types::Fourth) == "Fourth");
    static_assert(reflecxx::enumName(test_types::Scoped::Third) == "Third");
}

TEST(enum_visitor, fromName) {
    static_assert(reflecxx::fromName<test_types::Unscoped>("Fourth") == test_types::Fourth);
    static_assert(reflecxx::fromName<test_types::Scoped>("Third") == test_types::Scoped::Third);

    // not expected to compile
    // static_assert(reflecxx::fromName<test_types::Scoped>("fifth") == test_types::Scoped::Third);

    // Ensure we don't have to use string literals.
    std::string s = "Third";
    EXPECT_EQ(reflecxx::fromName<test_types::Scoped>(s.data()), test_types::Scoped::Third);

    EXPECT_ANY_THROW(reflecxx::fromName<test_types::Scoped>("fifth"));
}

TEST(enum_visitor, contains) {
    // unscoped has an offset of 2 in values
    static_assert(!reflecxx::enumContains<test_types::Unscoped>(0));
    static_assert(reflecxx::enumContains<test_types::Unscoped>(4));

    static_assert(reflecxx::enumContains<test_types::Scoped>(0));
    static_assert(!reflecxx::enumContains<test_types::Scoped>(3));
}

TEST(enum_visitor, enumerators) {
    std::array<test_types::Scoped, 3> scopedEs{test_types::Scoped::First, test_types::Scoped::Second,
                                               test_types::Scoped::Third};

    EXPECT_EQ(scopedEs, reflecxx::enumerators<test_types::Scoped>());
    // or expected use case:
    size_t i = 0;
    for (const auto& e : reflecxx::enumerators<test_types::Scoped>()) {
        EXPECT_EQ(e, scopedEs[i]);
        ++i;
    }
}

TEST(enum_visitor, names) {
    std::array<std::string_view, 3> names{"First", "Second", "Third"};
    size_t i = 0;
    for (const auto& e : reflecxx::enumNames<test_types::Scoped>()) {
        EXPECT_EQ(e, names[i]);
        ++i;
    }
}
