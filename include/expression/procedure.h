#pragma once

#include <vector>

#include "expression/expression.h"
#include "environment/environment.h"

class Procedure {
public:
    virtual Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const = 0;

    template <typename T>
    static ProcedurePtr create();
};

class BinaryOperator : public Procedure {
public:
    virtual Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const = 0;
protected:
    std::pair<Value, Value> eval_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class AddProcedure : public BinaryOperator {
public:
    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class EqualProcedure : public BinaryOperator {
public:
    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class SmallProcedure : public BinaryOperator {
public:
    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class IfProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class SelfDefinedProcedure : public Procedure {
public:
    SelfDefinedProcedure(ExpressionPtr expr, const std::vector<std::string>& names):
        expr_{expr}, names_{names} {}

    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;

    static ProcedurePtr create(ExpressionPtr expr, const std::vector<std::string>& names);

private:
    ExpressionPtr expr_;
    std::vector<std::string> names_;
};
