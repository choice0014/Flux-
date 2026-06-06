#!/bin/bash

# Compiler setup
CXX=g++

# Compiler flags (C++20 standard and UTF-8 encoding)
CXXFLAGS="-std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8"

# Include directory path
INCLUDES="-Iinclude"

# List of source files
SRCS="src/main.cpp src/Lexer.cpp src/Parser.cpp src/Interpreter.cpp"

# Output executable name
TARGET="flux_interpreter"

echo "Starting compilation..."

# Run the g++ command
$CXX $CXXFLAGS $SRCS $INCLUDES -o $TARGET

# Check if the build was successful
if [ $? -eq 0 ]; then
    echo "Build Successful! -> $TARGET"
else
    echo "Build Failed..."
    exit 1
fi