#include "expression/expression.h"

#include "gtest/gtest.h"

#include "environment/environment.h"

class ExpressionTest : public ::testing::Test {
protected:
    EnvironmentManager manager_;
    EnvironmentPtr env_{manager_.global()};
};

TEST_F(ExpressionTest, ValueExpression) {
    ParseTreePointer num = std::make_shared<ParseTreeNode>(Token("42"));
    ParseTreePointer quote = std::make_shared<ParseTreeNode>(Token("'hi"));

    EXPECT_EQ(ValueExpression(num).eval(env_).num(), 42);
    EXPECT_EQ(ValueExpression(quote).eval(env_).quote(), "hi");
}

TEST_F(ExpressionTest, DefineExpression) {
    ParseTreePointer num = std::make_shared<ParseTreeNode>(Token("42"));
    ExpressionPtr x = std::shared_ptr<Expression>(new ValueExpression(num));

    DefineExpression def("x", x);
    Value value = def.eval(env_);

    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(env_->get("x").num(), 42);
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
