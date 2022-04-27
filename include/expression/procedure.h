#pragma once

#include <vector>

#include "expression/expression.h"
#include "environment/environment.h"

class Procedure {
public:
    virtual Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const = 0;

    template <typename T>
    static ProcedurePtr create() {
        return std::shared_ptr<Procedure>(new T());
    }

    static ProcedurePtr create(ExpressionPtr expr, const std::vector<std::string>& names);

protected:
    std::pair<Value, Value> eval_binary_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
    std::vector<Value> eval_variant_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
};

class LambdaProcedure : public Procedure {
public:
    LambdaProcedure(ExpressionPtr expr, const std::vector<std::string>& names):
        expr_{expr}, names_{names} {}

    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;

private:
    ExpressionPtr expr_;
    std::vector<std::string> names_;
};


#define concat(x, y) x ## y

#define def_procedure_class(name) \
class concat(name, Procedure) : public Procedure { \
public: \
    Value call(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const; \
};

def_procedure_class(Add)
def_procedure_class(Equal)
def_procedure_class(Small)
def_procedure_class(If)
def_procedure_class(Begin)
def_procedure_class(Cons)
def_procedure_class(Null)
def_procedure_class(List)
def_procedure_class(Car)
def_procedure_class(Cdr)
