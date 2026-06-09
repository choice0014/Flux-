cmake_minimum_required(VERSION 3.15)
project(FluxLanguage VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory
include_directories(include)

# Source files
set(COMMON_SRCS
    src/Lexer.cpp
    src/Parser.cpp
    src/GUI.cpp
    src/Platform.cpp
)

set(VM_SRCS
    src/Compiler.cpp
    src/VM.cpp
)

set(INTERPRETER_SRCS
    src/Interpreter.cpp
)

# Main entry point
set(MAIN_SRC src/main.cpp)

# Options
option(BUILD_VM "Build with VM (Bytecode)" ON)
option(BUILD_INTERPRETER "Build with Interpreter (Tree-walking)" ON)

# Find Raylib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    # Fallback for systems where find_package might fail (like some WSL setups)
    # We'll look for raylib manually or use pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB raylib)
else()
    set(RAYLIB_LIBRARIES raylib)
endif()

# Determine platform-specific flags
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(PLATFORM_LIBS user32 raylib gdi32 winmm)
else()
    set(PLATFORM_LIBS ${RAYLIB_LIBRARIES} m pthread dl)
endif()

# Build targets
if(BUILD_VM)
    add_executable(flux_vm ${MAIN_SRC} ${COMMON_SRCS} ${VM_SRCS})
    target_compile_definitions(flux_vm PRIVATE USE_VM)
    target_link_libraries(flux_vm PRIVATE ${PLATFORM_LIBS})
endif()

if(BUILD_INTERPRETER)
    add_executable(flux_interpreter ${MAIN_SRC} ${COMMON_SRCS} ${INTERPRETER_SRCS})
    target_compile_definitions(flux_interpreter PRIVATE USE_INTERPRETER)
    target_link_libraries(flux_interpreter PRIVATE ${PLATFORM_LIBS})
endif()
