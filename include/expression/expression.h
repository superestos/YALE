#include <string>

class Value {
    
};

class Expression {
    virtual const Value& evaluate() = 0;
};

typedef int Num;
typedef std::string Quote;

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
