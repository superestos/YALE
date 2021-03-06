#include "expression/expression.h"

#include "gtest/gtest.h"

#include "environment/environment.h"
#include "scanner/scanner.h"
#include "expression/procedure.h"
#include "expression/expr_visitor.h"

class ExpressionTest : public ::testing::Test {
protected:
    std::stringstream input_;
    Scanner scanner_{input_};
    Parser parser_;

    ExpressionVisitor visitor_;

    void read(std::string str) {
        input_.clear();
        input_ << str;
        scanner_.clear();
        scanner_.read();
        parser_.analyze(scanner_.tokens());
    }

    auto expr(std::string str) {
        return Expression::create((read(str), parser_.next()));
    }

    auto eval(std::string str) {
        return expr(str)->accept(visitor_);
    }

    template <typename T>
    bool isinstance(Expression *ptr) {
        return dynamic_cast<T *>(ptr) != nullptr;
    }
};

TEST_F(ExpressionTest, ValueExpressionUsingSyntaxTree) {
    ParseTreePointer num = ParseTreeNode::create(Token("42"));
    ParseTreePointer quote = ParseTreeNode::create(Token("'hi"));

    EXPECT_EQ(ValueExpression(num).accept(visitor_).num(), 42);
    EXPECT_EQ(ValueExpression(quote).accept(visitor_).quote(), "hi");

    EXPECT_EQ(ValueExpression(88).accept(visitor_).num(), 88);
    EXPECT_EQ(ValueExpression(Value("lisp")).accept(visitor_).quote(), "lisp");

    read("24");
    auto num_expr = Expression::create(parser_.next());
    EXPECT_EQ(isinstance<ValueExpression>(num_expr.get()), true);
    EXPECT_EQ(num_expr->accept(visitor_).num(), 24);

    read("'xyz");
    auto quote_expr = Expression::create(parser_.next());
    EXPECT_EQ(isinstance<ValueExpression>(quote_expr.get()), true);
    EXPECT_EQ(quote_expr->accept(visitor_).quote(), "xyz");
}

TEST_F(ExpressionTest, DefineExpression1) {
    ParseTreePointer num = ParseTreeNode::create(Token("-42"));
    ExpressionPtr x = std::shared_ptr<Expression>(new ValueExpression(num));

    DefineExpression def("x", x);
    Value value = def.accept(visitor_);

    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(visitor_.env()->get("x").num(), -42);
}

TEST_F(ExpressionTest, DefineExpression2) {
    read("(define val 42)");

    auto def = Expression::create(parser_.next());
    Value value = def->accept(visitor_);

    EXPECT_EQ(isinstance<DefineExpression>(def.get()), true);
    EXPECT_EQ(value.type(), VALUE_VOID);
    EXPECT_EQ(visitor_.env()->get("val").num(), 42);
}

TEST_F(ExpressionTest, SetExpression) {
    eval("(define x 0)");
    EXPECT_EQ(eval("x").num(), 0);

    EXPECT_EQ(eval("(set! x 4)").type(), VALUE_VOID);
    EXPECT_EQ(eval("x").num(), 4);

    EXPECT_EQ(eval("(set! x (+ x 1))").type(), VALUE_VOID);
    EXPECT_EQ(eval("x").num(), 5);
}

TEST_F(ExpressionTest, VariableExpression1) {
    ExpressionPtr var = std::shared_ptr<Expression>(new VariableExpression("x"));

    visitor_.env()->define("x", 314);
    EXPECT_EQ(var->accept(visitor_).type(), VALUE_NUM);
    EXPECT_EQ(var->accept(visitor_).num(), 314);
}

TEST_F(ExpressionTest, VariableExpression2) {
    read("x");

    visitor_.env()->define("x", 314);
    auto var = Expression::create(parser_.next());

    EXPECT_EQ(isinstance<VariableExpression>(var.get()), true);
    EXPECT_EQ(var->accept(visitor_).type(), VALUE_NUM);
    EXPECT_EQ(var->accept(visitor_).num(), 314);
}

TEST_F(ExpressionTest, ApplyExpression1) {
    ExpressionPtr num1 = std::shared_ptr<Expression>(new ValueExpression(Value(8)));
    ExpressionPtr num2 = std::shared_ptr<Expression>(new ValueExpression(Value(1)));
    
    ExpressionPtr apply = std::shared_ptr<Expression>(new ApplyExpression(expr("+"), {num1, num2}));

    EXPECT_EQ(apply->accept(visitor_).num(), 9);
}

TEST_F(ExpressionTest, ApplyExpression2) {
    read("(+ 7 3)");

    auto apply = Expression::create(parser_.next());

    EXPECT_EQ(isinstance<ApplyExpression>(apply.get()), true);
    EXPECT_EQ(apply->accept(visitor_).num(), 10);
}

TEST_F(ExpressionTest, DefineProcedureAndApply1) {
    auto def = expr("(define (id x) x)");
    EXPECT_EQ(isinstance<DefineExpression>(def.get()), true);
    EXPECT_EQ(def->accept(visitor_).type(), VALUE_VOID);

    ApplyExpression apply(expr("id"), {expr("42")});
    EXPECT_EQ(apply.accept(visitor_).num(), 42);
}

TEST_F(ExpressionTest, DefineProcedureAndApply2) {
    DefineExpression def("id", expr("x"), {"x"});
    def.accept(visitor_);

    ApplyExpression apply(expr("id"), {expr("'abc")});
    EXPECT_EQ(apply.accept(visitor_).quote(), "abc");
}

TEST_F(ExpressionTest, CondExpression) {
    eval("(define (f x) (cond ((< x 10) (+ x 10)) ((< x 100) (+ x 100)) (else (+ x 1000))))");

    EXPECT_EQ(eval("(f 20)").num(), 120);
    EXPECT_EQ(eval("(f 5)").num(), 15);
    EXPECT_EQ(eval("(f 2000)").num(), 3000);
}

TEST_F(ExpressionTest, LambdaExpressionBasics) {
    auto lambda = expr("(lambda (x) (+ x x))");
    EXPECT_EQ(isinstance<ValueExpression>(lambda.get()), true);

    auto apply = expr(("((lambda (x) (+ x x)) 5)"));
    EXPECT_EQ(isinstance<ApplyExpression>(apply.get()), true);
    EXPECT_EQ(apply->accept(visitor_).num(), 10);
}

TEST_F(ExpressionTest, LambdaExpressionMultiVariable) {
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
