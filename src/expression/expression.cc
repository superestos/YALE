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

    if (args[1]->isCompound()) {
        auto& def = args[1]->children();
        assert(def[0]->token().type() == TOKEN_ID);
        name_ = def[0]->token().name();

        for (size_t i = 1; i < def.size(); i++) {
            assert(def[i]->token().type() == TOKEN_ID);
            arg_names_.emplace_back(def[i]->token().name());
        }

    } else {
        assert(args[1]->token().type() == TOKEN_ID);
        name_ = args[1]->token().name(); 

    }

    expr_ = Expression::create(args[2]);
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
