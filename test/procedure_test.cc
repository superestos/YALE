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

    auto eval(std::string str) {
        return expr(str)->eval(env_);
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

    env_->define("<", Value(Procedure::create<SmallProcedure>()));
    EXPECT_EQ(cond.call(env_, {expr("(< 1 2)"), expr("2"), expr("3")}).num(), 2);
    EXPECT_EQ(cond.call(env_, {expr("(< 3 2)"), expr("2"), expr("3")}).num(), 3);
}

TEST_F(ProcedureTest, Begin) {
    env_->define("begin", Value(Procedure::create<BeginProcedure>()));

    EXPECT_EQ(eval("(begin)").type(), VALUE_VOID);
    EXPECT_EQ(eval("(begin 1)").num(), 1);
    EXPECT_EQ(eval("(begin 1 2)").num(), 2);
    EXPECT_EQ(eval("(begin 1 2 '3rd)").quote(), "3rd");
}

TEST_F(ProcedureTest, Set) {
    env_->define("set!", Value(Procedure::create<SetProcedure>()));
    env_->define("+", Value(Procedure::create<AddProcedure>()));

    eval("(define x 0)");
    EXPECT_EQ(env_->get("x").num(), 0);

    EXPECT_EQ(eval("(set! x 4)").type(), VALUE_VOID);
    EXPECT_EQ(env_->get("x").num(), 4);

    EXPECT_EQ(eval("(set! x (+ x 1))").type(), VALUE_VOID);
    EXPECT_EQ(env_->get("x").num(), 5);
}

TEST_F(ProcedureTest, Cons) {
    env_->define("cons", Value(Procedure::create<ConsProcedure>()));
    env_->define("car", Value(Procedure::create<CarProcedure>()));
    env_->define("cdr", Value(Procedure::create<CdrProcedure>()));

    eval("(define x (cons 1 2))");
    EXPECT_EQ(eval("(car x)").num(), 1);
    EXPECT_EQ(eval("(cdr x)").num(), 2);
}

TEST_F(ProcedureTest, SelfDefinedIdentity) {
    auto id = expr("x");
    SelfDefinedProcedure identity(id, {"x"});

    EXPECT_EQ(identity.call(env_, {expr("42")}).num(), 42);
    EXPECT_EQ(identity.call(env_, {expr("'hi")}).quote(), "hi");
}

TEST_F(ProcedureTest, SelfDefinedIncrease) {
    auto inc = expr("(+ x 1)");
    SelfDefinedProcedure func(inc, {"x"});
    env_->define("+", Value(Procedure::create<AddProcedure>()));

    EXPECT_EQ(func.call(env_, {expr("42")}).num(), 43);
    EXPECT_EQ(func.call(env_, {expr("-42")}).num(), -41);
}

TEST_F(ProcedureTest, SelfDefinedFib) {
    auto fib_expr = expr("(if (< x 2) 1 (+ (fib (+ x -1)) (fib (+ x -2))))");

    ProcedurePtr fib_func = SelfDefinedProcedure::create(fib_expr, {"x"});
    env_->define("fib", Value(fib_func));
    env_->define("+", Value(Procedure::create<AddProcedure>()));
    env_->define("<", Value(Procedure::create<SmallProcedure>()));
    env_->define("if", Value(Procedure::create<IfProcedure>()));

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

TEST_F(ProcedureTest, SelfDefinedAcc) {
    env_->define("set!", Value(Procedure::create<SetProcedure>()));
    env_->define("+", Value(Procedure::create<AddProcedure>()));
    env_->define("begin", Value(Procedure::create<BeginProcedure>()));

    eval("(define (acc balance) (lambda (amount) (begin (set! balance (+ balance amount)) balance)))");
    EXPECT_EQ(eval("(acc 10)").type(), VALUE_PROCEDURE);

    EXPECT_EQ(eval("(acc 10)").env_->existed("balance"), true);

    eval("(define a1 (acc 10))");

    /*
    auto new_env = EnvironmentManager::create(env_);
    new_env->define("balance", Value(10));

    EXPECT_EQ(expr("(a1 0)")->eval(new_env).num(), 10);
    EXPECT_EQ(expr("(a1 10)")->eval(new_env).num(), 20);
    EXPECT_EQ(expr("(a1 20)")->eval(new_env).num(), 40);
    */

    EXPECT_EQ(eval("(a1 0)").num(), 10);
    EXPECT_EQ(eval("(a1 10)").num(), 20);
    EXPECT_EQ(eval("(a1 20)").num(), 40);
}
