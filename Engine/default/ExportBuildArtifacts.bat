@echo off

echo.
echo ================== Link Engine ==================
echo Copying Engine build products to EngineSDK and Client directories
echo =================================================
echo.

echo ===== [1/4] Copying Engine.lib to EngineSDK\lib =====
xcopy /Y /I "..\bin\Engine.lib"				"..\..\EngineSDK\lib\"
xcopy /Y /I "..\..\vcpkg_installed\x64-windows\debug\lib\*.lib*"  "..\..\EngineSDK\lib\"
xcopy /Y /I "..\..\vcpkg_installed\x64-windows\lib\*.lib*"        "..\..\EngineSDK\lib\"
echo =================================================
echo.

echo ===== [2/4] Copying Engine headers to EngineSDK\inc ===== 
xcopy /Y /I		"..\public\*.h*"					"..\..\EngineSDK\inc\"
xcopy /Y /I		"..\private\*.inl"					"..\..\EngineSDK\inc\"
xcopy /Y /I		"..\bin\shaders\*.hlsli*"		"..\..\EngineSDK\inc\"
xcopy /Y /I		"..\bin\shaders\*.hlsl*"		"..\..\EngineSDK\inc\"
xcopy /Y /I /E "..\external\FMOD"					"..\..\EngineSDK\inc\"
xcopy /Y /I /E "..\..\vcpkg_installed\x64-windows\include" "..\..\EngineSDK\inc\"
echo =================================================
echo.

echo ===== [3/4] Copying Engine.dll to Client\bin ===== 
xcopy /Y /I "..\bin\Engine.dll"						"..\..\Client\bin\"
xcopy /Y /I "..\bin\Engine.dll"						"..\..\Editor\bin\"
xcopy /Y /I	"..\bin\shaders\*.hlsli*"				"..\..\Client\shaders\bin\"
xcopy /Y /I	"..\bin\shaders\*.hlsl*"				"..\..\Client\shaders\bin\"
xcopy /Y /I /E "..\external\FMOD\*.dll*"			"..\..\Client\bin\"
xcopy /Y /I /E "..\external\FMOD\*.dll*"			"..\..\Editor\bin\"
echo =================================================
echo.

echo ===== [4/4] Copying Engine.pdb to Client\bin =====
xcopy /Y /I "..\bin\Engine.pdb"            "..\..\Client\bin\"
xcopy /Y /I "..\bin\Engine.pdb"            "..\..\Editor\bin\"
echo =================================================
echo.

echo ============= Complete Link Engine =============
:: xcopy /Y /I /D ".dll"							"$(SolutionDir)Tool\Bin\"
:: xcopy /Y /I /D ".pdb"							"$(SolutionDir)Tool\Bin\"
