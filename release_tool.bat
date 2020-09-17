SET RDIR=EffekseerToolWin

rmdir %RDIR%
mkdir %RDIR%

echo Compile Editor

echo Copy application

mkdir %RDIR%\Tool

copy Dev\release\Effekseer.exe %RDIR%\Tool\.
copy Dev\release\Effekseer.exe.config %RDIR%\Tool\.
copy Dev\release\EffekseerCore.dll %RDIR%\Tool\.
copy Dev\release\EffekseerViewer.dll %RDIR%\Tool\.
copy Dev\release\IronPython.dll %RDIR%\Tool\.
copy Dev\release\IronPython.Modules.dll %RDIR%\Tool\.
copy Dev\release\Microsoft.Dynamic.dll %RDIR%\Tool\.
copy Dev\release\Microsoft.Scripting.dll %RDIR%\Tool\.
copy Dev\release\EffekseerMaterialEditor.exe %RDIR%\Tool\.
copy Dev\release\Viewer.dll %RDIR%\Tool\.
copy Dev\release\Newtonsoft.Json.dll %RDIR%\Tool\.


mkdir %RDIR%\Tool\scripts
mkdir %RDIR%\Tool\scripts\export
copy Dev\release\scripts\export\Default.cs %RDIR%\Tool\scripts\export\.

mkdir %RDIR%\Tool\tools
copy Dev\release\tools\fbxToEffekseerModelConverter.exe %RDIR%\Tool\tools\.
copy Dev\release\tools\mqoToEffekseerModelConverter.exe %RDIR%\Tool\tools\.

copy Dev\release\tools\EffekseerMaterialCompilerDX9.dll %RDIR%\Tool\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerDX11.dll %RDIR%\Tool\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerDX12.dll %RDIR%\Tool\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerGL.dll %RDIR%\Tool\tools\.
copy Dev\release\tools\EffekseerMaterialCompilerMetal.dll %RDIR%\Tool\tools\.


mkdir %RDIR%\Tool\resources
robocopy Dev\release\resources\ %RDIR%\Tool\resources\. /s

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