@echo off
setlocal EnableDelayedExpansion

rem ========================================
rem Engine Build Artifact Export Script
rem ========================================
rem This script copies build artifacts:
rem   - EngineSDK/Inc: Public headers
rem   - EngineSDK/Lib: Import library (Engine.lib)
rem   - Client Bin:    Runtime DLLs (Engine.dll, fmod.dll, etc.)
rem 
rem NOTE: vcpkg dependencies are statically linked into Engine.dll
rem       (Unity/Unreal style - no separate dependency DLLs needed)
rem ========================================
rem Arguments:
rem   %1 = Configuration (Debug/Release)
rem   %2 = Engine TargetDir (where Engine.dll & Engine.lib are built)
rem   %3 = Client BinDir (where to copy DLLs for runtime)
rem ========================================

set CONFIG=%~1
set ENGINE_DIR=%~2
set CLIENT_BIN=%~3
set SDK_DIR=%~dp0..\..\EngineSDK

echo.
echo ========================================
echo   Exporting Engine Build Artifacts
echo ========================================
echo Configuration: %CONFIG%
echo Engine Dir:    %ENGINE_DIR%
echo Client Bin:    %CLIENT_BIN%
echo SDK Dir:       %SDK_DIR%
echo ========================================
echo.

rem Create EngineSDK directories (Inc and Lib only - no Bin!)
if not exist "%SDK_DIR%\Inc\" mkdir "%SDK_DIR%\Inc\"
if not exist "%SDK_DIR%\Lib\" mkdir "%SDK_DIR%\Lib\"

rem Create client bin directory if it doesn't exist
if not exist "%CLIENT_BIN%" (
    echo Creating directory: %CLIENT_BIN%
    mkdir "%CLIENT_BIN%"
)

rem ========================================
rem PART A: Export to EngineSDK (Development)
rem ========================================
echo.
echo [PART A] Exporting SDK Files to EngineSDK
echo ========================================

rem ----------------------------------------
rem A-1. Export Public Headers
rem ----------------------------------------
echo [A-1] Exporting Public Headers to EngineSDK\Inc...
set PUBLIC_DIR=%~dp0..\Public

if exist "%PUBLIC_DIR%\*.h" (
    xcopy /Y /D "%PUBLIC_DIR%\*.h" "%SDK_DIR%\Inc\" >nul
    if errorlevel 1 goto :error
    
    rem Count copied files
    set /a count=0
    for %%f in ("%PUBLIC_DIR%\*.h") do set /a count+=1
    echo   - !count! header files exported
) else (
    echo   WARNING: No headers found in %PUBLIC_DIR%
)

rem ----------------------------------------
rem A-2. Export Engine Import Library
rem ----------------------------------------
echo [A-2] Exporting Engine.lib to EngineSDK\Lib...
if exist "%ENGINE_DIR%Engine.lib" (
    xcopy /Y /D "%ENGINE_DIR%Engine.lib" "%SDK_DIR%\Lib\" >nul
    if errorlevel 1 goto :error
    echo   - Engine.lib exported
) else (
    echo   WARNING: Engine.lib not found in "%ENGINE_DIR%"
)

rem ========================================
rem PART B: Copy Runtime Files to Client Bin
rem ========================================
echo.
echo [PART B] Copying Runtime Files to Client Bin
echo ========================================

rem ----------------------------------------
rem B-1. Copy Engine DLL and PDB
rem ----------------------------------------
echo [B-1] Copying Engine.dll and Engine.pdb...
if exist "%ENGINE_DIR%Engine.dll" (
    xcopy /Y /D "%ENGINE_DIR%Engine.dll" "%CLIENT_BIN%" >nul
    if errorlevel 1 goto :error
    echo   - Engine.dll copied
) else (
    echo   WARNING: Engine.dll not found in %ENGINE_DIR%
)

rem Copy PDB only for Debug configuration
if /I "%CONFIG%"=="Debug" (
    if exist "%ENGINE_DIR%Engine.pdb" (
        xcopy /Y /D "%ENGINE_DIR%Engine.pdb" "%CLIENT_BIN%" >nul
        echo   - Engine.pdb copied
    )
) else (
    echo   - Engine.pdb skipped (Release build)
)

rem ----------------------------------------
rem B-2. Copy FMOD DLL
rem ----------------------------------------
echo [B-2] Copying FMOD DLL...
set FMOD_DIR=%~dp0..\External\FMOD\Lib

if exist "%FMOD_DIR%\fmod.dll" (
    xcopy /Y /D "%FMOD_DIR%\fmod.dll" "%CLIENT_BIN%" >nul
    if errorlevel 1 goto :error
    echo   - fmod.dll copied
) else (
    echo   INFO: fmod.dll not found (skipping)
)

rem ----------------------------------------
rem B-3. vcpkg Dependencies (Static Linked)
rem ----------------------------------------
rem NOTE: vcpkg dependencies (assimp, DirectXTK, etc.) are statically
rem       linked into Engine.dll, so no separate DLLs need to be copied.
echo [B-3] vcpkg dependencies statically linked into Engine.dll (no copy needed)

rem ----------------------------------------
rem Success
rem ----------------------------------------
echo.
echo ========================================
echo   Build Artifact Export Completed!
echo ========================================
echo   SDK Location:
echo     - Inc: %SDK_DIR%\Inc (headers)
echo     - Lib: %SDK_DIR%\Lib (Engine.lib)
echo   Client Runtime:
echo     - %CLIENT_BIN% (DLLs)
echo ========================================
echo.
exit /b 0

rem ----------------------------------------
rem Error Handler
rem ----------------------------------------
:error
echo.
echo ========================================
echo   ERROR: Failed to export artifacts!
echo ========================================
echo.
exit /b 1
