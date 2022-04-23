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
    virtual Value eval() const = 0;
    static ExpressionPtr create(const ParseTreePointer &parse_tree, const EnvironmentPtr &env);
    static ExpressionPtr create(const ExpressionPtr &expr, const EnvironmentPtr &env);

    auto& environment() {
        return env_;
    }

protected:
    EnvironmentPtr env_;
    enum {
        EXPR_VALUE,
        EXPR_APPLY,
        EXPR_DEFINE,
        EXPR_VAR,
    } type_;
};

typedef int Num;
typedef std::string Quote;
typedef std::array<std::shared_ptr<Value>, 2> Construct;

class Value {
public:
    Value(): type_{VALUE_VOID} {}

    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(ProcedurePtr value): type_{VALUE_PROCEDURE}, procedure_{value} {}

    Value(Construct cons): type_{VALUE_CONSTRUCT}, cons_{cons} {}

    ValueType type() const;
    Num num() const;
    const Quote& quote() const;
    const ProcedurePtr procedure() const;
    const Construct cons() const;

private:
    ValueType type_;
    Num num_;
    Quote quote_;
    ProcedurePtr procedure_;
    Construct cons_;
};

class ValueExpression : public Expression {
public:
    ValueExpression(Value value, const EnvironmentPtr &env): value_(value) {
        this->env_ = env;
        this->type_ = EXPR_VALUE;
    }

    ValueExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env);
    Value eval() const;

private:
    Value value_;
    
    Value parse_lambda(const ParseTreePointer &parse_tree);
};

class DefineExpression : public Expression {
public:
    DefineExpression(std::string name, ExpressionPtr expr, const EnvironmentPtr &env, const std::vector<std::string> &arg_names = {}):
        name_{name}, arg_names_{arg_names}, expr_{expr} 
    {
        this->env_ = env;
        this->type_ = EXPR_DEFINE;
    }

    DefineExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env);
    Value eval() const;

private:
    std::string name_;
    std::vector<std::string> arg_names_;
    ExpressionPtr expr_;
};

class VariableExpression : public Expression {
public:
    VariableExpression(std::string name, const EnvironmentPtr &env): name_{name} {
        this->env_ = env;
        this->type_ = EXPR_VAR;
    }

    VariableExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env);
    Value eval() const;
    const std::string name() const;

private:
    std::string name_;
};

class ApplyExpression : public Expression {
public:
    ApplyExpression(const ExpressionPtr &function, const std::vector<ExpressionPtr> &args, const EnvironmentPtr &env):
        function_{function}, args_{args} 
    {
        this->env_ = env;
        this->type_ = EXPR_APPLY;
    }

    ApplyExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env);
    Value eval() const;

private:
    ExpressionPtr function_;
    std::vector<ExpressionPtr> args_;
};
