#include <gtest/gtest.h>

#include "test_types.hpp"
#include <proto/enum_visitor.hpp>

//clean up
#include "../out.hpp"

TEST(enum_visitor, enumSize)
{
    // put static_asserts in a TEST because why not
    static_assert(proto::enumSize<Unscoped>() == 4);
    static_assert(proto::enumSize<Scoped>() == 3);
}

TEST(enum_visitor, toName)
{
    static_assert(proto::toString(Fourth) == "Fourth");
    static_assert(proto::toString(Scoped::Third) == "Third");
}

TEST(enum_visitor, fromName)
{
    static_assert(proto::fromString<Unscoped>("Fourth") == Fourth);
    static_assert(proto::fromString<Scoped>("Third") == Scoped::Third);

    // not expected to compile
    //static_assert(proto::fromString<Scoped>("fifth") == Scoped::Third);

    EXPECT_ANY_THROW(proto::fromString<Scoped>("fifth"));
}

TEST(enum_visitor, contains)
{
    // unscoped has an offset of 2 in values
    static_assert(proto::enumContains<Unscoped>(4));
    static_assert(!proto::enumContains<Unscoped>(0));
    static_assert(proto::enumContains<Scoped>(0));
    static_assert(!proto::enumContains<Scoped>(3));
}

TEST(enum_visitor, iterators)
{
    std::array<Scoped, 3> scopedEs{Scoped::First, Scoped::Second, Scoped::Third};
    EXPECT_EQ(scopedEs, proto::enumerators<Scoped>());
    // or expected use case:
    size_t i = 0;
    for (const auto& e : proto::enumerators<Scoped>()) {
        EXPECT_EQ(e, scopedEs[i]);
        ++i;
    }

    std::array<const char*, 3> names{"First", "Second", "Third"};
    i = 0;
    for (const auto& e : proto::enumNames<Scoped>()) {
        EXPECT_EQ(e, names[i]);
        ++i;
    }
}
