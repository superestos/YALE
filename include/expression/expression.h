#pragma once

#include <string>
#include <map>
#include <memory>
#include <cassert>
#include <ostream>
#include <array>
#include <variant>

#include "parser/parser.h"
//#include "expression/expr_visitor.h"

enum ValueType {
    VALUE_VOID,
    VALUE_NUM,
    VALUE_QUOTE,
    VALUE_PROCEDURE,
    VALUE_CONSTRUCT,
};

class Value;
class ExpressionVisitor;

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Procedure;
using ProcedurePtr = std::shared_ptr<Procedure>;

class Expression;
using ExpressionPtr = std::shared_ptr<Expression>;

class Expression {
public:
    virtual Value eval(const EnvironmentPtr &env) const = 0;
    virtual Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const = 0;
    static ExpressionPtr create(const ParseTreePointer &parse_tree);
};

using Num = int;
using Quote = std::string;
using Construct = std::array<std::shared_ptr<Value>, 2>;

class Value {
public:
    Value(): type_{VALUE_VOID} {}

    Value(Num value): type_{VALUE_NUM}, val_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, val_{value} {}

    Value(ProcedurePtr value, EnvironmentPtr env = {}): type_{VALUE_PROCEDURE}, val_{std::pair{value, env}} {}

    Value(Construct cons): type_{VALUE_CONSTRUCT}, val_{cons} {}

    ValueType type() const;
    Num num() const;
    const Quote& quote() const;
    const ProcedurePtr procedure() const;
    const Construct cons() const;
    const EnvironmentPtr env() const;

private:
    using Closure = std::pair<ProcedurePtr, EnvironmentPtr>;

    ValueType type_;
    std::variant<Num, Quote, Closure, Construct> val_;
};

std::ostream& operator<<(std::ostream& out, const Value &value);

class ValueExpression : public Expression {
public:
    ValueExpression(Value value): value_(value) {}

    ValueExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;
    Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const;

    Value value() const {
        return value_;
    }

private:
    Value value_;
    
    Value parse_lambda(const ParseTreePointer &parse_tree);
};

class DefineExpression : public Expression {
public:
    enum DeclareType {
        DEFINE,
        SET,
    };

    DefineExpression(std::string name, ExpressionPtr expr, const std::vector<std::string> &arg_names = {}, DeclareType type = DEFINE):
        name_{name}, arg_names_{arg_names}, expr_{expr}, type_{type} {}

    DefineExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;
    Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const;

    std::string name() const {
        return name_;
    }

    std::vector<std::string> arg_names() const {
        return arg_names_;
    }

    ExpressionPtr def_expr() const {
        return expr_;
    }

    bool is_define() const {
        return type_ == DEFINE;
    }

private:
    std::string name_;
    std::vector<std::string> arg_names_;
    ExpressionPtr expr_;
    DeclareType type_;
};

class VariableExpression : public Expression {
public:
    VariableExpression(std::string name):
        name_{name} {}

    VariableExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;
    Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const;

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
    Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const;

    ExpressionPtr function() const {
        return function_;
    }

    ExpressionPtr arg(size_t i) const {
        return args_[i];
    }

    std::vector<ExpressionPtr> args() const {
        return args_;
    }

    size_t length() const {
        return args_.size();
    }

private:
    ExpressionPtr function_;
    std::vector<ExpressionPtr> args_;
};

class CondExpression : public Expression {
public:
    CondExpression(const ParseTreePointer &parse_tree);
    Value eval(const EnvironmentPtr &env) const;
    Value accept(ExpressionVisitor &visitor, const EnvironmentPtr &env) const;

    ExpressionPtr condition(size_t i) const {
        return conditions_[i];
    }

    ExpressionPtr expr(size_t i) const {
        return exprs_[i];
    }

    size_t length() const {
        return exprs_.size();
    }

private:
    std::vector<ExpressionPtr> conditions_;
    std::vector<ExpressionPtr> exprs_;
};
