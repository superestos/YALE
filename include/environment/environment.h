#include <map>
#include <memory>

#include "expression/expression.h"

class Environment;
typedef std::shared_ptr<Environment> EnvironmentPtr;

class Environment {
public:
    Environment() {}

    Environment(EnvironmentPtr enclosing): enclosing_{enclosing} {}

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

    std::map<std::string, Value> map_;
    EnvironmentPtr enclosing_;
};
