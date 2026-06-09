#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Version.h"
#include "Lexer.h"
#include "Parser.h"

#ifdef USE_VM
#include "Compiler.h"
#include "VM.h"
#else
#include "Interpreter.h"
#endif

void printHelp() {
    std::cout <<
        "Flux Programming Language\n"
        "\n"
        "Usage:\n"
        "  flux <file.fx>        Run a Flux script\n"
        "  flux --help           Show this help message\n"
        "  flux --version        Show version information\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string arg = argv[1];

    if (arg == "--help" || arg == "-h") {
        printHelp();
        return 0;
    }

    if (arg == "--version" || arg == "-v") {
        std::cout << "Flux " << APP_VERSION << std::endl;
        return 0;
    }

    std::string filePath = arg;
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

#ifdef USE_VM
        Flux::Runtime::Chunk chunk;
        Flux::Compiler compiler;
        compiler.compile(*program, &chunk);

        Flux::VM vm;
        vm.interpret(&chunk);
#else
        Flux::Interpreter interpreter;
        interpreter.interpret(*program);
#endif
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}