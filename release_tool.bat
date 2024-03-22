SET RDIR=EffekseerTool

rmdir %RDIR%
mkdir %RDIR%

echo Compile Editor

echo Copy application

mkdir %RDIR%\Tool
mkdir %RDIR%\Tool\bin

robocopy Dev\release %RDIR%\Tool\bin

mkdir %RDIR%\Tool\bin\scripts
mkdir %RDIR%\Tool\bin\scripts\export
copy Dev\release\scripts\export\Default.cs %RDIR%\Tool\bin\scripts\export\.

mkdir %RDIR%\Tool\bin\tools
copy Dev\release\tools\fbxToEffekseerCurveConverter.exe %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\fbxToEffekseerModelConverter.exe %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\libfbxsdk.dll %RDIR%\Tool\bin\tools\.

copy Dev\release\tools\EffekseerMaterialCompilerDX9.dll %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerDX11.dll %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerDX12.dll %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerGL.dll %RDIR%\Tool\bin\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerMetal.dll %RDIR%\Tool\bin\tools\.


mkdir %RDIR%\Tool\bin\resources
robocopy Dev\release\resources\ %RDIR%\Tool\bin\resources\. /s

copy Tool\EffekseerLauncher\build_windows\Release\EffekseerLauncher.exe %RDIR%\Tool\Effekseer.exe

echo Sample

mkdir %RDIR%\Sample
robocopy Release\Sample %RDIR%\Sample *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S
robocopy ResourceData\samples %RDIR%\Sample *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S

echo Readme
copy readme_tool_win.txt %RDIR%\readme.txt
copy docs\readme_sample.txt %RDIR%\Sample\readme.txt

copy docs\Help_Ja.html %RDIR%\Help_Ja.html
copy docs\Help_En.html %RDIR%\Help_En.html
copy LICENSE_TOOL %RDIR%\LICENSE_TOOL


pause