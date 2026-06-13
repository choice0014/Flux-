@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"

REM Build Flux interpreter
cl /std:c++20 /utf-8 /EHsc /Iinclude src/main.cpp src/Lexer.cpp src/Parser.cpp src/Compiler.cpp src/VM.cpp /Feflux.exe

REM Build Flux package manager
cl /std:c++20 /utf-8 /EHsc /Iinclude flux-pkg/main.cpp /Feflux-pkg.exe /link urlmon.lib winhttp.lib

echo.
echo Done. flux.exe and flux-pkg.exe built.
