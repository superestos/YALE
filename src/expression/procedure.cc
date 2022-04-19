#include "expression/procedure.h"

#include <cassert>

std::pair<Value, Value> BinaryOperator::eval_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    std::pair<Value, Value> pair = {args[0]->eval(env), args[1]->eval(env)};
    assert(pair.first.type() == pair.second.type());
    return pair;
}

Value AddProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    Value left, right;
    std::tie(left, right) = this->eval_args(env, args);
    assert(left.type() == VALUE_NUM);
    return Value(left.num() + right.num());
}

Value EqualProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    Value left = args[0]->eval(env);
    Value right = args[1]->eval(env);
    assert(left.type() == right.type());

    if (left.type() == VALUE_NUM) {
        if (left.num() == right.num()) {
            return Value(1);
        } else {
            return Value(0);
        }
    } else if (left.type() == VALUE_QUOTE) {
        if (left.quote() == right.quote()) {
            return Value(1);
        } else {
            return Value(0);
        }
    }

    assert(false);
}

Value SmallProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    Value left = args[0]->eval(env);
    Value right = args[1]->eval(env);
    assert(left.type() == right.type());

    if (left.num() < right.num()) {
        return Value(1);
    } else {
        return Value(0);
    }

    assert(false);
}

Value IfProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 3);
    Value cond = args[0]->eval(env);
    assert(cond.type() == VALUE_NUM);

    return cond.num() != 0? args[1]->eval(env): args[2]->eval(env);
}

Value SelfDefinedProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == names_.size());
    EnvironmentPtr new_env = EnvironmentManager::create(env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]->eval(env));
    }

    return expr_->eval(new_env);
}