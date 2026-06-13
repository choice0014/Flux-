#ifndef FLUX_VALUE_H
#define FLUX_VALUE_H

#include <string>
#include <variant>
#include <memory>
#include <map>
#include <vector>

namespace Flux {
namespace Runtime {

struct ObjFunction;
class Chunk;

struct NativeFn;
struct MapValue;

using Value = std::variant<int, float, std::string, bool,
    std::shared_ptr<ObjFunction>,
    std::shared_ptr<NativeFn>,
    std::shared_ptr<MapValue>>;

struct ObjFunction {
    std::string name;
    int arity;
    std::shared_ptr<Chunk> chunk;
    ObjFunction() : arity(0), chunk(std::make_shared<Chunk>()) {}
};

struct NativeFn {
    std::string name;
    int arity;  // -1 means variadic
    Value (*fn)(const std::vector<Value>&);
};

struct MapValue {
    std::map<std::string, Value> entries;
};

inline std::string valueToString(const Value& val) {
    if (std::holds_alternative<int>(val)) return std::to_string(std::get<int>(val));
    if (std::holds_alternative<float>(val)) {
        std::string s = std::to_string(std::get<float>(val));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }
    if (std::holds_alternative<std::string>(val)) return std::get<std::string>(val);
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val) ? "true" : "false";
    if (std::holds_alternative<std::shared_ptr<NativeFn>>(val))
        return "<native fn: " + std::get<std::shared_ptr<NativeFn>>(val)->name + ">";
    if (std::holds_alternative<std::shared_ptr<MapValue>>(val))
        return "<module>";
    return "";
}

} // namespace Runtime
} // namespace Flux

#endif
