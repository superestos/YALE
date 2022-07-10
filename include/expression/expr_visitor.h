#pragma once

#include "expression/expression.h"

class ExpressionVisitor {
public:
    Value visitValueExpression(const ValueExpression &expr, const EnvironmentPtr &env);
    Value visitDefineExpression(const DefineExpression &expr, const EnvironmentPtr &env);
    Value visitVariableExpression(const VariableExpression &expr, const EnvironmentPtr &env);
    Value visitApplyExpression(const ApplyExpression &expr, const EnvironmentPtr &env);
    Value visitCondExpression(const CondExpression &expr, const EnvironmentPtr &env);
};
