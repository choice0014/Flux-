@echo off
setlocal enabledelayedexpansion

set "TARGET_BAT="

:: 1. Loop only to find the correct path (No 'call' inside the loop)
for %%V in (2026 18) do (
    for %%E in (Enterprise Professional Community) do (
        if not defined TARGET_BAT (
            if exist "%ProgramFiles%\Microsoft Visual Studio\%%V\%%E\Common7\Tools\VsDevCmd.bat" (
                set "TARGET_BAT=%ProgramFiles%\Microsoft Visual Studio\%%V\%%E\Common7\Tools\VsDevCmd.bat"
                echo [INFO] Found Visual Studio %%V %%E environment.
            )
        )
    )
)

:: Error handling using a safe jump
if not defined TARGET_BAT goto :NOT_FOUND

:: 2. Completely escape the parsing block and initialize the developer environment
echo [INFO] Loading developer environment...
endlocal & call "%TARGET_BAT%"

:: 3. Build project with C++20 and UTF-8 flags
echo [Building Flux VM...]
cl /std:c++20 /utf-8 /EHsc /I"../include" /DUSE_VM ../src/main.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/GUI.cpp ../src/Platform.cpp ../src/Compiler.cpp ../src/VM.cpp raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib /Feflux_vm.exe

echo [Building Flux Interpreter...]
cl /std:c++20 /utf-8 /EHsc /I"../include" ../src/main.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/GUI.cpp ../src/Platform.cpp ../src/Interpreter.cpp raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib /Feflux_interpreter.exe

pause
exit /b

:NOT_FOUND
echo [ERROR] Visual Studio environment (Version 18 to 2026) could not be found.
pause
exit /b