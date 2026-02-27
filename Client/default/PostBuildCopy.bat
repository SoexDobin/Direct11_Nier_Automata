@echo off
setlocal

:: ==============================================================================
:: Client Post-Build Copy Script
:: ==============================================================================
::
:: Description:
:: Copies the built Client library (.lib) to the Editor and Launcher bin directories.
::
:: Arguments:
:: %1 - The Solution Directory ($(SolutionDir))
:: %2 - The Output Path of the built library ($(TargetPath))
:: %3 - The Target File Name ($(TargetFileName))
:: ==============================================================================

set SOLUTION_DIR=%~1
set SOURCE_PATH=%~2
set TARGET_NAME=%~3

echo [Client Post-Build] Starting copy process...
echo Solution Directory: %SOLUTION_DIR%
echo Source Path: %SOURCE_PATH%
echo Target Name: %TARGET_NAME%

:: Check if the source file exists
if not exist "%SOURCE_PATH%" (
    echo [Error] Source file not found: "%SOURCE_PATH%"
    exit /b 1
)

:: Ensure the target directories exist
if not exist "%SOLUTION_DIR%Editor\bin" (
    echo [Info] Creating Editor\bin directory...
    mkdir "%SOLUTION_DIR%Editor\bin"
)

if not exist "%SOLUTION_DIR%Launcher\bin" (
    echo [Info] Creating Launcher\bin directory...
    mkdir "%SOLUTION_DIR%Launcher\bin"
)

:: Copy to Editor\bin
echo [Client Post-Build] Copying to Editor\bin...
copy /Y "%SOURCE_PATH%" "%SOLUTION_DIR%Editor\bin\%TARGET_NAME%"
copy /Y "%SOURCE_PATH%" "%SOLUTION_DIR%Editor\bin\Client.pdb"
if %errorlevel% neq 0 (
    echo [Error] Failed to copy to Editor\bin.
    exit /b %errorlevel%
)

:: Copy to Launcher\bin
echo [Client Post-Build] Copying to Launcher\bin...
copy /Y "%SOURCE_PATH%" "%SOLUTION_DIR%Launcher\bin\%TARGET_NAME%"
copy /Y "%SOURCE_PATH%" "%SOLUTION_DIR%Launcher\bin\Client.pdb"
if %errorlevel% neq 0 (
    echo [Error] Failed to copy to Launcher\bin.
    exit /b %errorlevel%
)

echo [Client Post-Build] Copy completed successfully!
exit /b 0
