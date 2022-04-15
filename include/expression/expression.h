#pragma once

#include <string>
#include <map>
#include <memory>
#include <cassert>

#include "parser/parser.h"

typedef enum {
    VALUE_ERR,
    VALUE_NUM,
    VALUE_QUOTE,
    VALUE_FUNCTION,
} ValueType;

typedef int Num;
typedef std::string Quote;

class Value;
class Environment;
typedef std::shared_ptr<Environment> EnvironmentPtr;

class EnvironmentManager;

class Expression {
    virtual const Value& eval(const EnvironmentPtr &env) const = 0;
};

typedef std::shared_ptr<Expression> ExpressionPtr;

class Value {
public:
    Value(): type_{VALUE_ERR} {}

    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(ExpressionPtr value): type_{VALUE_FUNCTION}, function_{value} {}

    ValueType type() const {
        return type_;
    }

    Num num() const {
        assert(type() == VALUE_NUM);
        return num_;
    }

    const Quote& quote() const {
        assert(type() == VALUE_QUOTE);
        return quote_;
    }

    const ExpressionPtr function() const {
        assert(type() == VALUE_FUNCTION);
        return function_;
    }

private:
    ValueType type_;
    Num num_;
    Quote quote_;
    ExpressionPtr function_;
};

class Environment {
friend class EnvironmentManager;

public:
    void define(const std::string &name, Value value) {
        assert(!local_existed(name));
        map_[name] = value;
    }

    void set(const std::string &name, Value value) {
        assert(local_existed(name));
        map_[name] = value;
    }

    const Value& get(const std::string &name) {
        if (local_existed(name)) {
            return map_[name];
        } else if (enclosing_.get() != nullptr) {
            return enclosing_->get(name);
        }
        assert(false);
    }

    bool existed(const std::string &name) {
        return local_existed(name) || (enclosing_.get() != nullptr && enclosing_->existed(name));
    }

private:
    bool local_existed(const std::string &name) {
        return map_.count(name) > 0;
    }

    Environment(EnvironmentPtr enclosing): enclosing_{enclosing} {}

    Environment() {}

    std::map<std::string, Value> map_;
    EnvironmentPtr enclosing_;
};

class ValueExpression : public Expression {
public:
    ValueExpression(const ParseTreePointer parse_tree) {
        if (!parse_tree->isCompound()) {
            Token token = parse_tree->token();
            if (token.type() == TOKEN_QUOTE) {
                value_ = Value(token.name().substr(1));
            } else if (token.type() == TOKEN_NUM) {
                value_ = Value(std::stoi(token.name()));
            }
        }
    }

    const Value& eval(const EnvironmentPtr &env) const {
        return value_;
    }

private:
    Value value_;
};


/*
class NumValue {
public:
    NumValue(Num value): value_{value} {}

    Num operator() () {
        return value_;
    }

private:
    Num value_;
};

class QuoteValue {
public:
    QuoteValue(Quote value): value_{value} {}

    const Quote& operator() () {
        return value_;
    }

private:
    Quote value_;
};
*/
