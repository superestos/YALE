#include "environment/environment.h"

#include "expression/procedure.h"

void Environment::define(const std::string &name, Value value) {
    assert(!local_existed(name));
    map_[name] = value;
}

void Environment::set(const std::string &name, Value value) {
    assert(existed(name));
    if (local_existed(name)) {
        map_[name] = value;
    } else if (enclosing_.get() != nullptr) {
        enclosing_->set(name, value);
    }
}

const Value& Environment::get(const std::string &name) const {
    if (local_existed(name)) {
        return map_.at(name);
    } else if (enclosing_.get() != nullptr) {
        return enclosing_->get(name);
    }
    assert(false);
}

bool Environment::existed(const std::string &name) const {
    return local_existed(name) || (enclosing_.get() != nullptr && enclosing_->existed(name));
}

void Environment::set_enclosing(const EnvironmentPtr &enclosing) {
    if (enclosing.get() == enclosing_.get()) {
        return;
    }

    enclosing_ = enclosing;
}

bool Environment::local_existed(const std::string &name) const {
    return map_.count(name) > 0;
}

void Environment::add_builtin() {
    define("true", Value(1));
    define("false", Value(0));
    define("else", Value(1));

    define("+", Value(Procedure::create<AddProcedure>()));
    define("=", Value(Procedure::create<EqualProcedure>()));
    define("<", Value(Procedure::create<SmallProcedure>()));
    define("if", Value(Procedure::create<IfProcedure>()));
    define("begin", Value(Procedure::create<BeginProcedure>()));

    define("set!", Value(Procedure::create<SetProcedure>()));
    
    define("cons", Value(Procedure::create<ConsProcedure>()));
    define("car", Value(Procedure::create<CarProcedure>()));
    define("cdr", Value(Procedure::create<CdrProcedure>()));
    define("null?", Value(Procedure::create<NullProcedure>()));
    define("list", Value(Procedure::create<ListProcedure>()));
}

EnvironmentManager::EnvironmentManager(EnvironmentConfig config) {
    Environment *env = new Environment();
    global_ = std::shared_ptr<Environment>(env);

    if (config == ENV_BUILTIN) {
        global_->add_builtin();
    }
}

EnvironmentPtr EnvironmentManager::global() {
    return global_;
}

EnvironmentPtr EnvironmentManager::create(EnvironmentPtr enclosing) {
    Environment *env = new Environment(enclosing);
    return std::shared_ptr<Environment>(env);
}
