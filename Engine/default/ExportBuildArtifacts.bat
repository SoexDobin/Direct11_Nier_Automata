@echo off

echo.
echo ================== Link Engine ==================
echo Copying Engine build products to EngineSDK and Client directories
echo =================================================
echo.

echo ===== [1/4] Copying Engine.lib to EngineSDK\lib =====
xcopy /Y /I "..\bin\Engine.lib"            "..\..\EngineSDK\lib\"
echo =================================================
echo.

echo ===== [2/4] Copying Engine headers to EngineSDK\inc ===== 
xcopy /Y /I "..\public\*.h*"			   "..\..\EngineSDK\inc\"
echo =================================================
echo.

echo ===== [3/4] Copying Engine.dll to Client\bin ===== 
xcopy /Y /I "..\bin\Engine.dll"            "..\..\Client\bin\"
echo =================================================
echo.

echo ===== [4/4] Copying Engine.pdb to Client\bin =====
xcopy /Y /I "..\bin\Engine.pdb"            "..\..\Client\bin\"
echo =================================================
echo.

echo ============= Complete Link Engine =============
:: xcopy /Y /I /D ".dll"							"$(SolutionDir)Tool\Bin\"
:: xcopy /Y /I /D ".pdb"							"$(SolutionDir)Tool\Bin\"