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

const Value& DefineExpression::eval(const EnvironmentPtr &env) const {
    Value value = expr_->eval(env);
    env->define(name_, value);
    return void_value;
}
