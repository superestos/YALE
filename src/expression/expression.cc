#include "expression/expression.h"

#include "expression/procedure.h"
#include "environment/environment.h"

ExpressionPtr Expression::create(const ParseTreePointer &parse_tree) {
    if (parse_tree->isCompound()) {
        assert(parse_tree->children().size() > 0);
        auto &children = parse_tree->children();

        if (!children[0]->isCompound()) {
            if (children[0]->token().name() == "define") {
                return std::make_shared<DefineExpression>(parse_tree);
            } else if (children[0]->token().name() == "lambda") {
                return std::make_shared<ValueExpression>(parse_tree);
            } else if (children[0]->token().name() == "cond") {
                return std::make_shared<CondExpression>(parse_tree);
            }
        }
        return std::make_shared<ApplyExpression>(parse_tree);

    } else {
        if (parse_tree->token().type() == TOKEN_ID) {
            return std::make_shared<VariableExpression>(parse_tree);
        } else {
            return std::make_shared<ValueExpression>(parse_tree);
        }
    }
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

const EnvironmentPtr Value::env() const {
    assert(type() == VALUE_PROCEDURE);
    return env_;
}

const Construct Value::cons() const {
    assert(type() == VALUE_CONSTRUCT);
    return cons_;
}

ValueExpression::ValueExpression(const ParseTreePointer &parse_tree) {
    if (!parse_tree->isCompound()) {
        Token token = parse_tree->token();
        if (token.type() == TOKEN_QUOTE) {
            value_ = Value(token.name().substr(1));
        } else if (token.type() == TOKEN_NUM) {
            value_ = Value(std::stoi(token.name()));
        }
    } else {
        value_ = this->parse_lambda(parse_tree);
    }
}

Value ValueExpression::parse_lambda(const ParseTreePointer &parse_tree) {
    auto& children = parse_tree->children();
    assert(children.size() == 3);
    assert(children[0]->token().name() == "lambda");
    assert(children[1]->isCompound());

    std::vector<std::string> names;
    auto args = children[1]->children();
    for (auto &arg: args) {
        assert(arg->token().type() == TOKEN_ID);
        names.emplace_back(arg->token().name());
    }

    auto expr = Expression::create(children[2]);
    return Value(Procedure::create(expr, names));
}

Value ValueExpression::eval(const EnvironmentPtr &env) const {
    return value_;
}

DefineExpression::DefineExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto& children = parse_tree->children();
    assert(children.size() == 3);
    assert(children[0]->token().name() == "define");

    if (children[1]->isCompound()) {
        auto& def = children[1]->children();
        assert(def[0]->token().type() == TOKEN_ID);
        name_ = def[0]->token().name();

        for (size_t i = 1; i < def.size(); i++) {
            assert(def[i]->token().type() == TOKEN_ID);
            arg_names_.emplace_back(def[i]->token().name());
        }

    } else {
        assert(children[1]->token().type() == TOKEN_ID);
        name_ = children[1]->token().name(); 

    }

    expr_ = Expression::create(children[2]);
}

Value DefineExpression::eval(const EnvironmentPtr &env) const {
    if (arg_names_.empty()) {
        env->define(name_, expr_->eval(env));
    } else {
        env->define(name_, Value(Procedure::create(expr_, arg_names_), env));
    }
    return Value();
}

VariableExpression::VariableExpression(const ParseTreePointer &parse_tree) {
    assert(!parse_tree->isCompound());
    assert(parse_tree->token().type() == TOKEN_ID);

    name_ = parse_tree->token().name();
}

const std::string VariableExpression::name() const {
    return name_;
}

Value VariableExpression::eval(const EnvironmentPtr &env) const {
    return env->get(name_);
}

ApplyExpression::ApplyExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto children = parse_tree->children();
    assert(children.size() > 0);

    function_ = Expression::create(children[0]);

    for (size_t i = 1; i < children.size(); i++) {
        args_.emplace_back(Expression::create(children[i]));
    }
}

Value ApplyExpression::eval(const EnvironmentPtr &env) const {
    Value value = function_->eval(env);
    assert(value.type() == VALUE_PROCEDURE);
    if (value.env().get() == nullptr) {
        return value.procedure()->call(env, args_);
    } else {
        //value.env()->set_enclosing(env);
        return value.procedure()->call(value.env(), args_);
    }
}

CondExpression::CondExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto& children = parse_tree->children();
    assert(children.size() > 1);
    assert(children[0]->token().name() == "cond");

    for (size_t i = 1; i < children.size(); i++) {
        auto& subexpr = children[i];
        assert(subexpr->isCompound());
        assert(subexpr->children().size() == 2);

        conditions_.emplace_back(Expression::create(subexpr->children()[0]));
        exprs_.emplace_back(Expression::create(subexpr->children()[1]));
    }
}

Value CondExpression::eval(const EnvironmentPtr &env) const {
    for (size_t i = 0; i < conditions_.size(); i++) {
        Value satisified = conditions_[i]->eval(env);
        assert(satisified.type() == VALUE_NUM);

        if (satisified.num() != 0) {
            return exprs_[i]->eval(env);
        }
    }

    assert(false);
    return Value();
}
