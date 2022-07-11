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

    void push_env() {
        env_ = env_manager_.create(env_);
    }

    void pop_env() {
        env_ = env_->get_enclosing();
    }

    EnvironmentPtr env() {
        return env_;
    }

    ExpressionVisitor(EnvironmentConfig config = ENV_BUILTIN):
        env_manager_{config}
    {
        env_ = env_manager_.global();
    }

private:
    EnvironmentManager env_manager_;
    EnvironmentPtr env_;
};
