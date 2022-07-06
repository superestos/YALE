#pragma once

#include "expression/expression.h"

class ExpressionVisitor {
    Value visitValueExpression(DefineExpression &expr, const EnvironmentPtr &env) {
        return expr.eval(env);
    }

    Value visitDefineExpression(DefineExpression &expr, const EnvironmentPtr &env) {
        return expr.eval(env);
    }

    Value visitVariableExpression(VariableExpression &expr, const EnvironmentPtr &env) {
        return expr.eval(env);
    }

    Value visitApplyExpression(ApplyExpression &expr, const EnvironmentPtr &env) {
        return expr.eval(env);
    }

    Value visitCondExpression(CondExpression &expr, const EnvironmentPtr &env) {
        return expr.eval(env);
    }
};
