#ifndef FLUX_OPCODE_H
#define FLUX_OPCODE_H

#include <cstdint>

namespace Flux {

enum OpCode : uint8_t {
    OP_CONSTANT,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,

    OP_POP,

    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    
    OP_GET_LOCAL,
    OP_SET_LOCAL,

    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,

    OP_CALL,
    OP_RETURN,

    OP_LOAD_MODULE,
    OP_GET_PROPERTY,
};

} // namespace Flux

#endif
