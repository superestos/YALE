#pragma once

#include <string>
#include <map>
#include <memory>
#include <cassert>
#include <ostream>

#include "parser/parser.h"

typedef enum {
    VALUE_VOID,
    VALUE_NUM,
    VALUE_QUOTE,
    VALUE_PROCEDURE,
    VALUE_CONSTRUCT,
} ValueType;

class Value;

class Environment;
typedef std::shared_ptr<Environment> EnvironmentPtr;

class Procedure;
typedef std::shared_ptr<Procedure> ProcedurePtr;

class Expression;
typedef std::shared_ptr<Expression> ExpressionPtr;

class Expression {
public:
    virtual Value eval(const EnvironmentPtr &env) const = 0;
    static ExpressionPtr create(const ParseTreePointer &parse_tree);
};

typedef int Num;
typedef std::string Quote;
typedef std::array<std::shared_ptr<Value>, 2> Construct;

class Value {
public:
    Value(): type_{VALUE_VOID} {}

    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(ProcedurePtr value, EnvironmentPtr env = {}): type_{VALUE_PROCEDURE}, procedure_{value}, env_{env} {}

    Value(Construct cons): type_{VALUE_CONSTRUCT}, cons_{cons} {}

    ValueType type() const;
    Num num() const;
    const Quote& quote() const;
    const ProcedurePtr procedure() const;
    const Construct cons() const;
    const EnvironmentPtr env() const;

private:
    ValueType type_;
    Num num_;
    Quote quote_;

    ProcedurePtr procedure_;
    EnvironmentPtr env_;

    Construct cons_;
};

std::ostream& operator<<(std::ostream& out, const Value &value);

class ValueExpression : public Expression {
public:
    ValueExpression(Value value): value_(value) {}

    ValueExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    Value value_;
    
    Value parse_lambda(const ParseTreePointer &parse_tree);
};

class DefineExpression : public Expression {
public:
    typedef enum {
        DEFINE,
        SET,
    } Type;

    DefineExpression(std::string name, ExpressionPtr expr, const std::vector<std::string> &arg_names = {}, Type type = DEFINE):
        name_{name}, arg_names_{arg_names}, expr_{expr}, type_{type} {}

    DefineExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    std::string name_;
    std::vector<std::string> arg_names_;
    ExpressionPtr expr_;
    Type type_;
};

class VariableExpression : public Expression {
public:
    VariableExpression(std::string name):
        name_{name} {}

    VariableExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;
    const std::string name() const;

private:
    std::string name_;
};

class ApplyExpression : public Expression {
public:
    ApplyExpression(const ExpressionPtr &function, const std::vector<ExpressionPtr> &args):
        function_{function}, args_{args} {}

    ApplyExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    ExpressionPtr function_;
    std::vector<ExpressionPtr> args_;
};

class CondExpression : public Expression {
public:
    CondExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    std::vector<ExpressionPtr> conditions_;
    std::vector<ExpressionPtr> exprs_;
};
