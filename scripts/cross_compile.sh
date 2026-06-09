#!/bin/bash
# Cross-compilation script for Flux (Linux/macOS to Windows)
# Requires: mingw-w64 (x86_64-w64-mingw32-gcc/g++)

set -e

BUILD_DIR="build_cross"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "--- Starting Cross-Compilation for Windows (x64) ---"

# Check for Mingw
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null
then
    echo "Error: x86_64-w64-mingw32-g++ not found. Please install mingw-w64."
    exit 1
fi

# Create a toolchain file for CMake
cat <<EOF > mingw_toolchain.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Run CMake
cmake -DCMAKE_TOOLCHAIN_FILE=mingw_toolchain.cmake ..

# Build
make -j$(nproc)

echo "--- Build Successful! ---"
echo "Binaries are located in: $BUILD_DIR"
ls -lh flux_vm.exe flux_interpreter.exe
