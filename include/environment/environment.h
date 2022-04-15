#pragma once

#include "expression/expression.h"

class EnvironmentManager {
public:
    EnvironmentManager() {
        Environment *env = new Environment();
        global_ = std::shared_ptr<Environment>(env);
    }

    EnvironmentPtr global() {
        return global_;
    }

    EnvironmentPtr create(EnvironmentPtr enclosing) {
        Environment *env = new Environment(enclosing);
        return std::shared_ptr<Environment>(env);
    }

private:
    EnvironmentPtr global_;
};
