#pragma once

#include <vector>

#include "expression/expression.h"
#include "environment/environment.h"

class Procedure {
public:
    virtual Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const = 0;
};

class AddProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const;
};

class EqualProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const;
};

class SmallProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const;
};

class IfProcedure : public Procedure {
public:
    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const;
};

class SelfDefinedProcedure : public Procedure {
public:
    SelfDefinedProcedure(ExpressionPtr expr, const std::vector<std::string>& names):
        expr_{expr}, names_{names} {}

    Value call(const EnvironmentPtr &env, const std::vector<Value>& args) const;

private:
    ExpressionPtr expr_;
    std::vector<std::string> names_;
};
