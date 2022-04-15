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
    VALUE_FUNCTION,
} ValueType;

class Value;

class Environment;
typedef std::shared_ptr<Environment> EnvironmentPtr;

class Expression {
public:
    virtual const Value& eval(const EnvironmentPtr &env) const = 0;
};

typedef std::shared_ptr<Expression> ExpressionPtr;

typedef int Num;
typedef std::string Quote;

class Value {
public:
    Value(): type_{VALUE_VOID} {}

    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(ExpressionPtr value): type_{VALUE_FUNCTION}, function_{value} {}

    ValueType type() const;
    Num num() const;
    const Quote& quote() const;
    const ExpressionPtr function() const;

private:
    ValueType type_;
    Num num_;
    Quote quote_;
    ExpressionPtr function_;
};

class ValueExpression : public Expression {
public:
    ValueExpression(const ParseTreePointer parse_tree);
    const Value& eval(const EnvironmentPtr &env) const;

private:
    Value value_;
};

class DefineExpression : public Expression {
public:
    DefineExpression(const ParseTreePointer parse_tree);

    DefineExpression(std::string name, ExpressionPtr expr):
        name_{name}, expr_{expr} {}

    const Value& eval(const EnvironmentPtr &env) const;

private:
    std::string name_;
    ExpressionPtr expr_;
};
