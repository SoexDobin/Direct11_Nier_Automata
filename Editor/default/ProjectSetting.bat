@echo off
set "SourceDir=%~1\Data"
set "DestDir=%~2Data"

echo [Post-Build] Copying Data...
echo Source: "%SourceDir%"
echo Dest:   "%DestDir%"

if not exist "%SourceDir%" (
    echo [Warning] Source Data directory not found: "%SourceDir%"
    exit /b 0
)

if not exist "%DestDir%" (
    echo [Info] Creating destination directory...
    mkdir "%DestDir%"
)

xcopy /s /y /d "%SourceDir%" "%DestDir%"

if %errorlevel% neq 0 (
    echo [Error] Failed to copy Data folder. Error Code: %errorlevel%
    exit /b %errorlevel%
)

echo [Success] Data copy complete.
exit /b 0
