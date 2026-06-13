@echo off
setlocal enabledelayedexpansion

echo ============================================
echo  Flux Language Installer v1.0
echo ============================================
echo.

:: Target directory: %LOCALAPPDATA%\Flux
set "TARGET=%LOCALAPPDATA%\Flux"
set "BIN_DIR=%TARGET%\bin"
set "MODULES_DIR=%TARGET%\modules"

echo Installing to: %TARGET%
echo.

:: Create directories
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%MODULES_DIR%" mkdir "%MODULES_DIR%"

:: Copy executables
echo Copying flux.exe...
copy /Y "%~dp0flux.exe" "%BIN_DIR%\flux.exe" >nul
echo Copying flux-pkg.exe...
copy /Y "%~dp0flux-pkg.exe" "%BIN_DIR%\flux-pkg.exe" >nul

:: Verify
if not exist "%BIN_DIR%\flux.exe" (
    echo Error: flux.exe not found. Run build.bat first.
    pause
    exit /b 1
)

echo.
echo Flux installed successfully!
echo.

:: Add to PATH
echo %PATH% | findstr /I /C:"%BIN_DIR%" >nul
if %errorlevel% equ 0 (
    echo Already in PATH: %BIN_DIR%
) else (
    echo Adding to PATH...
    setx PATH "%PATH%;%BIN_DIR%"
    echo.
    echo NOTE: You may need to restart your terminal for PATH changes.
)

echo.
echo ============================================
echo  Usage:
echo    flux script.fx       - Run a Flux script
echo    flux-pkg list         - List installed modules
echo    flux-pkg install math - Install math module
echo ============================================
echo.

pause
