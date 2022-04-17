#include "expression/procedure.h"

#include <cassert>

Value AddProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    Num sum = 0;
    for (const Value& arg: args) {
        sum += arg.num();
    }

    return Value(sum);
}

Value EqualProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    assert(args.size() == 2);
    if (args[0].type() == VALUE_NUM) {
        assert(args[1].type() == VALUE_NUM);

        if (args[0].num() == args[1].num()) {
            return Value(1);
        } else {
            return Value(0);
        }
    } else if (args[0].type() == VALUE_QUOTE) {
        assert(args[1].type() == VALUE_QUOTE);

        if (args[0].quote() == args[1].quote()) {
            return Value(1);
        } else {
            return Value(0);
        }
    }

    assert(false);
}

Value IfProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    assert(args.size() == 3);
    assert(args[0].type() == VALUE_NUM);

    return args[0].num() != 0? args[1]: args[2];
}

Value SelfDefinedProcedure::call(const EnvironmentPtr &env, const std::vector<Value>& args) const {
    assert(args.size() == names_.size());
    EnvironmentPtr new_env = EnvironmentManager::create(env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]);
    }

    return expr_->eval(new_env);
}