#include "expression/expression.h"

#include "expression/procedure.h"
#include "environment/environment.h"

ExpressionPtr Expression::create(const ParseTreePointer &parse_tree) {
    if (parse_tree->isCompound()) {
        assert(parse_tree->children().size() > 0);
        if (parse_tree->children()[0]->token().name() == "define") {
            return std::make_shared<DefineExpression>(parse_tree);
        } else if (parse_tree->children()[0]->token().type() == TOKEN_ID) {
            return std::make_shared<DynamicApplyExpression>(parse_tree);
        }
    } else {
        if (parse_tree->token().type() == TOKEN_ID) {
            return std::make_shared<ValueExpression>(parse_tree);
        }
    }

    return std::make_shared<ValueExpression>(parse_tree);
}

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

const ProcedurePtr Value::procedure() const {
    assert(type() == VALUE_PROCEDURE);
    return procedure_;
}

ValueExpression::ValueExpression(const ParseTreePointer &parse_tree) {
    if (!parse_tree->isCompound()) {
        Token token = parse_tree->token();
        if (token.type() == TOKEN_QUOTE) {
            value_ = Value(token.name().substr(1));
        } else if (token.type() == TOKEN_NUM) {
            value_ = Value(std::stoi(token.name()));
        }
    }
}

Value ValueExpression::eval(const EnvironmentPtr &env) const {
    return value_;
}

DefineExpression::DefineExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto& args = parse_tree->children();
    assert(args.size() == 3);
    assert(args[0]->token().name() == "define");
    assert(args[1]->token().type() == TOKEN_ID);

    name_ = args[1]->token().name();
    expr_ = std::shared_ptr<Expression>(new ValueExpression(args[2]));
}

Value DefineExpression::eval(const EnvironmentPtr &env) const {
    Value value = expr_->eval(env);
    env->define(name_, value);
    return Value();
}

VariableExpression::VariableExpression(const ParseTreePointer &parse_tree) {
    assert(!parse_tree->isCompound());
    assert(parse_tree->token().type() == TOKEN_ID);

    name_ = parse_tree->token().name();
}

Value VariableExpression::eval(const EnvironmentPtr &env) const {
    return env->get(name_);
}

/*
Value ApplyExpression::eval(const EnvironmentPtr &env) const {
    return procedure_->call(env, args_);
}
*/

DynamicApplyExpression::DynamicApplyExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto children = parse_tree->children();
    assert(children[0]->token().type() == TOKEN_ID);

    name_ = children[0]->token().name();
    for (size_t i = 1; i < children.size(); i++) {
        args_.emplace_back(Expression::create(children[i]));
    }
}

Value DynamicApplyExpression::eval(const EnvironmentPtr &env) const {
    return env->get(name_).procedure()->call(env, args_);
}
