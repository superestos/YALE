#pragma once

#include "expression/expression.h"

class EnvironmentManager;

class Environment {
friend class EnvironmentManager;

public:
    void define(const std::string &name, Value value);
    void set(const std::string &name, Value value);
    const Value& get(const std::string &name) const;
    bool existed(const std::string &name) const;

    //void set_enclosing(const EnvironmentPtr &enclosing);
    EnvironmentPtr get_enclosing();

private:
    bool local_existed(const std::string &name) const;
    void add_builtin();

    Environment(EnvironmentPtr enclosing): enclosing_{enclosing} {}
    Environment() {}

    std::map<std::string, Value> map_;
    EnvironmentPtr enclosing_;
    
};

enum EnvironmentConfig {
    ENV_EMPTY,
    ENV_BUILTIN,
};

class EnvironmentManager {
public:
    EnvironmentManager(EnvironmentConfig config = ENV_EMPTY);
    EnvironmentPtr global();
    static EnvironmentPtr create(EnvironmentPtr enclosing);

private:
    EnvironmentPtr global_;
};
