#include "expression/expression.h"

#include "expression/procedure.h"
#include "environment/environment.h"

ExpressionPtr Expression::create(const ParseTreePointer &parse_tree, const EnvironmentPtr &env) {
    if (parse_tree->isCompound()) {
        assert(parse_tree->children().size() > 0);
        auto &children = parse_tree->children();

        if (!children[0]->isCompound()) {
            if (children[0]->token().name() == "define") {
                return std::make_shared<DefineExpression>(parse_tree, env);
            } else if (children[0]->token().name() == "lambda") {
                return std::make_shared<ValueExpression>(parse_tree, env);
            }
        }
        return std::make_shared<ApplyExpression>(parse_tree, env);

    } else {
        if (parse_tree->token().type() == TOKEN_ID) {
            return std::make_shared<VariableExpression>(parse_tree, env);
        } else {
            return std::make_shared<ValueExpression>(parse_tree, env);
        }
    }
}

ExpressionPtr Expression::create(const ExpressionPtr &expr, const EnvironmentPtr &env) {
    ExpressionPtr result;

    if (expr->type_ == EXPR_VALUE) {
        auto ptr = reinterpret_cast<ValueExpression*>(expr.get());
        result = std::make_shared<ValueExpression>(*ptr);
    } else if (expr->type_ == EXPR_APPLY) {
        auto ptr = reinterpret_cast<ApplyExpression*>(expr.get());
        result = std::make_shared<ApplyExpression>(*ptr);
    } else if (expr->type_ == EXPR_DEFINE) {
        auto ptr = reinterpret_cast<DefineExpression*>(expr.get());
        result = std::make_shared<DefineExpression>(*ptr);
    } else if (expr->type_ == EXPR_VAR) {
        auto ptr = reinterpret_cast<VariableExpression*>(expr.get());
        result = std::make_shared<VariableExpression>(*ptr);
    }

    assert(result.get() != nullptr);
    result->env_ = env;
    return result;
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

ValueExpression::ValueExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env) {
    this->env_ = env;
    this->type_ = EXPR_VALUE;

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

    auto expr = Expression::create(children[2], this->env_);
    return Value(SelfDefinedProcedure::create(expr, names));
}

Value ValueExpression::eval() const {
    return value_;
}

DefineExpression::DefineExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env) {
    this->env_ = env;
    this->type_ = EXPR_DEFINE;

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

    expr_ = Expression::create(children[2], env);
}

Value DefineExpression::eval() const {
    if (arg_names_.empty()) {
        this->env_->define(name_, expr_->eval());
    } else {
        this->env_->define(name_, SelfDefinedProcedure::create(expr_, arg_names_));
    }
    return Value();
}

VariableExpression::VariableExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env) {
    this->env_ = env;
    this->type_ = EXPR_VAR;

    assert(!parse_tree->isCompound());
    assert(parse_tree->token().type() == TOKEN_ID);

    name_ = parse_tree->token().name();
}

const std::string VariableExpression::name() const {
    return name_;
}

Value VariableExpression::eval() const {
    return this->env_->get(name_);
}

ApplyExpression::ApplyExpression(const ParseTreePointer &parse_tree, const EnvironmentPtr &env) {
    this->env_ = env;
    this->type_ = EXPR_APPLY;

    assert(parse_tree->isCompound());
    auto children = parse_tree->children();
    assert(children.size() > 0);

    function_ = Expression::create(children[0], this->env_);

    for (size_t i = 1; i < children.size(); i++) {
        args_.emplace_back(Expression::create(children[i], this->env_));
    }
}

Value ApplyExpression::eval() const {
    return function_->eval().procedure()->call(this->env_, args_);
}
