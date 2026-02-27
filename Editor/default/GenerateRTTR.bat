@echo off

echo ========================================
echo [Editor] Prefab Metadata Generator
echo ========================================
echo.

set SCRIPT_DIR=%~dp0
set CLIENT_PUBLIC=%~dp0..\..\Client\public
set OUTPUT_DIR=%~dp0..\private\

echo ========== [INFO] Script: %SCRIPT_DIR%HeaderParser.ps1
echo ========== [INFO] Input (Client): %CLIENT_PUBLIC%
echo ========== [INFO] Output: %OUTPUT_DIR%client_metadata.json
echo.

powershell -ExecutionPolicy Bypass -File "%SCRIPT_DIR%HeaderParser.ps1" -InputDir "%CLIENT_PUBLIC%" -OutputJson "%OUTPUT_DIR%client_metadata.json"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========== [SUCCESS] Editor metadata generation completed!
) else (
    echo.
    echo ========== [ERROR] Editor metadata generation failed!
    exit /b 1
)
echo ========================================
