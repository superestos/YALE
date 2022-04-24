#include "expression/procedure.h"

#include <cassert>

ProcedurePtr SelfDefinedProcedure::create(ExpressionPtr expr, const std::vector<std::string>& names) {
    return std::shared_ptr<Procedure>(new SelfDefinedProcedure(expr, names));
}

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

Value BeginProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    Value value;
    for (auto &arg: args) {
        value = arg->eval(env);
    }
    return value;
}

Value SetProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    auto var = reinterpret_cast<VariableExpression*>(args[0].get());
    assert(var != nullptr);

    std::string name = var->name();
    env->set(name, args[1]->eval(env));
    return Value();
}

Value ConsProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    return Value({std::make_shared<Value>(args[0]->eval(env)), std::make_shared<Value>(args[1]->eval(env))});
}

Value CarProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 1);
    Value value = args[0]->eval(env);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[0];
}

Value CdrProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 1);
    Value value = args[0]->eval(env);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[1];
}

Value SelfDefinedProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == names_.size());
    EnvironmentPtr new_env = EnvironmentManager::create(env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]->eval(env));
    }

    Value value = expr_->eval(new_env);
    if (value.type() == VALUE_PROCEDURE) {
        value.env_ = new_env;
    }

    return value;
}
