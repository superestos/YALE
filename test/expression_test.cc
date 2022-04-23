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

    void read(std::string str) {
        input_.clear();
        input_ << str;
        scanner_.clear();
        scanner_.read();
        parser_.analyze(scanner_.tokens());
    }

    auto expr(std::string str) {
        return Expression::create((read(str), parser_.next()), env_);
    }

    auto eval(std::string str) {
        return expr(str)->eval();
    }

    template <typename T>
    bool isinstance(Expression *ptr) {
        return dynamic_cast<T *>(ptr) != nullptr;
    }
};

TEST_F(ExpressionTest, ValueExpression) {
    ParseTreePointer num = ParseTreeNode::create(Token("42"));
    ParseTreePointer quote = ParseTreeNode::create(Token("'hi"));

    EXPECT_EQ(ValueExpression(num, env_).eval().num(), 42);
    EXPECT_EQ(ValueExpression(quote, env_).eval().quote(), "hi");

    EXPECT_EQ(ValueExpression(88, env_).eval().num(), 88);
    EXPECT_EQ(ValueExpression(Value("lisp"), env_).eval().quote(), "lisp");

    auto num_expr = expr("24");
    EXPECT_EQ(isinstance<ValueExpression>(num_expr.get()), true);
    EXPECT_EQ(num_expr->eval().num(), 24);

    auto quote_expr = expr("'xyz");
    EXPECT_EQ(isinstance<ValueExpression>(quote_expr.get()), true);
    EXPECT_EQ(quote_expr->eval().quote(), "xyz");
}

TEST_F(ExpressionTest, DefineExpression1) {
    ParseTreePointer num = ParseTreeNode::create(Token("-42"));
    ExpressionPtr x = std::shared_ptr<Expression>(new ValueExpression(num, env_));

    DefineExpression def("x", x, env_);
    Value value = def.eval();

    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(env_->get("x").num(), -42);
}

TEST_F(ExpressionTest, DefineExpression2) {
    auto def = expr("(define val 42)");
    Value value = def->eval();

    EXPECT_EQ(isinstance<DefineExpression>(def.get()), true);
    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(env_->get("val").num(), 42);
}

TEST_F(ExpressionTest, VariableExpression1) {
    ExpressionPtr var = std::shared_ptr<Expression>(new VariableExpression("x", env_));

    env_->define("x", 314);
    EXPECT_EQ(var->eval().type(), VALUE_NUM);
    EXPECT_EQ(var->eval().num(), 314);
}

TEST_F(ExpressionTest, VariableExpression2) {
    env_->define("x", 314);
    auto var = expr("x");

    EXPECT_EQ(isinstance<VariableExpression>(var.get()), true);
    EXPECT_EQ(var->eval().type(), VALUE_NUM);
    EXPECT_EQ(var->eval().num(), 314);
}

TEST_F(ExpressionTest, ApplyExpression1) {
    env_->define("+", Value(Procedure::create<AddProcedure>()));

    ExpressionPtr num1 = std::shared_ptr<Expression>(new ValueExpression(Value(8), env_));
    ExpressionPtr num2 = std::shared_ptr<Expression>(new ValueExpression(Value(1), env_));
    
    ExpressionPtr apply = std::shared_ptr<Expression>(new ApplyExpression(expr("+"), {num1, num2}, env_));

    EXPECT_EQ(apply->eval().num(), 9);
}

TEST_F(ExpressionTest, ApplyExpression2) {
    env_->define("+", Value(Procedure::create<AddProcedure>()));
    auto apply = expr("(+ 7 3)");

    EXPECT_EQ(isinstance<ApplyExpression>(apply.get()), true);
    EXPECT_EQ(apply->eval().num(), 10);
}

TEST_F(ExpressionTest, DefineProcedureAndApply1) {
    auto def = expr("(define (id x) x)");
    EXPECT_EQ(isinstance<DefineExpression>(def.get()), true);
    EXPECT_EQ(def->eval().type(), VALUE_VOID);

    ApplyExpression apply(expr("id"), {expr("42")}, env_);
    EXPECT_EQ(apply.eval().num(), 42);
}

TEST_F(ExpressionTest, DefineProcedureAndApply2) {
    DefineExpression def("id", expr("x"), env_, {"x"});
    def.eval();

    ApplyExpression apply(expr("id"), {expr("'abc")}, env_);
    EXPECT_EQ(apply.eval().quote(), "abc");
}

TEST_F(ExpressionTest, LambdaExpressionBasics) {
    env_->define("+", Value(Procedure::create<AddProcedure>()));

    auto lambda = expr("(lambda (x) (+ x x))");
    EXPECT_EQ(isinstance<ValueExpression>(lambda.get()), true);

    auto apply = expr(("((lambda (x) (+ x x)) 5)"));
    EXPECT_EQ(isinstance<ApplyExpression>(apply.get()), true);
    EXPECT_EQ(apply->eval().num(), 10);
}

TEST_F(ExpressionTest, LambdaExpressionMultiVariable) {
    env_->define("+", Value(Procedure::create<AddProcedure>()));

    auto lambda = expr("(lambda (x y) (+ x y))");
    EXPECT_EQ(isinstance<ValueExpression>(lambda.get()), true);
    eval("(define add (lambda (x y) (+ x y)))");

    EXPECT_EQ(isinstance<ApplyExpression>(expr("(add 3 2)").get()), true);
    EXPECT_EQ(eval("(add 3 2)").num(), 5);
}

TEST(ValueTest, BasicValue) {
    Value n(42);
    Value q("lisp");
    Value c({std::make_shared<Value>(42), std::make_shared<Value>("lisp")});

    EXPECT_EQ(n.type(), VALUE_NUM);
    EXPECT_EQ(n.num(), 42);

    EXPECT_EQ(q.type(), VALUE_QUOTE);
    EXPECT_EQ(q.quote(), "lisp");

    EXPECT_EQ(c.type(), VALUE_CONSTRUCT);
    EXPECT_EQ(c.cons()[0]->type(), VALUE_NUM);
    EXPECT_EQ(c.cons()[0]->num(), 42);
    EXPECT_EQ(c.cons()[1]->type(), VALUE_QUOTE);
    EXPECT_EQ(c.cons()[1]->quote(), "lisp");
}
