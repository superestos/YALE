#pragma once

#include <string>
#include <map>
#include <memory>
#include <cassert>

#include "parser/parser.h"

typedef enum {
    VALUE_VOID,
    VALUE_NUM,
    VALUE_QUOTE,
    VALUE_PROCEDURE,
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

class Value {
public:
    Value(): type_{VALUE_VOID} {}

    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(ProcedurePtr value): type_{VALUE_PROCEDURE}, procedure_{value} {}

    ValueType type() const;
    Num num() const;
    const Quote& quote() const;
    const ProcedurePtr procedure() const;

private:
    ValueType type_;
    Num num_;
    Quote quote_;
    ProcedurePtr procedure_;
};

class ValueExpression : public Expression {
public:
    ValueExpression(Value value): value_(value) {}

    ValueExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    Value value_;
};

class DefineExpression : public Expression {
public:
    DefineExpression(std::string name, ExpressionPtr expr, const std::vector<std::string> &arg_names = {}):
        name_{name}, arg_names_{arg_names}, expr_{expr} {}

    DefineExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    std::string name_;
    std::vector<std::string> arg_names_;
    ExpressionPtr expr_;
};

class VariableExpression : public Expression {
public:
    VariableExpression(std::string name):
        name_{name} {}

    VariableExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    std::string name_;
};

class ApplyExpression : public Expression {
public:
    ApplyExpression(const std::string &name, const std::vector<ExpressionPtr>& args):
        name_{name}, args_{args} {}

    ApplyExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;

private:
    std::string name_;
    std::vector<ExpressionPtr> args_;
};
