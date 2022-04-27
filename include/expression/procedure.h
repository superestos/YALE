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

protected:
    std::pair<Value, Value> eval_args(const EnvironmentPtr &env, const std::vector<ExpressionPtr>& args) const;
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
def_procedure_class(Set)
def_procedure_class(Cons)
def_procedure_class(Car)
def_procedure_class(Cdr)

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
