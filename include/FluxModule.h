#ifndef FLUX_MODULE_API_H
#define FLUX_MODULE_API_H

#ifdef _WIN32
#define FLUX_EXPORT __declspec(dllexport)
#else
#define FLUX_EXPORT __attribute__((visibility("default")))
#endif

#define FLUX_MODULE_API_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

// Required exports — each module DLL must define these:

// Return FLUX_MODULE_API_VERSION for compatibility checking
FLUX_EXPORT int flux_module_api_version(void);

// Return the canonical module name (e.g., "math")
FLUX_EXPORT const char* flux_module_name(void);

// Return how many functions this module exports
FLUX_EXPORT int flux_module_function_count(void);

// Get the i-th function's name, arity, and function pointer
// index: 0 .. function_count-1
FLUX_EXPORT void flux_module_get_function(int index,
                                          const char** out_name,
                                          int* out_arity,
                                          void** out_fn_ptr);

#ifdef __cplusplus
}
#endif

#endif
