# 🤖 AI Agent Build Instructions for Flux

This project is designed to be cross-platform and easily buildable by AI agents (Gemini, Claude, GPT, etc.). Follow these instructions to compile Flux on any environment.

## 📋 Prerequisites
- **CMake** (3.15+)
- **C++20 Compiler** (GCC 10+, Clang 10+, or MSVC 2019+)

## 🛠️ Local Build (Windows, Linux, macOS)
Run the following commands in the project root:

```bash
mkdir build
cd build
cmake ..
# On Windows (MSVC):
cmake --build . --config Release
# On Linux/macOS:
make -j4
```

## 🌍 Cross-Compilation (Linux to Windows)
If you are an AI agent running on a Linux environment and need to produce a Windows `.exe`:

1.  Ensure `mingw-w64` is installed.
2.  Run the provided script:
    ```bash
    bash scripts/cross_compile.sh
    ```

## 🧩 Build Options
You can toggle the execution engines via CMake:
- `-DBUILD_VM=ON/OFF` (Default: ON) - Bytecode VM version.
- `-DBUILD_INTERPRETER=ON/OFF` (Default: ON) - Tree-walking interpreter version (includes Win32 GUI support on Windows).

## 🚀 Deployment for Other Agents
If you are passing this project to another AI agent, simply tell them:
> "This project uses CMake. Use `cmake ..` in a build folder to generate the build files for your platform. See `scripts/AI_BUILD_GUIDE.md` for details."
