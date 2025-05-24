SET RDIR=EffekseerTool



rem setup directories

if exist "%RDIR%" rmdir /s /q "%RDIR%"

set "BIN_DIR=%RDIR%\Tool\bin"

set "SCRIPT_DIR=%BIN_DIR%\scripts\export"

set "TOOLS_DIR=%BIN_DIR%\tools"

set "RES_DIR=%BIN_DIR%\resources"



mkdir "%SCRIPT_DIR%"

mkdir "%TOOLS_DIR%"

mkdir "%RES_DIR%"



echo Compile Editor



echo Copy application

robocopy Dev\release "%BIN_DIR%"

copy Dev\release\scripts\export\Default.cs "%SCRIPT_DIR%\"



for %%F in (fbxToEffekseerCurveConverter.exe fbxToEffekseerModelConverter.exe libfbxsdk.dll) do (

    copy "Dev\release\tools\%%F" "%TOOLS_DIR%\"

)

for %%C in (DX9 DX11 DX12 GL Metal) do (

    copy "Dev\release\tools\EffekseerMaterialCompiler%%C.dll" "%TOOLS_DIR%\"

)



robocopy Dev\release\resources "%RES_DIR%" /s

copy Tool\EffekseerLauncher\build_windows\Release\EffekseerLauncher.exe "%RDIR%\Tool\Effekseer.exe"



echo Sample
mkdir "%RDIR%\Sample"
robocopy Release\Sample "%RDIR%\Sample" *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S
robocopy ResourceData\samples "%RDIR%\Sample" *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S
echo Readme
copy readme_tool_win.txt "%RDIR%\readme.txt"
copy docs\readme_sample.txt "%RDIR%\Sample\readme.txt"
copy docs\Help_Ja.html "%RDIR%\Help_Ja.html"
copy docs\Help_En.html "%RDIR%\Help_En.html"
copy LICENSE_TOOL "%RDIR%\LICENSE_TOOL"
pause
