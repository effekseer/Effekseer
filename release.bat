SET RDIR=Effekseer150RC1
SET RDIR_R=EffekseerRuntime150RC1

rmdir %RDIR%
mkdir %RDIR%

rmdir %RDIR_R%
mkdir %RDIR_R%

echo Compile Editor

mkdir build_release
cd build_release

cmake -A x64 -D BUILD_VIEWER=ON ../
cmake --build . --config Release
cd ..

"C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild" Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86 /t:Rebuild
"C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild" Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86 /t:Rebuild

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
copy Dev\release\Viewer.dll %RDIR%\Tool\.

mkdir %RDIR%\Tool\scripts
mkdir %RDIR%\Tool\scripts\export
copy Dev\release\scripts\export\Default.cs %RDIR%\Tool\scripts\export\.

mkdir %RDIR%\Tool\tools
copy Dev\release\tools\fbxToEffekseerModelConverter.exe %RDIR%\Tool\tools\.
copy Dev\release\tools\mqoToEffekseerModelConverter.exe %RDIR%\Tool\tools\.

mkdir %RDIR%\Tool\resources
robocopy Dev\release\resources\ %RDIR%\Tool\resources\. /s


echo Copy runtime
mkdir %RDIR_R%\Examples\

mkdir %RDIR_R%\src
mkdir %RDIR_R%\src\include
mkdir %RDIR_R%\src\lib

copy Dev\Cpp\Effekseer\Effekseer.h %RDIR_R%\src\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR_R%\src\include\.
copy Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h %RDIR_R%\src\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR_R%\src\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR_R%\src\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR_R%\src\include\.

copy Dev\Cpp\EffekseerRendererDX9.sln %RDIR_R%\src\.
copy Dev\Cpp\EffekseerRendererDX11.sln %RDIR_R%\src\.
copy Dev\Cpp\EffekseerRendererGL.sln %RDIR_R%\src\.

mkdir %RDIR_R%\src\Effekseer
robocopy Dev\Cpp\Effekseer %RDIR_R%\src\Effekseer *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererDX9
robocopy Dev\Cpp\EffekseerRendererDX9 %RDIR_R%\src\EffekseerRendererDX9 *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererDX11
robocopy Dev\Cpp\EffekseerRendererDX11 %RDIR_R%\src\EffekseerRendererDX11 *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererDX12
robocopy Dev\Cpp\EffekseerRendererDX12 %RDIR_R%\src\EffekseerRendererDX12 *.h *.cpp *.fx *.bat CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererGL
robocopy Dev\Cpp\EffekseerRendererGL %RDIR_R%\src\EffekseerRendererGL *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererLLGI
robocopy Dev\Cpp\EffekseerRendererLLGI %RDIR_R%\src\EffekseerRendererLLGI *.h *.cpp /S

mkdir %RDIR_R%\src\EffekseerRendererCommon
robocopy Dev\Cpp\EffekseerRendererCommon %RDIR_R%\src\EffekseerRendererCommon *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererMetal
robocopy Dev\Cpp\EffekseerRendererMetal %RDIR_R%\src\EffekseerRendererMetal *.h *.cpp *.mm CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerRendererVulkan
robocopy Dev\Cpp\EffekseerRendererVulkan %RDIR_R%\src\EffekseerRendererVulkan *.h *.cpp *.vert *.frag *.inl *.py CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerSoundXAudio2
robocopy Dev\Cpp\EffekseerSoundXAudio2 %RDIR_R%\src\EffekseerSoundXAudio2 *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerSoundAL
robocopy Dev\Cpp\EffekseerSoundAL %RDIR_R%\src\EffekseerSoundAL *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerSoundDSound
robocopy Dev\Cpp\EffekseerSoundDSound %RDIR_R%\src\EffekseerSoundDSound *.h *.cpp *.filters *.vcxproj *.fx CMakeLists.txt /S

