#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        std::cout << "Usage: flux <filename.fx>" << std::endl;
        return 0;
    }

    std::string filePath = argv[1];
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    file.close();

    try {
        Flux::Lexer lexer(code);
        auto tokens = lexer.tokenize();

        Flux::Parser parser(tokens);
        auto program = parser.parse();

        Flux::Interpreter interpreter;
        interpreter.interpret(*program);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
