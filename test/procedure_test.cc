#include "expression/procedure.h"

#include "gtest/gtest.h"

#include "expression/expression.h"
#include "scanner/scanner.h"
#include "environment/environment.h"

class ProcedureTest : public ::testing::Test {
protected:
    EnvironmentManager manager_{ENV_BUILTIN};
    EnvironmentPtr env_{manager_.global()};

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
        //return expr(str)->eval(env_);
        return expr(str)->accept(visitor_);
    }
};
/*
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

    EXPECT_EQ(cond.call(env_, {expr("(< 1 2)"), expr("2"), expr("3")}).num(), 2);
    EXPECT_EQ(cond.call(env_, {expr("(< 3 2)"), expr("2"), expr("3")}).num(), 3);
}
*/
TEST_F(ProcedureTest, Begin) {
    EXPECT_EQ(eval("(begin)").type(), VALUE_VOID);
    EXPECT_EQ(eval("(begin 1)").num(), 1);
    EXPECT_EQ(eval("(begin 1 2)").num(), 2);
    EXPECT_EQ(eval("(begin 1 2 '3rd)").quote(), "3rd");
}

TEST_F(ProcedureTest, Cons) {
    eval("(define x (cons 1 2))");
    EXPECT_EQ(eval("(car x)").num(), 1);
    EXPECT_EQ(eval("(cdr x)").num(), 2);
}

TEST_F(ProcedureTest, List) {
    eval("(define a (list))");
    EXPECT_EQ(eval("(null? a)").num(), 1);

    eval("(define b (list 1))");
    EXPECT_EQ(eval("(null? b)").num(), 0);
    EXPECT_EQ(eval("(car b)").num(), 1);
    EXPECT_EQ(eval("(null? (cdr b))").num(), 1);

    eval("(define c (list 1 2 3))");
    EXPECT_EQ(eval("(null? c)").num(), 0);
    EXPECT_EQ(eval("(car c)").num(), 1);
    EXPECT_EQ(eval("(null? (cdr c))").num(), 0);
    EXPECT_EQ(eval("(car (cdr c))").num(), 2);
    EXPECT_EQ(eval("(null? (cdr (cdr c)))").num(), 0);
    EXPECT_EQ(eval("(car (cdr (cdr c)))").num(), 3);
    EXPECT_EQ(eval("(null? (cdr (cdr (cdr c))))").num(), 1);
}
/*
TEST_F(ProcedureTest, DefinedIdentity) {
    auto id = expr("x");
    LambdaProcedure identity(id, {"x"});

    EXPECT_EQ(identity.call(env_, {expr("42")}).num(), 42);
    EXPECT_EQ(identity.call(env_, {expr("'hi")}).quote(), "hi");
}

TEST_F(ProcedureTest, DefinedIncrease) {
    auto inc = expr("(+ x 1)");
    LambdaProcedure func(inc, {"x"});

    EXPECT_EQ(func.call(env_, {expr("42")}).num(), 43);
    EXPECT_EQ(func.call(env_, {expr("-42")}).num(), -41);
}

TEST_F(ProcedureTest, DefinedFib) {
    auto fib_expr = expr("(if (< x 2) 1 (+ (fib (+ x -1)) (fib (+ x -2))))");

    ProcedurePtr fib_func = Procedure::create(fib_expr, {"x"});
    env_->define("fib", Value(fib_func));

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
*/
TEST_F(ProcedureTest, DefinedAcc) {
    eval("(define (acc balance) (lambda (amount) (begin (set! balance (+ balance amount)) balance)))");
    EXPECT_EQ(eval("(acc 10)").type(), VALUE_PROCEDURE);

    EXPECT_EQ(eval("(acc 10)").env()->existed("balance"), true);

    eval("(define a1 (acc 10))");

    EXPECT_EQ(eval("(a1 0)").num(), 10);
    EXPECT_EQ(eval("(a1 10)").num(), 20);
    EXPECT_EQ(eval("(a1 20)").num(), 40);

    eval("(define a2 (acc 20))");
    EXPECT_EQ(eval("(a2 10)").num(), 30);
    EXPECT_EQ(eval("(a2 20)").num(), 50);

    EXPECT_EQ(eval("(a1 20)").num(), 60);
}

