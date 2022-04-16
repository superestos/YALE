#include "expression.h"

class Procedure {
public:
    virtual Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const = 0;
};

class AddProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
        Num sum = 0;
        for (const Value& arg: args) {
            sum += arg.num();
        }

        return Value(sum);
    }
};
