@echo off
echo ========================================
echo Client RTTR Registration Code Generator
echo ========================================
echo.

set INPUT_DIR=..\public
set OUTPUT_FILE=..\private\Client_RTTR.cpp

python ClientRTTRGenerator.py "%INPUT_DIR%" "%OUTPUT_FILE%" || powershell -ExecutionPolicy Bypass -File "%~dp0ClientRTTRGenerator.ps1" -InputDir "%INPUT_DIR%" -OutputFile "%OUTPUT_FILE%"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========== [SUCCESS] Client RTTR generation completed!
) else (
    echo.
    echo ========== [ERROR] Client RTTR generation failed!
    exit /b 1
)
echo ========================================
