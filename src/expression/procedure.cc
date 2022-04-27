#include "expression/procedure.h"

#include <cassert>

ProcedurePtr Procedure::create(ExpressionPtr expr, const std::vector<std::string>& names) {
    return std::shared_ptr<Procedure>(new LambdaProcedure(expr, names));
}

std::pair<Value, Value> Procedure::eval_binary_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    std::pair<Value, Value> pair = {args[0]->eval(env), args[1]->eval(env)};
    assert(pair.first.type() == pair.second.type());
    return pair;
}

std::vector<Value> Procedure::eval_variant_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    std::vector<Value> result;
    for (auto& arg: args) {
        result.emplace_back(arg->eval(env));
    }
    return result;
}

#define def_procedure_call(name) Value concat(name, Procedure)::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const

def_procedure_call(Add) {
    auto nums = this->eval_variant_args(env, args);
    Num sum = 0;
    for (auto &n: nums) {
        assert(n.type() == VALUE_NUM);
        sum += n.num();
    }
    return Value(sum);
}

def_procedure_call(Equal) {
    Value left, right;
    std::tie(left, right) = this->eval_binary_args(env, args);
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

def_procedure_call(Small) {
    Value left, right;
    std::tie(left, right) = this->eval_binary_args(env, args);
    assert(left.type() == right.type());

    if (left.num() < right.num()) {
        return Value(1);
    } else {
        return Value(0);
    }

    assert(false);
}

def_procedure_call(If) {
    assert(args.size() == 3);
    Value cond = args[0]->eval(env);
    assert(cond.type() == VALUE_NUM);

    return cond.num() != 0? args[1]->eval(env): args[2]->eval(env);
}

def_procedure_call(Begin) {
    return args.empty()? Value(): this->eval_variant_args(env, args)[args.size() - 1];
}

def_procedure_call(Set) {
    assert(args.size() == 2);
    auto var = reinterpret_cast<VariableExpression*>(args[0].get());
    assert(var != nullptr);

    std::string name = var->name();
    env->set(name, args[1]->eval(env));
    return Value();
}

def_procedure_call(Cons) {
    assert(args.size() == 2);
    return Value({std::make_shared<Value>(args[0]->eval(env)), std::make_shared<Value>(args[1]->eval(env))});
}

def_procedure_call(Car) {
    assert(args.size() == 1);
    Value value = args[0]->eval(env);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[0];
}

def_procedure_call(Cdr) {
    assert(args.size() == 1);
    Value value = args[0]->eval(env);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[1];
}

Value LambdaProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == names_.size());
    EnvironmentPtr new_env = EnvironmentManager::create(env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]->eval(env));
    }

    Value value = expr_->eval(new_env);
    if (value.type() == VALUE_PROCEDURE) {
        return Value(value.procedure(), new_env);
    }

    return value;
}
