#include "expression/expression.h"

#include "gtest/gtest.h"

#include "environment/environment.h"

class ExpressionTest : public ::testing::Test {
protected:
    EnvironmentManager manager_;
    EnvironmentPtr global_{manager_.global()};
};

TEST_F(ExpressionTest, ValueExpression) {
    ParseTreePointer num = std::make_shared<ParseTreeNode>(Token("42"));
    ParseTreePointer quote = std::make_shared<ParseTreeNode>(Token("'hi"));

    EXPECT_EQ(ValueExpression(num).eval(global_).num(), 42);
    EXPECT_EQ(ValueExpression(quote).eval(global_).quote(), "hi");
}

TEST(ValueTest, BasicValue) {
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
