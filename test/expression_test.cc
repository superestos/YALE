#include "expression/expression.h"

#include "gtest/gtest.h"

#include "environment/environment.h"
#include "scanner/scanner.h"
#include "expression/procedure.h"

class ExpressionTest : public ::testing::Test {
protected:
    EnvironmentManager manager_;
    EnvironmentPtr env_{manager_.global()};

    std::stringstream input_;
    Scanner scanner_{input_};
    Parser parser_;
};

TEST_F(ExpressionTest, ValueExpression) {
    ParseTreePointer num = std::make_shared<ParseTreeNode>(Token("42"));
    ParseTreePointer quote = std::make_shared<ParseTreeNode>(Token("'hi"));

    EXPECT_EQ(ValueExpression(num).eval(env_).num(), 42);
    EXPECT_EQ(ValueExpression(quote).eval(env_).quote(), "hi");

    EXPECT_EQ(ValueExpression(88).eval(env_).num(), 88);
    EXPECT_EQ(ValueExpression(Value("lisp")).eval(env_).quote(), "lisp");
}

TEST_F(ExpressionTest, DefineExpression1) {
    ParseTreePointer num = std::make_shared<ParseTreeNode>(Token("42"));
    ExpressionPtr x = std::shared_ptr<Expression>(new ValueExpression(num));

    DefineExpression def("x", x);
    Value value = def.eval(env_);

    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(env_->get("x").num(), 42);
}

TEST_F(ExpressionTest, DefineExpression2) {
    input_.clear();
    input_ << "(define val 42)";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    DefineExpression def(parser_.tree()[0]);
    Value value = def.eval(env_);

    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(env_->get("val").num(), 42);
}

TEST_F(ExpressionTest, VariableExpression1) {
    ExpressionPtr var = std::shared_ptr<Expression>(new VariableExpression("x"));

    env_->define("x", 314);
    EXPECT_EQ(var->eval(env_).num(), 314);
}

TEST_F(ExpressionTest, ApplyExpression1) {
    ExpressionPtr x = std::shared_ptr<Expression>(new VariableExpression("x"));
    ProcedurePtr id = std::shared_ptr<Procedure>(new SelfDefinedProcedure(x, {"x"}));

    ExpressionPtr val = std::shared_ptr<Expression>(new ValueExpression(365));
    ExpressionPtr apply = std::shared_ptr<Expression>(new ApplyExpression(id, {val}));

    EXPECT_EQ(apply->eval(env_).num(), 365);
}

TEST_F(ExpressionTest, DynamicApplyExpression1) {
    env_->define("+", Value(std::shared_ptr<Procedure>(new AddProcedure())));

    ExpressionPtr num1 = std::shared_ptr<Expression>(new ValueExpression(Value(8)));
    ExpressionPtr num2 = std::shared_ptr<Expression>(new ValueExpression(Value(1)));
    
    ExpressionPtr expr = std::shared_ptr<Expression>(new DynamicApplyExpression("+", {num1, num2}));

    EXPECT_EQ(expr->eval(env_).num(), 9);
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
