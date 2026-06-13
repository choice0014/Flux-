#include "../../include/FluxModule.h"
#include "../../include/Value.h"
#include "../../include/Chunk.h"
#include <cmath>
#include <string>

using namespace Flux::Runtime;

// Module functions
static Value flux_sqrt(const std::vector<Value>& args) {
    double v = std::holds_alternative<int>(args[0]) ? (double)std::get<int>(args[0]) : (double)std::get<float>(args[0]);
    return (float)std::sqrt(v);
}

static Value flux_sin(const std::vector<Value>& args) {
    double v = std::holds_alternative<int>(args[0]) ? (double)std::get<int>(args[0]) : (double)std::get<float>(args[0]);
    return (float)std::sin(v);
}

static Value flux_cos(const std::vector<Value>& args) {
    double v = std::holds_alternative<int>(args[0]) ? (double)std::get<int>(args[0]) : (double)std::get<float>(args[0]);
    return (float)std::cos(v);
}

static Value flux_abs(const std::vector<Value>& args) {
    if (std::holds_alternative<int>(args[0])) {
        int v = std::get<int>(args[0]);
        return v < 0 ? -v : v;
    }
    float v = std::get<float>(args[0]);
    return v < 0 ? -v : v;
}

static Value flux_pow(const std::vector<Value>& args) {
    double base = std::holds_alternative<int>(args[0]) ? (double)std::get<int>(args[0]) : (double)std::get<float>(args[0]);
    double exp = std::holds_alternative<int>(args[1]) ? (double)std::get<int>(args[1]) : (double)std::get<float>(args[1]);
    return (float)std::pow(base, exp);
}

struct FuncEntry {
    const char* name;
    int arity;
    void* fnPtr;
};

static FuncEntry functions[] = {
    {"sqrt", 1, reinterpret_cast<void*>(flux_sqrt)},
    {"sin",  1, reinterpret_cast<void*>(flux_sin)},
    {"cos",  1, reinterpret_cast<void*>(flux_cos)},
    {"abs",  1, reinterpret_cast<void*>(flux_abs)},
    {"pow",  2, reinterpret_cast<void*>(flux_pow)},
};
static const int funcCount = sizeof(functions) / sizeof(functions[0]);

extern "C" {

FLUX_EXPORT int flux_module_api_version(void) {
    return FLUX_MODULE_API_VERSION;
}

FLUX_EXPORT const char* flux_module_name(void) {
    return "math";
}

FLUX_EXPORT int flux_module_function_count(void) {
    return funcCount;
}

FLUX_EXPORT void flux_module_get_function(int index,
                                           const char** out_name,
                                           int* out_arity,
                                           void** out_fn_ptr) {
    if (index >= 0 && index < funcCount) {
        *out_name  = functions[index].name;
        *out_arity = functions[index].arity;
        *out_fn_ptr = functions[index].fnPtr;
    }
}

} // extern "C"
