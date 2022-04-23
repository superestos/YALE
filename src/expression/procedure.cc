#include "expression/procedure.h"

#include <cassert>

ProcedurePtr SelfDefinedProcedure::create(ExpressionPtr expr, const std::vector<std::string>& names) {
    return std::shared_ptr<Procedure>(new SelfDefinedProcedure(expr, names));
}

std::pair<Value, Value> BinaryOperator::eval_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    std::pair<Value, Value> pair = {args[0]->eval(), args[1]->eval()};
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
    Value left = args[0]->eval();
    Value right = args[1]->eval();
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
    Value left = args[0]->eval();
    Value right = args[1]->eval();
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
    Value cond = args[0]->eval();
    assert(cond.type() == VALUE_NUM);

    return cond.num() != 0? args[1]->eval(): args[2]->eval();
}

Value BeginProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    Value value;
    for (auto &arg: args) {
        value = arg->eval();
    }
    return value;
}

Value SetProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    auto var = reinterpret_cast<VariableExpression*>(args[0].get());
    assert(var != nullptr);

    std::string name = var->name();
    env->set(name, args[1]->eval());
    return Value();
}

Value ConsProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    return Value({std::make_shared<Value>(args[0]->eval()), std::make_shared<Value>(args[1]->eval())});
}

Value CarProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 1);
    Value value = args[0]->eval();
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[0];
}

Value CdrProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 1);
    Value value = args[0]->eval();
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[1];
}

Value SelfDefinedProcedure::call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == names_.size());
    //EnvironmentPtr new_env = EnvironmentManager::create(env);

    auto new_env = EnvironmentManager::create(expr_->environment());
    auto new_expr = Expression::create(expr_, new_env);

    for (size_t i = 0; i < names_.size(); i++) {
        new_env->define(names_[i], args[i]->eval());
    }

    return new_expr->eval();

    // TODO: create new environment
    //return expr_->eval(new_env);  
    //return expr_->eval();  
}
