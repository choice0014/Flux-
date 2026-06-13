#ifndef FLUX_VM_H
#define FLUX_VM_H

#include "Chunk.h"
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

namespace Flux {

enum class InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

struct CallFrame {
    Runtime::Chunk* chunk;
    uint8_t* ip;
    Runtime::Value* slots; 
};

class VM {
public:
    VM();
    InterpretResult interpret(Runtime::Chunk* chunk);
    void callFluxFunction(const std::string& name);
    Runtime::Value callValue(Runtime::Value callee, const std::vector<Runtime::Value>& args);

    void push(Runtime::Value value) { *stackTop = value; stackTop++; }
    Runtime::Value pop() { stackTop--; return *stackTop; }
    Runtime::Value peek(int distance) { return *(stackTop - 1 - distance); }

private:
    static const int FRAMES_MAX = 64;
    static const int STACK_MAX = 256;

    CallFrame frames[FRAMES_MAX];
    int frameCount;

    Runtime::Value stack[STACK_MAX];
    Runtime::Value* stackTop;

    std::shared_ptr<std::map<std::string, Runtime::Value>> globals;
    std::unordered_set<std::string> loadedModules;

    uint8_t readByte() { return *frames[frameCount - 1].ip++; }
    Runtime::Value readConstant() { return frames[frameCount - 1].chunk->constants[readByte()]; }

    InterpretResult run();
    void printStackTrace();
    void registerBuiltins();
    void loadModule(const std::string& name);
};

} // namespace Flux

#endif

