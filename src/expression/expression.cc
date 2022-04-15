#include "expression/expression.h"

#include "environment/environment.h"

Value void_value;

const Value& DefineExpression::eval(const EnvironmentPtr &env) const {
    Value value = expr_->eval(env);
    env->define(name_, value);
    return void_value;
}
