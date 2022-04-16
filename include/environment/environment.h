#pragma once

#include "expression/expression.h"

class EnvironmentManager;

class Environment {
friend class EnvironmentManager;

public:
    void define(const std::string &name, Value value) {
        assert(!local_existed(name));
        map_[name] = value;
    }

    void set(const std::string &name, Value value) {
        assert(local_existed(name));
        map_[name] = value;
    }

    const Value& get(const std::string &name) {
        if (local_existed(name)) {
            return map_[name];
        } else if (enclosing_.get() != nullptr) {
            return enclosing_->get(name);
        }
        assert(false);
    }

    bool existed(const std::string &name) {
        return local_existed(name) || (enclosing_.get() != nullptr && enclosing_->existed(name));
    }

private:
    bool local_existed(const std::string &name) {
        return map_.count(name) > 0;
    }

    Environment(EnvironmentPtr enclosing): enclosing_{enclosing} {}

    Environment() {}

    std::map<std::string, Value> map_;
    EnvironmentPtr enclosing_;
};

class EnvironmentManager {
public:
    EnvironmentManager() {
        Environment *env = new Environment();
        global_ = std::shared_ptr<Environment>(env);
    }

    EnvironmentPtr global() {
        return global_;
    }

    static EnvironmentPtr create(EnvironmentPtr enclosing) {
        Environment *env = new Environment(enclosing);
        return std::shared_ptr<Environment>(env);
    }

private:
    EnvironmentPtr global_;
};