mkdir %RDIR_R%\src\EffekseerMaterialCompiler
robocopy Dev\Cpp\EffekseerMaterialCompiler %RDIR_R%\src\EffekseerMaterialCompiler *.h *.cpp CMakeLists.txt /S

mkdir %RDIR_R%\src\3rdParty\LLGI
robocopy Dev\Cpp\3rdParty\LLGI %RDIR_R%\src\3rdParty\LLGI /S

mkdir %RDIR_R%\src\3rdParty\stb_effekseer
robocopy Dev\Cpp\3rdParty\stb_effekseer %RDIR_R%\src\3rdParty\stb_effekseer *.h

echo Samples for runtimes
robocopy Examples %RDIR_R%\Examples\ *.h *.cpp *.mm *.txt *.fx *.efk *.wav *.png /S

copy Dev\Cpp\CMakeLists.txt %RDIR_R%\src\.

echo Sample

mkdir %RDIR%\Sample
robocopy Release\Sample %RDIR%\Sample *.efkproj *.efkmodel *.txt *.png *.mqo *.fbx /S

echo License
cp Release/LICENSE.txt %RDIR_R%/LICENSE.txt

echo Readme
copy readme_tool_win.txt %RDIR%\readme.txt
copy readme_runtime.txt %RDIR_R%\readme.txt

echo Help
mkdir %RDIR%\Help

mkdir %RDIR_R%\Help
robocopy docs\Help_Runtime %RDIR_R%\Help *.html *.css *.efkproj *.png /S

echo doxygen(exe or bat)
call doxygen.bat doxygen.template
call doxygen.exe doxygen.template

mkdir %RDIR_R%\Help\html\doxygen
robocopy html %RDIR_R%\Help\html\doxygen /S

echo Readme2
copy docs\readme_sample.txt %RDIR%\Sample\readme.txt

echo VS

mkdir %RDIR_R%\Compiled\include\
mkdir %RDIR_R%\Compiled\lib\

mkdir %RDIR_R%\Compiled\lib\VS2017\
mkdir %RDIR_R%\Compiled\lib\VS2017WIN64\

robocopy VS15\Dev\Cpp\Effekseer\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\Effekseer\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\EffekseerRendererDX9\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\EffekseerRendererDX9\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\EffekseerRendererDX11\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\EffekseerRendererDX11\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

copy VS15\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017\Debug\.
copy VS15\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017\Release\.

robocopy VS15WIN64\Dev\Cpp\Effekseer\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\Effekseer\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX9\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX9\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX11\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX11\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

copy VS15WIN64\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017WIN64\Debug\.
copy VS15WIN64\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017WIN64\Release\.

mkdir %RDIR_R%\Compiled\lib\VS2019\
mkdir %RDIR_R%\Compiled\lib\VS2019WIN64\

robocopy VS16\Dev\Cpp\Effekseer\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\Effekseer\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16\Dev\Cpp\EffekseerRendererDX9\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\EffekseerRendererDX9\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16\Dev\Cpp\EffekseerRendererDX11\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\EffekseerRendererDX11\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

copy VS16\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019\Debug\.
copy VS16\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019\Release\.

robocopy VS16WIN64\Dev\Cpp\Effekseer\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\Effekseer\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX9\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX9\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX11\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX11\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

copy VS16WIN64\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019WIN64\Debug\.
copy VS16WIN64\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019WIN64\Release\.


copy Dev\Cpp\Effekseer\Effekseer.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerRendererDX12\EffekseerRendererDX12.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR_R%\Compiled\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR_R%\Compiled\include\.

copy Release\CMakeLists.txt %RDIR_R%\.
robocopy Downloads\glfw %RDIR_R%\Examples\Utils\glfw\. /S
copy Release\build_msvc2017.bat %RDIR_R%\build_msvc2017.bat
copy Release\build_msvc2019.bat %RDIR_R%\build_msvc2019.bat
copy Release\build_macOS.sh %RDIR_R%\build_macOS.sh

pause