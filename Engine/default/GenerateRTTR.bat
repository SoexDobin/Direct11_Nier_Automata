@echo off
echo ========================================
echo RTTR Registration Code Generator
echo ========================================
echo.

set SCRIPT_PATH=%~dp0RTTRGenerator.ps1
set INPUT_DIR=%1
set OUTPUT_DIR=%2
echo ========== [INFO] Input: %INPUT_DIR%
echo ========== [INFO] Output: %OUTPUT_DIR%
echo.

powershell -ExecutionPolicy Bypass -File "%SCRIPT_PATH%" -InputDir "%INPUT_DIR%" -OutputDir "%OUTPUT_DIR%"
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========== [SUCCESS] RTTR generation completed!
) else (
    echo.
    echo ========== [ERROR] RTTR generation failed!
    exit /b 1
)
echo ========================================
