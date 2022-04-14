#include "expression/expression.h"

#include "gtest/gtest.h"

TEST(valueTest, BasicValue) {
    NumValue n(42);
    QuoteValue q("hello world");

    EXPECT_EQ(n(), 42);
    EXPECT_EQ(q(), "hello world");
}
