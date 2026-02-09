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

python RTTRGenerator.py "..\public" "..\private\rttr_generated" :: || powershell -ExecutionPolicy Bypass -File "%~dp0RTTRGenerator.ps1" -InputDir "..\public" -OutputDir "..\private\rttr_generated"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========== [SUCCESS] RTTR generation completed!
) else (
    echo.
    echo ========== [ERROR] RTTR generation failed!
    exit /b 1
)
echo ========================================
