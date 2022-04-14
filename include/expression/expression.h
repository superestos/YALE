#include <string>
#include <cassert>

typedef enum {
    VALUE_ERR,
    VALUE_NUM,
    VALUE_QUOTE,
    VALUE_FUNCTION,
} ValueType;

typedef int Num;
typedef std::string Quote;

class Expression {
    //virtual const Value& evaluate() = 0;
};

class Value {
public:
    Value(Num value): type_{VALUE_NUM}, num_{value} {}

    Value(Quote value): type_{VALUE_QUOTE}, quote_{value} {}

    Value(Expression value): type_{VALUE_FUNCTION}, function_{value} {}

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

    const Expression& function() const {
        assert(type() == VALUE_FUNCTION);
        return function_;
    }

private:
    ValueType type_;
    Num num_;
    Quote quote_;
    Expression function_;
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
