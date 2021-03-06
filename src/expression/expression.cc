#include "expression/expression.h"

#include "expression/procedure.h"
#include "environment/environment.h"
#include "expression/expr_visitor.h"

ExpressionPtr Expression::create(const ParseTreePointer &parse_tree) {
    if (parse_tree->isCompound()) {
        assert(parse_tree->children().size() > 0);
        auto &children = parse_tree->children();

        if (!children[0]->isCompound()) {
            if (children[0]->token().type() == TOKEN_DEFINE) {
                return std::make_shared<DefineExpression>(parse_tree);
            } else if (children[0]->token().type() == TOKEN_LAMBDA) {
                return std::make_shared<ValueExpression>(parse_tree);
            } else if (children[0]->token().type() == TOKEN_COND) {
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
    return std::get<Num>(val_);
}

const Quote& Value::quote() const {
    assert(type() == VALUE_QUOTE);
    return std::get<Quote>(val_);
}

const ProcedurePtr Value::procedure() const {
    assert(type() == VALUE_PROCEDURE);
    return std::get<Closure>(val_).first;
}

const EnvironmentPtr Value::env() const {
    assert(type() == VALUE_PROCEDURE);
    return std::get<Closure>(val_).second;
}

const Construct Value::cons() const {
    assert(type() == VALUE_CONSTRUCT);
    return std::get<Construct>(val_);
}

std::ostream& operator<<(std::ostream& out, const Value &value) {
    switch(value.type()) {
        case VALUE_VOID: return out << "<null>";
        case VALUE_NUM: return out << value.num();
        case VALUE_QUOTE: return out << "'" << value.quote();
        case VALUE_PROCEDURE: return out << "<procedure>";
        case VALUE_CONSTRUCT: return out << "(" << *(value.cons()[0]) << ", " << *(value.cons()[1]) << ")";
    }
    assert(false);
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

Value ValueExpression::accept(ExpressionVisitor &visitor) const {
    return visitor.visitValueExpression(*this);
}

DefineExpression::DefineExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto& children = parse_tree->children();
    assert(children.size() == 3);

    if (children[0]->token().name() == "define") {
        type_ = DEFINE;
    } else if (children[0]->token().name() == "set!") {
        type_ = SET;
    } else {
        assert(false);
    }

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

Value DefineExpression::accept(ExpressionVisitor &visitor) const {
    return visitor.visitDefineExpression(*this);
}

VariableExpression::VariableExpression(const ParseTreePointer &parse_tree) {
    assert(!parse_tree->isCompound());
    assert(parse_tree->token().type() == TOKEN_ID);

    name_ = parse_tree->token().name();
}

const std::string VariableExpression::name() const {
    return name_;
}

Value VariableExpression::accept(ExpressionVisitor &visitor) const {
    return visitor.visitVariableExpression(*this);
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

Value ApplyExpression::accept(ExpressionVisitor &visitor) const {
    return visitor.visitApplyExpression(*this);
}

CondExpression::CondExpression(const ParseTreePointer &parse_tree) {
    assert(parse_tree->isCompound());
    auto& children = parse_tree->children();
    assert(children.size() > 1);
    assert(children[0]->token().type() == TOKEN_COND);

    for (size_t i = 1; i < children.size(); i++) {
        auto& subexpr = children[i];
        assert(subexpr->isCompound());
        assert(subexpr->children().size() == 2);

        conditions_.emplace_back(Expression::create(subexpr->children()[0]));
        exprs_.emplace_back(Expression::create(subexpr->children()[1]));
    }
}

Value CondExpression::accept(ExpressionVisitor &visitor) const {
    return visitor.visitCondExpression(*this);
}
