#include "expression/procedure.h"

Value AddProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    Num sum = 0;
    for (const Value& arg: args) {
        sum += arg.num();
    }

    return Value(sum);
}

Value SelfDefinedProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    assert(args.size() == names_.size());
    EnvironmentPtr new_env = EnvironmentManager::create(env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]);
    }

    return expr_->eval(new_env);
}