@echo off

g++ -std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8 -Iinclude ../src/main.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/Compiler.cpp ../src/VM.cpp -o flux_interpreter.exe
pause