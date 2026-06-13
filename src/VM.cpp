#include "VM.h"
#include "OpCode.h"
#include "AST.h"
#include "FluxModule.h"
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Flux {

VM::VM() : frameCount(0), stackTop(stack), globals(std::make_shared<std::map<std::string, Runtime::Value>>()) {
    registerBuiltins();
}

void VM::registerBuiltins() {
    auto printFn = std::make_shared<Runtime::NativeFn>();
    printFn->name = "print";
    printFn->arity = 1;
    printFn->fn = [](const std::vector<Runtime::Value>& args) -> Runtime::Value {
        std::cout << Runtime::valueToString(args[0]) << std::endl;
        return 0;
    };
    (*globals)["print"] = printFn;
}

void VM::loadModule(const std::string& name) {
    if (loadedModules.count(name)) return;

#ifdef _WIN32
    std::string dllPath = "modules/" + name + ".dll";
    HMODULE hModule = LoadLibraryA(dllPath.c_str());
    if (!hModule) {
        throw std::runtime_error("Could not load module '" + name + "'.");
    }

    auto apiVersion = reinterpret_cast<int(*)()>(
        GetProcAddress(hModule, "flux_module_api_version"));
    auto moduleName = reinterpret_cast<const char*(*)()>(
        GetProcAddress(hModule, "flux_module_name"));
    auto funcCount = reinterpret_cast<int(*)()>(
        GetProcAddress(hModule, "flux_module_function_count"));
    auto getFunc = reinterpret_cast<void(*)(int, const char**, int*, void**)>(
        GetProcAddress(hModule, "flux_module_get_function"));

    if (!apiVersion || !moduleName || !funcCount || !getFunc) {
        FreeLibrary(hModule);
        throw std::runtime_error("Module '" + name + "' missing required exports.");
    }

    if (apiVersion() != FLUX_MODULE_API_VERSION) {
        FreeLibrary(hModule);
        throw std::runtime_error("Module '" + name + "' API version mismatch.");
    }

    int count = funcCount();
    auto mapVal = std::make_shared<Runtime::MapValue>();
    for (int i = 0; i < count; i++) {
        const char* fnName = nullptr;
        int fnArity = 0;
        void* fnPtr = nullptr;
        getFunc(i, &fnName, &fnArity, &fnPtr);

        auto nativeFn = std::make_shared<Runtime::NativeFn>();
        nativeFn->name = fnName ? fnName : "";
        nativeFn->arity = fnArity;
        nativeFn->fn = reinterpret_cast<Runtime::Value(*)(const std::vector<Runtime::Value>&)>(fnPtr);

        mapVal->entries[nativeFn->name] = nativeFn;
    }

    (*globals)[name] = mapVal;
    loadedModules.insert(name);
#else
    throw std::runtime_error("Module loading is only supported on Windows.");
#endif
}

InterpretResult VM::interpret(Runtime::Chunk* chunk) {
    frameCount = 0;
    stackTop = stack;
    
    CallFrame* frame = &frames[frameCount++];
    frame->chunk = chunk;
    frame->ip = chunk->code.data();
    frame->slots = stack;

    return run();
}

void VM::printStackTrace() {
    std::cerr << "Stack Trace:" << std::endl;
    for (int i = frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &frames[i];
        std::cerr << "  at [IP: " << (frame->ip - frame->chunk->code.data()) << "]" << std::endl;
    }
}

InterpretResult VM::run() {
    int initialFrameCount = frameCount;
    for (;;) {
        try {
            uint8_t instruction = readByte();
            switch (instruction) {
                case OP_CONSTANT: push(readConstant()); break;
                case OP_NULL:  push(0); break;
                case OP_TRUE:  push(true); break;
                case OP_FALSE: push(false); break;

                case OP_EQUAL: {
                    auto b = pop(); auto a = pop();
                    push(a == b); break;
                }
                case OP_GREATER: {
                    auto b = pop(); auto a = pop();
                    double v1 = std::holds_alternative<int>(a) ? (double)std::get<int>(a) : std::get<float>(a);
                    double v2 = std::holds_alternative<int>(b) ? (double)std::get<int>(b) : std::get<float>(b);
                    push(v1 > v2); break;
                }
                case OP_LESS: {
                    auto b = pop(); auto a = pop();
                    double v1 = std::holds_alternative<int>(a) ? (double)std::get<int>(a) : std::get<float>(a);
                    double v2 = std::holds_alternative<int>(b) ? (double)std::get<int>(b) : std::get<float>(b);
                    push(v1 < v2); break;
                }
                case OP_ADD: {
                    auto b = pop(); auto a = pop();
                    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) push(std::get<int>(a) + std::get<int>(b));
                    else if (std::holds_alternative<std::string>(a) || std::holds_alternative<std::string>(b)) push(Runtime::valueToString(a) + Runtime::valueToString(b));
                    else push((float)((std::holds_alternative<int>(a) ? (double)std::get<int>(a) : (double)std::get<float>(a)) + (std::holds_alternative<int>(b) ? (double)std::get<int>(b) : (double)std::get<float>(b))));
                    break;
                }
                case OP_SUBTRACT: {
                    auto b = pop(); auto a = pop();
                    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) push(std::get<int>(a) - std::get<int>(b));
                    else push((float)((std::holds_alternative<int>(a) ? (double)std::get<int>(a) : (double)std::get<float>(a)) - (std::holds_alternative<int>(b) ? (double)std::get<int>(b) : (double)std::get<float>(b))));
                    break;
                }
                case OP_MULTIPLY: {
                    auto b = pop(); auto a = pop();
                    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) push(std::get<int>(a) * std::get<int>(b));
                    else push((float)((std::holds_alternative<int>(a) ? (double)std::get<int>(a) : (double)std::get<float>(a)) * (std::holds_alternative<int>(b) ? (double)std::get<int>(b) : (double)std::get<float>(b))));
                    break;
                }
                case OP_DIVIDE: {
                    auto b = pop(); auto a = pop();
                    push((float)((std::holds_alternative<int>(a) ? (double)std::get<int>(a) : (double)std::get<float>(a)) / (std::holds_alternative<int>(b) ? (double)std::get<int>(b) : (double)std::get<float>(b))));
                    break;
                }
                case OP_NOT: {
                    auto val = pop();
                    if (std::holds_alternative<bool>(val)) push(!std::get<bool>(val));
                    else if (std::holds_alternative<int>(val)) push(std::get<int>(val) == 0);
                    break;
                }
                case OP_POP: pop(); break;

                case OP_DEFINE_GLOBAL: {
                    std::string name = std::get<std::string>(readConstant());
                    Runtime::Value val = pop();
                    (*globals)[name] = val;
                    break;
                }
                case OP_GET_GLOBAL: {
                    std::string name = std::get<std::string>(readConstant());
                    Runtime::Value result;
                    bool found = false;
                    if (globals->count(name)) {
                        result = (*globals)[name];
                        found = true;
                    }
                    if (found) push(result);
                    else throw std::runtime_error("Undefined variable '" + name + "'");
                    break;
                }
                case OP_SET_GLOBAL: {
                    std::string name = std::get<std::string>(readConstant());
                    Runtime::Value val = peek(0);
                    (*globals)[name] = val;
                    break;
                }
                case OP_GET_LOCAL: push(frames[frameCount - 1].slots[readByte()]); break;
                case OP_SET_LOCAL: frames[frameCount - 1].slots[readByte()] = peek(0); break;

                case OP_JUMP: {
                    uint16_t offset = (uint16_t)((readByte() << 8) | readByte());
                    frames[frameCount - 1].ip += offset; break;
                }
                case OP_JUMP_IF_FALSE: {
                    uint16_t offset = (uint16_t)((readByte() << 8) | readByte());
                    auto condition = peek(0);
                    bool isTrue = (std::holds_alternative<bool>(condition) && std::get<bool>(condition)) || (std::holds_alternative<int>(condition) && std::get<int>(condition) != 0);
                    if (!isTrue) frames[frameCount - 1].ip += offset;
                    break;
                }
                case OP_LOOP: {
                    uint16_t offset = (uint16_t)((readByte() << 8) | readByte());
                    frames[frameCount - 1].ip -= offset; break;
                }

                case OP_LOAD_MODULE: {
                    std::string moduleName = std::get<std::string>(readConstant());
                    loadModule(moduleName);
                    break;
                }

                case OP_GET_PROPERTY: {
                    std::string prop = std::get<std::string>(readConstant());
                    Runtime::Value obj = pop();
                    if (auto* mapPtr = std::get_if<std::shared_ptr<Runtime::MapValue>>(&obj)) {
                        auto& entries = (*mapPtr)->entries;
                        auto it = entries.find(prop);
                        if (it != entries.end()) {
                            push(it->second);
                        } else {
                            throw std::runtime_error("Module has no function '" + prop + "'");
                        }
                    } else {
                        throw std::runtime_error("Can only access properties on a module.");
                    }
                    break;
                }

                case OP_CALL: {
                    int argCount = readByte();
                    Runtime::Value callee = peek(argCount);

                    if (auto* funcPtr = std::get_if<std::shared_ptr<Runtime::ObjFunction>>(&callee)) {
                        auto func = *funcPtr;
                        if (func->arity != argCount) {
                            throw std::runtime_error("Function '" + func->name +
                                "' expects " + std::to_string(func->arity) +
                                " arguments, got " + std::to_string(argCount));
                        }
                        if (frameCount == FRAMES_MAX) throw std::runtime_error("Stack overflow.");
                        CallFrame* frame = &frames[frameCount++];
                        frame->chunk = func->chunk.get();
                        frame->ip = func->chunk->code.data();
                        frame->slots = stackTop - argCount - 1;

                    } else if (auto* nativePtr = std::get_if<std::shared_ptr<Runtime::NativeFn>>(&callee)) {
                        auto nativeFn = *nativePtr;
                        if (nativeFn->arity >= 0 && nativeFn->arity != argCount) {
                            throw std::runtime_error("Native function '" + nativeFn->name +
                                "' expects " + std::to_string(nativeFn->arity) +
                                " arguments, got " + std::to_string(argCount));
                        }
                        std::vector<Runtime::Value> args;
                        args.reserve(argCount);
                        for (int i = argCount - 1; i >= 0; i--) {
                            args.push_back(peek(i));
                        }
                        stackTop -= (argCount + 1);
                        Runtime::Value result = nativeFn->fn(args);
                        push(result);

                    } else {
                        throw std::runtime_error("Can only call functions.");
                    }
                    break;
                }

                case OP_RETURN: {
                    auto result = pop(); 
                    frameCount--;
                    if (frameCount < initialFrameCount) {
                         push(result); return InterpretResult::INTERPRET_OK;
                    }
                    if (frameCount == 0) return InterpretResult::INTERPRET_OK;
                    stackTop = frames[frameCount].slots; push(result); break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Runtime Error: " << e.what() << std::endl;
            printStackTrace();
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
    }
}

Runtime::Value VM::callValue(Runtime::Value callee, const std::vector<Runtime::Value>& args) {
    if (std::holds_alternative<std::shared_ptr<Runtime::ObjFunction>>(callee)) {
        auto func = std::get<std::shared_ptr<Runtime::ObjFunction>>(callee);
        Runtime::Value* savedStackTop = stackTop;
        push(callee);
        for (auto const& arg : args) push(arg);
        CallFrame* frame = &frames[frameCount++];
        frame->chunk = func->chunk.get();
        frame->ip = func->chunk->code.data();
        frame->slots = stackTop - args.size() - 1;
        run();
        Runtime::Value result = pop();
        stackTop = savedStackTop;
        return result;
    }
    return 0;
}

void VM::callFluxFunction(const std::string& name) {
    Runtime::Value val;
    bool found = false;
    if (globals->count(name)) {
        val = (*globals)[name];
        found = true;
    }
    if (found && std::holds_alternative<std::shared_ptr<Runtime::ObjFunction>>(val)) {
         auto func = std::get<std::shared_ptr<Runtime::ObjFunction>>(val);
         if (frameCount == FRAMES_MAX) return;
         CallFrame* frame = &frames[frameCount++];
         frame->chunk = func->chunk.get();
         frame->ip = func->chunk->code.data();
         frame->slots = stackTop;
         run();
    }
}

} // namespace Flux

