#!/bin/bash

# Compiler setup
CXX=g++

# Compiler flags (C++20 standard and UTF-8 encoding)
CXXFLAGS="-std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8"

# Include directory path
INCLUDES="-I../include"

# Link libraries
LIBS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

# Common source files
COMMON_SRCS="../src/Lexer.cpp ../src/Parser.cpp"

echo "Building Flux VM..."
$CXX $CXXFLAGS $INCLUDES -DUSE_VM ../src/main.cpp $COMMON_SRCS ../src/Compiler.cpp ../src/VM.cpp $LIBS -o flux_vm

echo "Building Flux Interpreter..."
$CXX $CXXFLAGS $INCLUDES ../src/main.cpp $COMMON_SRCS ../src/Interpreter.cpp $LIBS -o flux_interpreter

echo "Build Successful!"