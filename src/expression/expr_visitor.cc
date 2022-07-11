#include "expression/expression.h"
#include "expression/expr_visitor.h"
#include "environment/environment.h"
#include "expression/procedure.h"


Value ExpressionVisitor::visitValueExpression(const ValueExpression &expr) {
    return expr.value();
}

Value ExpressionVisitor::visitDefineExpression(const DefineExpression &expr) {
    Value value = expr.arg_names().empty()? expr.def_expr()->accept(*this): Value(Procedure::create(expr.def_expr(), expr.arg_names()));
    if (expr.is_define()) {
        env_->define(expr.name(), value);
    } else {
        env_->set(expr.name(), value);
    }
    return Value();
}

Value ExpressionVisitor::visitVariableExpression(const VariableExpression &expr) {
    return env_->get(expr.name());
}

Value ExpressionVisitor::visitApplyExpression(const ApplyExpression &expr) {
    Value func = expr.function()->accept(*this);
    assert(func.type() == VALUE_PROCEDURE);

    EnvironmentPtr restored_env = env_;
    if (func.env().get() != nullptr) {
        env_ = func.env();
    }
    
    Value result = func.procedure()->call(*this, expr.args());
    env_ = restored_env;
    return result;
}

Value ExpressionVisitor::visitCondExpression(const CondExpression &expr) {
    for (size_t i = 0; i < expr.length(); i++) {
        Value satisified = expr.condition(i)->accept(*this);
        assert(satisified.type() == VALUE_NUM);

        if (satisified.num() != 0) {
            return expr.expr(i)->accept(*this);
        }
    }

    assert(false);
    return Value();
}
