@echo off
setlocal enabledelayedexpansion

echo ========================================
echo RTTR Registration Code Generator
echo ========================================

:: Arguments from VS: %1=Input(public), %2=Output(rttr_generated)
set "INPUT_DIR=%~1"
set "OUTPUT_DIR=%~2"

:: Default fallback if args are empty (though VS should provide them)
if "%INPUT_DIR%"=="" set "INPUT_DIR=%~dp0..\public"
if "%OUTPUT_DIR%"=="" set "OUTPUT_DIR=%~dp0..\private\rttr_generated"

echo [INFO] Input Dir:  %INPUT_DIR%
echo [INFO] Output Dir: %OUTPUT_DIR%
echo.

:: Try multiple python aliases (Prioritize vcpkg Python)
set "PYTHON_CMD="

:: 0. Try vcpkg's internal python (Recommended)
if exist "C:\vcpkg\installed\x64-windows\tools\python3\python.exe" (
    set "PYTHON_CMD=C:\vcpkg\installed\x64-windows\tools\python3\python.exe"
) else (
    :: 1. Try 'python' in PATH
    python --version >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        set "PYTHON_CMD=python"
    ) else (
        :: 2. Try 'py -3' alias
        py -3 --version >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "PYTHON_CMD=py -3"
        ) else (
            :: 3. Search in common installation paths (Local AppData)
            for /d %%p in ("%LOCALAPPDATA%\Programs\Python\Python*") do (
                if exist "%%p\python.exe" (
                    set "PYTHON_CMD=%%p\python.exe"
                )
            )
            
            :: 4. Search in Program Files
            if "!PYTHON_CMD!"=="" (
                for /d %%p in ("%ProgramFiles%\Python*") do (
                    if exist "%%p\python.exe" (
                        set "PYTHON_CMD=%%p\python.exe"
                    )
                )
            )
        )
    )
)

if "!PYTHON_CMD!"=="" (
    echo [ERROR] RTTR Generator: Python was NOT found in your system.
    echo -------------------------------------------------------------------------
    echo This script requires Python to be installed and added to your PATH.
    echo 1. Please install Python from https://www.python.org/
    echo 2. Ensure 'Add Python to PATH' is checked during installation.
    echo 3. Or, manually add your Python installation folder to System Environment Variables.
    echo -------------------------------------------------------------------------
    exit /b 1
)

echo [INFO] RTTR Generator using: !PYTHON_CMD!
"%PYTHON_CMD%" "%~dp0RTTRGenerator.py" "%INPUT_DIR%" "%OUTPUT_DIR%"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========== [SUCCESS] RTTR generation completed!
    exit /b 0
) else (
    echo.
    echo ========== [ERROR] RTTR generation failed with code %ERRORLEVEL%
    exit /b 1
)
