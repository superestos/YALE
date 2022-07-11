#pragma once

#include "expression/expression.h"
#include "environment/environment.h"

class ExpressionVisitor {
public:
    Value visitValueExpression(const ValueExpression &expr);
    Value visitDefineExpression(const DefineExpression &expr);
    Value visitVariableExpression(const VariableExpression &expr);
    Value visitApplyExpression(const ApplyExpression &expr);
    Value visitCondExpression(const CondExpression &expr);

    void push_env();
    void pop_env();

    ExpressionVisitor(EnvironmentConfig config = ENV_BUILTIN):
        env_manager_{ENV_BUILTIN}
    {
        env_ = env_manager_.global();
    }

private:
    EnvironmentManager env_manager_;
    EnvironmentPtr env_;
};
