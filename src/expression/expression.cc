#include "expression/expression.h"

#include "expression/procedure.h"
#include "environment/environment.h"

ExpressionPtr Expression::create(const ParseTreePointer &parse_tree) {
    if (parse_tree->isCompound()) {
        assert(parse_tree->children().size() > 0);
        if (parse_tree->children()[0]->token().name() == "define") {
            return std::make_shared<DefineExpression>(parse_tree);
        } else if (parse_tree->children()[0]->token().type() == TOKEN_ID) {
            return std::make_shared<ApplyExpression>(parse_tree);
        }
    } else {
        if (parse_tree->token().type() == TOKEN_ID) {
            return std::make_shared<VariableExpression>(parse_tree);
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
        auto& children = parse_tree->children();
        assert(children.size() == 3);
        assert(children[0]->token().name() == "lambda");
        assert(children[1]->isCompound());

        std::vector<std::string> names;
        for (auto &arg: children) {
            assert(arg->token().type() == TOKEN_ID);
            names.emplace_back(arg->token().name());
        }

        auto expr = Expression::create(children[2]);
        value_ = Value(SelfDefinedProcedure::create(expr, names));
    }
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
        env->define(name_, SelfDefinedProcedure::create(expr_, arg_names_));
    }
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

ApplyExpression::ApplyExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto children = parse_tree->children();
    assert(children[0]->token().type() == TOKEN_ID);

    name_ = children[0]->token().name();
    for (size_t i = 1; i < children.size(); i++) {
        args_.emplace_back(Expression::create(children[i]));
    }
}

Value ApplyExpression::eval(const EnvironmentPtr &env) const {
    return env->get(name_).procedure()->call(env, args_);
}
