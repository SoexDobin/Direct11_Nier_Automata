@echo off

echo link lib

xcopy /Y /I /D "..\bin\Engine.lib"            "$(Solutiondir)EngineSDK\lib\"
xcopy /Y /I /D "..\..\EngineSDK\inc\*.h*"	  "$(Solutiondir)EngineSDK\inc\"
xcopy /Y /I /D "..\bin\Engine.dll"            "$(Solutiondir)Client\bin\"
xcopy /Y /I /D "..\bin\Engine.pdb"            "$(Solutiondir)Client\bin\"

:: xcopy /Y /I /D ".dll"							"$(SolutionDir)Tool\Bin\"
:: xcopy /Y /I /D ".pdb"							"$(SolutionDir)Tool\Bin\"