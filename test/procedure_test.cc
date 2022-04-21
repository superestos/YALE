#include "expression/procedure.h"

#include "gtest/gtest.h"

#include "expression/expression.h"
#include "scanner/scanner.h"
#include "environment/environment.h"

class ProcedureTest : public ::testing::Test {
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
        return Expression::create((read(str), parser_.next()));
    }
};

TEST_F(ProcedureTest, Add) {
    AddProcedure add;
    auto val1 = expr("2");
    auto val2 = expr("4");

    std::vector<ExpressionPtr> args = {val1, val2};

    EXPECT_EQ(add.call(env_, args).num(), 6);
}

TEST_F(ProcedureTest, Condition) {
    EqualProcedure equal;
    SmallProcedure small;
    IfProcedure cond;

    EXPECT_EQ(equal.call(env_, {expr("2"), expr("2")}).num(), 1);
    EXPECT_EQ(equal.call(env_, {expr("3"), expr("2")}).num(), 0);
    EXPECT_EQ(equal.call(env_, {expr("'b"), expr("'b")}).num(), 1);
    EXPECT_EQ(equal.call(env_, {expr("'yes"), expr("'no")}).num(), 0);

    EXPECT_EQ(cond.call(env_, {expr("1"), expr("2"), expr("3")}).num(), 2);
    EXPECT_EQ(cond.call(env_, {expr("0"), expr("2"), expr("3")}).num(), 3);

    env_->define("<", Value(std::shared_ptr<Procedure>(new SmallProcedure())));
    EXPECT_EQ(cond.call(env_, {expr("(< 1 2)"), expr("2"), expr("3")}).num(), 2);
    EXPECT_EQ(cond.call(env_, {expr("(< 3 2)"), expr("2"), expr("3")}).num(), 3);
}

/*
TEST_F(ProcedureTest, SelfDefinedIdentity) {
    ExpressionPtr expr = std::shared_ptr<Expression>(new VariableExpression("x"));
    SelfDefinedProcedure identity(expr, {"x"});

    std::vector<Value> num_args = {42};
    EXPECT_EQ(identity.call(env_, num_args).num(), 42);

    std::vector<Value> quote_args = {Value("hi")};
    EXPECT_EQ(identity.call(env_, quote_args).quote(), "hi");
}

TEST_F(ProcedureTest, SelfDefinedIncrease) {
    ExpressionPtr x = std::shared_ptr<Expression>(new VariableExpression("x"));
    ExpressionPtr one = std::shared_ptr<Expression>(new ValueExpression(Value(1)));
    ProcedurePtr add = std::shared_ptr<Procedure>(new AddProcedure());
    ExpressionPtr inc_expr = std::shared_ptr<Expression>(new ApplyExpression(add, {x, one}));

    SelfDefinedProcedure inc_func(inc_expr, {"x"});

    std::vector<Value> num_args = {42};
    EXPECT_EQ(inc_func.call(env_, num_args).num(), 43);
}

TEST_F(ProcedureTest, SelfDefinedFib) {
    ExpressionPtr x = std::shared_ptr<Expression>(new VariableExpression("x"));
    ExpressionPtr minus_one = std::shared_ptr<Expression>(new ValueExpression(Value(-1)));
    ExpressionPtr minus_two = std::shared_ptr<Expression>(new ValueExpression(Value(-2)));
    ExpressionPtr one = std::shared_ptr<Expression>(new ValueExpression(Value(1)));
    ExpressionPtr two = std::shared_ptr<Expression>(new ValueExpression(Value(2)));

    ProcedurePtr add = std::shared_ptr<Procedure>(new AddProcedure());
    ExpressionPtr minus_one_expr = std::shared_ptr<Expression>(new ApplyExpression(add, {x, minus_one}));
    ExpressionPtr minus_two_expr = std::shared_ptr<Expression>(new ApplyExpression(add, {x, minus_two}));

    ExpressionPtr fib_x_1 = std::shared_ptr<Expression>(new DynamicApplyExpression("fib", {minus_one_expr}));
    ExpressionPtr fib_x_2 = std::shared_ptr<Expression>(new DynamicApplyExpression("fib", {minus_two_expr}));
    ExpressionPtr rec_fib = std::shared_ptr<Expression>(new ApplyExpression(add, {fib_x_1, fib_x_2}));

    ProcedurePtr sm = std::shared_ptr<Procedure>(new SmallProcedure());
    ProcedurePtr if_ = std::shared_ptr<Procedure>(new IfProcedure());
    ExpressionPtr sm_2 = std::shared_ptr<Expression>(new ApplyExpression(sm, {x, two}));
    ExpressionPtr if_expr = std::shared_ptr<Expression>(new ApplyExpression(if_, {sm_2, one, rec_fib}));

    ProcedurePtr fib = std::shared_ptr<Procedure>(new SelfDefinedProcedure(if_expr, {"x"}));
    env_->define("fib", Value(fib));

    EXPECT_EQ(fib->call(env_, {1}).num(), 1);
}
*/

TEST_F(ProcedureTest, SelfDefinedIdentity) {
    auto id = expr("x");
    SelfDefinedProcedure identity(id, {"x"});

    EXPECT_EQ(identity.call(env_, {expr("42")}).num(), 42);
    EXPECT_EQ(identity.call(env_, {expr("'hi")}).quote(), "hi");
}

TEST_F(ProcedureTest, SelfDefinedIncrease) {
    auto inc = expr("(+ x 1)");
    SelfDefinedProcedure func(inc, {"x"});
    env_->define("+", Value(std::shared_ptr<Procedure>(new AddProcedure())));

    EXPECT_EQ(func.call(env_, {expr("42")}).num(), 43);
    EXPECT_EQ(func.call(env_, {expr("-42")}).num(), -41);
}


TEST_F(ProcedureTest, SelfDefinedFib) {
    auto fib_expr = expr("(if (< x 2) 1 (+ (fib (+ x -1)) (fib (+ x -2))))");

    ProcedurePtr fib_func = std::shared_ptr<Procedure>(new SelfDefinedProcedure(fib_expr, {"x"}));
    env_->define("fib", Value(fib_func));
    env_->define("+", Value(std::shared_ptr<Procedure>(new AddProcedure())));
    env_->define("<", Value(std::shared_ptr<Procedure>(new SmallProcedure())));
    env_->define("if", Value(std::shared_ptr<Procedure>(new IfProcedure())));

    EXPECT_EQ(fib_func->call(env_, {expr("1")}).num(), 1);
    EXPECT_EQ(fib_func->call(env_, {expr("2")}).num(), 2);
    EXPECT_EQ(fib_func->call(env_, {expr("3")}).num(), 3);
    EXPECT_EQ(fib_func->call(env_, {expr("4")}).num(), 5);
    EXPECT_EQ(fib_func->call(env_, {expr("5")}).num(), 8);
    EXPECT_EQ(fib_func->call(env_, {expr("6")}).num(), 13);
    EXPECT_EQ(fib_func->call(env_, {expr("7")}).num(), 21);
    EXPECT_EQ(fib_func->call(env_, {expr("8")}).num(), 34);
    EXPECT_EQ(fib_func->call(env_, {expr("9")}).num(), 55);
}

