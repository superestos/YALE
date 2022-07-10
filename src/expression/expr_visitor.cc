#include "expression/expression.h"
#include "expression/expr_visitor.h"
#include "environment/environment.h"
#include "expression/procedure.h"


Value ExpressionVisitor::visitValueExpression(const ValueExpression &expr, const EnvironmentPtr &env) {
    return expr.value();
}

Value ExpressionVisitor::visitDefineExpression(const DefineExpression &expr, const EnvironmentPtr &env) {
    Value value = expr.arg_names().empty()? expr.def_expr()->eval(env): Value(Procedure::create(expr.def_expr(), expr.arg_names()));
    if (expr.is_define()) {
        env->define(expr.name(), value);
    } else {
        env->set(expr.name(), value);
    }
    return Value();
}

Value ExpressionVisitor::visitVariableExpression(const VariableExpression &expr, const EnvironmentPtr &env) {
    return env->get(expr.name());
}

Value ExpressionVisitor::visitApplyExpression(const ApplyExpression &expr, const EnvironmentPtr &env) {
    return expr.eval(env);
}

Value ExpressionVisitor::visitCondExpression(const CondExpression &expr, const EnvironmentPtr &env) {
    for (size_t i = 0; i < expr.length(); i++) {
        Value satisified = expr.conditions(i)->eval(env);
        assert(satisified.type() == VALUE_NUM);

        if (satisified.num() != 0) {
            return expr.exprs(i)->eval(env);
        }
    }

    assert(false);
    return Value();
}
