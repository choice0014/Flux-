#ifndef FLUX_ENVIRONMENT_H
#define FLUX_ENVIRONMENT_H

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include "Value.h"

namespace Flux {

class Environment {
public:
    Environment(std::shared_ptr<Environment> parent = nullptr) : parent(parent) {}

    void define(const std::string& name, Runtime::Value value) {
        values[name] = value;
    }

    void assign(const std::string& name, Runtime::Value value) {
        if (values.count(name)) {
            values[name] = value;
            return;
        }
        if (parent) {
            parent->assign(name, value);
            return;
        }
        throw std::runtime_error("Undefined variable '" + name + "'.");
    }

    Runtime::Value get(const std::string& name) {
        if (values.count(name)) return values[name];
        if (parent) return parent->get(name);
        throw std::runtime_error("Undefined variable '" + name + "'.");
    }

private:
    std::shared_ptr<Environment> parent;
    std::map<std::string, Runtime::Value> values;
};

} // namespace Flux

#endif
