#include "expression/expression.h"

#include "gtest/gtest.h"

TEST(valueTest, BasicValue) {
    Value n(42);
    Value q("hello world");

    EXPECT_EQ(n.type(), VALUE_NUM);
    EXPECT_EQ(n.num(), 42);

    EXPECT_EQ(q.type(), VALUE_QUOTE);
    EXPECT_EQ(q.quote(), "hello world");
}

/*
TEST(valueTest, BasicValue) {
    NumValue n(42);
    QuoteValue q("hello world");

    EXPECT_EQ(n(), 42);
    EXPECT_EQ(q(), "hello world");
}
*/
