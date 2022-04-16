#include <vector>

#include "expression/expression.h"
#include "environment/environment.h"

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

class SelfDefinedProcedure : public Procedure {
public:
    SelfDefinedProcedure(ExpressionPtr expr, const std::vector<std::string>& names):
        expr_{expr}, names_{names} {}

    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
        assert(args.size() == names_.size());
        EnvironmentPtr new_env = EnvironmentManager::create(env);

        for (size_t i = 0; i < names_.size(); i++) {
            new_env->define(names_[i], args[i]);
        }

        expr_->eval(new_env);
    }

private:
    ExpressionPtr expr_;
    std::vector<std::string> names_;
};