TEST_F(ProcedureTest, DefinedExisted) {
    eval(" \
    (define (element-of-set? x set) \
        (cond ((null? set) false) \
              ((= x (car set)) true) \
              ((< x (car set)) false) \
              (else (element-of-set? x (cdr set))) \
    ))");

    eval("(define empty (list))");
    eval("(define a (list 2 5 7))");

    EXPECT_EQ(eval("(element-of-set? 1 empty)").num(), 0);
    EXPECT_EQ(eval("(element-of-set? 1 a)").num(), 0);    
    EXPECT_EQ(eval("(element-of-set? 2 a)").num(), 1);

    EXPECT_EQ(eval("(element-of-set? 3 a)").num(), 0);
    
    EXPECT_EQ(eval("(element-of-set? 5 a)").num(), 1);
    EXPECT_EQ(eval("(element-of-set? 6 a)").num(), 0);
    EXPECT_EQ(eval("(element-of-set? 7 a)").num(), 1);
    EXPECT_EQ(eval("(element-of-set? 9 a)").num(), 0);
}

TEST_F(ProcedureTest, DefinedPair) {
    eval("(define (pair x y) (lambda (m) (cond ((= m 'head) x) ((= m 'tail) y) (else 'error))))");

    EXPECT_EQ(eval("((pair 3 5) 'head)").num(), 3);
    EXPECT_EQ(eval("((pair 3 5) 'tail)").num(), 5);

    eval("(define (head p) (p 'head))");
    eval("(define (tail p) (p 'tail))");
    eval("(define a (pair 4 7))");

    EXPECT_EQ(eval("(head a)").num(), 4);
    EXPECT_EQ(eval("(tail a)").num(), 7);
}

TEST_F(ProcedureTest, ListEqual) {
    eval(" \
    (define (list-eq? l1 l2) \
        (cond ((null? l1) (null? l2)) \
              ((null? l2) false) \
              ((= (car l1) (car l2)) (list-eq? (cdr l1) (cdr l2))) \
              (else false) \
    ))");

    EXPECT_EQ(eval("(list-eq? (list) (list))").num(), 1);
    EXPECT_EQ(eval("(list-eq? (list 1) (list))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list) (list 1))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list 'a) (list 'a))").num(), 1);
    EXPECT_EQ(eval("(list-eq? (list 'a) (list 'b))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list 3) (list (+ 1 2)))").num(), 1);
    EXPECT_EQ(eval("(list-eq? (list 2 3) (list 2))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list 2 8) (list 2 7))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list 3) (list 3 2))").num(), 0);
    EXPECT_EQ(eval("(list-eq? (list 4 5 6) (list 4 (+ 4 1) (+ 4 2)))").num(), 1);
}

TEST_F(ProcedureTest, Map) {
    eval(" \
    (define (list-eq? l1 l2) \
        (cond ((null? l1) (null? l2)) \
              ((null? l2) false) \
              ((= (car l1) (car l2)) (list-eq? (cdr l1) (cdr l2))) \
              (else false) \
    ))");

    eval("(define (map f l) ( \
            if (null? l) l (cons (f (car l)) (map f (cdr l))) \
    ))");
    eval("(define c (list 1 2))");
    //eval("(define l (list 3 1 0 4))");

    eval("(define dc (map (lambda (x) (+ x x)) c))");
    EXPECT_EQ(eval("dc").cons()[0]->num(), 2);
    EXPECT_EQ(eval("dc").cons()[1]->cons()[0]->num(), 4);
    EXPECT_EQ(eval("dc").cons()[1]->cons()[1]->type(), VALUE_VOID);

    eval("(define double (lambda (x) (+ x x)))");
    EXPECT_EQ(eval("(list-eq? (map double (list 4 3 2 1 0)) (list 8 6 4 2 0))").num(), 1);
}

TEST_F(ProcedureTest, Reduce) {
    eval("(define (reduce f unit l) ( \
        if (null? l) unit (f (car l) (reduce f unit (cdr l))) \
    ))");

    EXPECT_EQ(eval("(reduce + 0 (list))").num(), 0);
    EXPECT_EQ(eval("(reduce + 0 (list 1))").num(), 1);
    EXPECT_EQ(eval("(reduce + 0 (list 1 2))").num(), 3);
    EXPECT_EQ(eval("(reduce + 0 (list 1 2 3))").num(), 6);

    eval("(define (fold f acc l) ( \
        if (null? l) acc (fold f (f acc (car l)) (cdr l)) \
    ))");

    EXPECT_EQ(eval("(fold + 0 (list))").num(), 0);
    EXPECT_EQ(eval("(fold + 0 (list 1))").num(), 1);
    EXPECT_EQ(eval("(fold + 0 (list 1 2))").num(), 3);
    EXPECT_EQ(eval("(fold + 0 (list 1 2 3))").num(), 6);
}
