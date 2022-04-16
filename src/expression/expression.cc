#include "expression/expression.h"

#include "environment/environment.h"

Value void_value;

ValueType Value::type() const {
    return type_;
}

Num Value::num() const {
    assert(type() == VALUE_NUM);
    return num_;
}

const Quote& Value::quote() const {
    assert(type() == VALUE_QUOTE);
    return quote_;
}

const ExpressionPtr Value::function() const {
    assert(type() == VALUE_FUNCTION);
    return function_;
}

ValueExpression::ValueExpression(const ParseTreePointer parse_tree) {
    if (!parse_tree->isCompound()) {
        Token token = parse_tree->token();
        if (token.type() == TOKEN_QUOTE) {
            value_ = Value(token.name().substr(1));
        } else if (token.type() == TOKEN_NUM) {
            value_ = Value(std::stoi(token.name()));
        }
    }
}

const Value& ValueExpression::eval(const EnvironmentPtr &env) const {
    return value_;
}

DefineExpression::DefineExpression(const ParseTreePointer parse_tree) {
    assert(parse_tree->isCompound());
    auto& args = parse_tree->children();
    assert(args.size() == 3);
    assert(args[0]->token().name() == "define");
    assert(args[1]->token().type() == TOKEN_ID);

    name_ = args[1]->token().name();
    expr_ = std::shared_ptr<Expression>(new ValueExpression(args[2]));
}

const Value& DefineExpression::eval(const EnvironmentPtr &env) const {
    Value value = expr_->eval(env);
    env->define(name_, value);
    return void_value;
}

const Value& VariableExpression::eval(const EnvironmentPtr &env) const {
    return env->get(name_);
}
