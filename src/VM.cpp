#include "VM.h"
#include "OpCode.h"
#include "AST.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <filesystem>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#pragma comment(lib, "user32.lib")

namespace Flux { class VM; }
static Flux::VM* g_activeVM = nullptr;
static std::map<int, std::string> g_buttonCallbacks;
static int g_nextControlId = 1000;
static HWND g_hWnd = NULL;

static std::wstring utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

static LRESULT CALLBACK FluxWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND: {
            int id = LOWORD(wp);
            if (HIWORD(wp) == BN_CLICKED && g_buttonCallbacks.count(id) && g_activeVM) {
                g_activeVM->callFluxFunction(g_buttonCallbacks[id]);
            }
            break;
        }
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
#endif

namespace Flux {

VM::VM() : frameCount(0), stackTop(stack) {
#ifdef _WIN32
    g_activeVM = this;
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

InterpretResult VM::run() {
    for (;;) {
        uint8_t instruction = readByte();
        switch (instruction) {
            case OP_CONSTANT: push(readConstant()); break;
            case OP_NULL:  push(std::shared_ptr<Runtime::Object>(nullptr)); break;
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
                double v2 = std::holds_alternative<int>(b) ? (double)std::get<int>(b) : (double)std::get<float>(b);
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
            case OP_PRINT: std::cout << Runtime::valueToString(pop()) << std::endl; break;
            case OP_POP: pop(); break;

            case OP_DEFINE_GLOBAL: {
                std::string name = std::get<std::string>(readConstant());
                globals[name] = pop(); break;
            }
            case OP_GET_GLOBAL: {
                std::string name = std::get<std::string>(readConstant());
                if (globals.count(name)) push(globals[name]);
                else if (name.find('.') != std::string::npos) {
                    size_t dot = name.find('.');
                    std::string objName = name.substr(0, dot);
                    std::string memName = name.substr(dot+1);
                    if (globals.count(objName) && std::holds_alternative<std::shared_ptr<Runtime::Object>>(globals[objName])) {
                        push(std::get<std::shared_ptr<Runtime::Object>>(globals[objName])->members[memName]);
                    } else push(name); 
                } else throw std::runtime_error("Undefined variable '" + name + "'");
                break;
            }
            case OP_SET_GLOBAL: {
                std::string name = std::get<std::string>(readConstant());
                if (globals.count(name)) globals[name] = peek(0);
                else throw std::runtime_error("Undefined variable '" + name + "'");
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
            case OP_CALL: {
                int argCount = readByte();
                auto callee = peek(argCount);
                if (std::holds_alternative<std::shared_ptr<Runtime::ObjFunction>>(callee)) {
                    auto func = std::get<std::shared_ptr<Runtime::ObjFunction>>(callee);
                    if (frameCount == FRAMES_MAX) throw std::runtime_error("Stack overflow.");
                    CallFrame* frame = &frames[frameCount++];
                    frame->chunk = func->chunk.get();
                    frame->ip = func->chunk->code.data();
                    frame->slots = stackTop - argCount - 1;
                } else if (std::holds_alternative<std::string>(callee)) {
                    std::string fullName = std::get<std::string>(callee);
                    if (fullName == "print") {
                         std::cout << Runtime::valueToString(peek(0)) << std::endl;
                         pop(); pop(); push(0);
                    } else if (fullName.find('.') != std::string::npos) {
                        size_t dot = fullName.find('.');
                        std::string objName = fullName.substr(0, dot);
                        std::string subName = fullName.substr(dot+1);
                        if (globals.count(objName)) {
                            auto val = globals[objName];
                            if (std::holds_alternative<std::shared_ptr<Runtime::Object>>(val)) {
                                auto obj = std::get<std::shared_ptr<Runtime::Object>>(val);
                                std::string methodFullName = obj->typeName + "." + subName;
                                if (globals.count(methodFullName)) {
                                    auto funcVal = globals[methodFullName];
                                    if (std::holds_alternative<std::shared_ptr<Runtime::ObjFunction>>(funcVal)) {
                                        auto func = std::get<std::shared_ptr<Runtime::ObjFunction>>(funcVal);
                                        *(stackTop - argCount - 1) = obj;
                                        CallFrame* frame = &frames[frameCount++];
                                        frame->chunk = func->chunk.get();
                                        frame->ip = func->chunk->code.data();
                                        frame->slots = stackTop - argCount - 1;
                                        break;
                                    }
                                }
                            }
                        }
                        // Handle built-in modules
                        if (objName == "gui") { /* ... */ }
                        pop(); push(0);
                    }
                } else throw std::runtime_error("Can only call functions.");
                break;
            }
            case OP_NEW: {
                std::string name = std::get<std::string>(readConstant());
                auto obj = std::make_shared<Runtime::Object>(); obj->typeName = name;
                if (structs.count(name)) {
                    for (auto& m : structs[name]->members) {
                        if (m.type == "int") obj->members[m.name] = 0; else if (m.type == "float") obj->members[m.name] = 0.0f;
                        else if (m.type == "bool") obj->members[m.name] = false; else obj->members[m.name] = std::string("");
                    }
                } else if (classes.count(name)) {
                    for (auto& p : classes[name]->properties) {
                        for (auto& decl : p->decls) {
                             if (p->type == "int") obj->members[decl->name] = 0; else if (p->type == "float") obj->members[decl->name] = 0.0f;
                             else if (p->type == "bool") obj->members[decl->name] = false; else obj->members[decl->name] = std::string("");
                        }
                    }
                }
                push(obj); break;
            }
            case OP_GET_PROPERTY: {
                auto objVal = pop(); std::string name = std::get<std::string>(readConstant());
                if (std::holds_alternative<std::shared_ptr<Runtime::Object>>(objVal)) push(std::get<std::shared_ptr<Runtime::Object>>(objVal)->members[name]);
                else throw std::runtime_error("Property access on non-object.");
                break;
            }
            case OP_SET_PROPERTY: {
                auto val = pop(); auto objVal = pop(); std::string name = std::get<std::string>(readConstant());
                if (std::holds_alternative<std::shared_ptr<Runtime::Object>>(objVal)) { std::get<std::shared_ptr<Runtime::Object>>(objVal)->members[name] = val; push(val); }
                else throw std::runtime_error("Property assignment on non-object.");
                break;
            }
            case OP_DEFINE_STRUCT: {
                std::string name = std::get<std::string>(readConstant()); structs[name] = (AST::StructDef*)std::get<void*>(readConstant()); break;
            }
            case OP_DEFINE_CLASS: {
                std::string name = std::get<std::string>(readConstant()); classes[name] = (AST::ClassDef*)std::get<void*>(readConstant()); break;
            }
            case OP_RETURN: {
                auto result = pop(); frameCount--;
                if (frameCount == 0) return InterpretResult::INTERPRET_OK;
                stackTop = frames[frameCount].slots; push(result); break;
            }
        }
    }
}

void VM::callFluxFunction(const std::string& name) {
    if (globals.count(name)) {
        auto val = globals[name];
        if (std::holds_alternative<std::shared_ptr<Runtime::ObjFunction>>(val)) {
             auto func = std::get<std::shared_ptr<Runtime::ObjFunction>>(val);
             if (frameCount == FRAMES_MAX) return;
             CallFrame* frame = &frames[frameCount++];
             frame->chunk = func->chunk.get();
             frame->ip = func->chunk->code.data();
             frame->slots = stackTop;
             run();
        }
    }
}

} // namespace Flux
