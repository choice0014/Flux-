@echo off
set "TARGET=%LOCALAPPDATA%\Flux"
echo Uninstalling Flux from %TARGET%...
echo.

:: Remove from PATH (we can't easily remove from setx, but we can remove the dir)
if exist "%TARGET%" (
    rmdir /s /q "%TARGET%"
    echo Removed %TARGET%
) else (
    echo Flux is not installed at %TARGET%
)

echo.
echo To remove from PATH manually:
echo   1. Open System Properties ^> Environment Variables
echo   2. Remove %%LOCALAPPDATA%%\Flux\bin from your PATH
echo.
echo Done.
pause
