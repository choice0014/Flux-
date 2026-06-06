#ifndef FLUX_CHUNK_H
#define FLUX_CHUNK_H

#include <vector>
#include <cstdint>
#include "Value.h"

namespace Flux {
namespace Runtime {

class Chunk {
public:
    std::vector<uint8_t> code;           
    std::vector<Value> constants; 
    std::vector<int> lines;               

    void write(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }

    int addConstant(Value value) {
        constants.push_back(value);
        return (int)constants.size() - 1;
    }
};

} // namespace Runtime
} // namespace Flux

#endif
