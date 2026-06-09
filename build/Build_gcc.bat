@echo off

echo [Building Flux VM...]
g++ -std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8 -I../include -DUSE_VM ../src/main.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/GUI.cpp ../src/Platform.cpp ../src/Compiler.cpp ../src/VM.cpp -o flux_vm.exe -lraylib -lgdi32 -lwinmm

echo [Building Flux Interpreter...]
g++ -std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8 -I../include ../src/main.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/GUI.cpp ../src/Platform.cpp ../src/Interpreter.cpp -o flux_interpreter.exe -lraylib -lgdi32 -lwinmm

echo Build Complete.
pause