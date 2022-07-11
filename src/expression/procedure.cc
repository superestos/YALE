#include "expression/procedure.h"

#include <cassert>

ProcedurePtr Procedure::create(ExpressionPtr expr, const std::vector<std::string>& names) {
    return std::shared_ptr<Procedure>(new LambdaProcedure(expr, names));
}

std::pair<Value, Value> Procedure::eval_binary_args(ExpressionVisitor &visitor, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == 2);
    std::pair<Value, Value> pair = {args[0]->accept(visitor), args[1]->accept(visitor)};
    assert(pair.first.type() == pair.second.type());
    return pair;
}

std::vector<Value> Procedure::eval_variant_args(ExpressionVisitor &visitor, const std::vector<ExpressionPtr>& args) const {
    std::vector<Value> result;
    for (auto& arg: args) {
        result.emplace_back(arg->accept(visitor));
    }
    return result;
}

#define def_procedure_call(name) Value concat(name, Procedure)::call(ExpressionVisitor &visitor, const std::vector<ExpressionPtr>& args) const

def_procedure_call(Add) {
    auto nums = this->eval_variant_args(visitor, args);
    Num sum = 0;
    for (auto &n: nums) {
        assert(n.type() == VALUE_NUM);
        sum += n.num();
    }
    return Value(sum);
}

def_procedure_call(Equal) {
    Value left, right;
    std::tie(left, right) = this->eval_binary_args(visitor, args);
    assert(left.type() == right.type());

    if (left.type() == VALUE_NUM) {
        if (left.num() == right.num()) {
            return Value(1);
        } else {
            return Value(0);
        }
    } else if (left.type() == VALUE_QUOTE) {
        if (left.quote() == right.quote()) {
            return Value(1);
        } else {
            return Value(0);
        }
    }

    assert(false);
}

def_procedure_call(Small) {
    Value left, right;
    std::tie(left, right) = this->eval_binary_args(visitor, args);
    assert(left.type() == right.type());

    if (left.num() < right.num()) {
        return Value(1);
    } else {
        return Value(0);
    }

    assert(false);
}

def_procedure_call(If) {
    assert(args.size() == 3);
    Value cond = args[0]->accept(visitor);
    assert(cond.type() == VALUE_NUM);

    return cond.num() != 0? args[1]->accept(visitor): args[2]->accept(visitor);
}

def_procedure_call(Begin) {
    return args.empty()? Value(): this->eval_variant_args(visitor, args)[args.size() - 1];
}

def_procedure_call(Cons) {
    assert(args.size() == 2);
    return Value({std::make_shared<Value>(args[0]->accept(visitor)), std::make_shared<Value>(args[1]->accept(visitor))});
}

def_procedure_call(Null) {
    assert(args.size() == 1);
    return args[0]->accept(visitor).type() == VALUE_VOID;
}

def_procedure_call(List) {
    auto values = this->eval_variant_args(visitor, args);
    auto result = std::make_shared<Value>();

    for (auto it = values.crbegin(); it != values.crend(); ++it) {
        Construct cons = {std::make_shared<Value>(*it), result};
        result = std::make_shared<Value>(cons);
    }
    
    return Value(*result);
}

def_procedure_call(Car) {
    assert(args.size() == 1);
    Value value = args[0]->accept(visitor);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[0];
}

def_procedure_call(Cdr) {
    assert(args.size() == 1);
    Value value = args[0]->accept(visitor);
    assert(value.type() == VALUE_CONSTRUCT);
    return *value.cons()[1];
}

Value LambdaProcedure::call(ExpressionVisitor &visitor, const std::vector<ExpressionPtr>& args) const {
    assert(args.size() == names_.size());
    //EnvironmentPtr new_env = EnvironmentManager::create(env);
    visitor.push_env();

    for (size_t i = 0; i < names_.size(); i++) {
        visitor.env()->define(names_[i], args[i]->accept(visitor));
    }

    Value value = expr_->accept(visitor);

    if (value.type() == VALUE_PROCEDURE) {
        return Value(value.procedure(), visitor.env());
    }
    visitor.pop_env();

    return value;
}
