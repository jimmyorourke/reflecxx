#include <gtest/gtest.h>

#include <simple_math/simple_math.hpp>

TEST(Basic, Add) {
    EXPECT_EQ(simple_math::add(3, 4), 7);
}

TEST(Basic, Mult) {
    EXPECT_EQ(simple_math::multiply(3, 4), 12);
}

TEST(Complex, NestedAdd) {
    EXPECT_EQ(simple_math::add(simple_math::add(3, 4), 4), 11);
}

TEST(Complex, NestedMult) {
    EXPECT_EQ(simple_math::multiply(simple_math::multiply(3, 4), 4), 48);
}
