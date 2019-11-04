SET RDIR=Effekseer150b1
SET RDIR_R=EffekseerRuntime150b1

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
mkdir %RDIR_R%\RuntimeSample\
mkdir %RDIR_R%\RuntimeSample\lib
mkdir %RDIR_R%\RuntimeSample\include

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
robocopy Dev\Cpp\Effekseer %RDIR_R%\src\Effekseer *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerRendererDX9
robocopy Dev\Cpp\EffekseerRendererDX9 %RDIR_R%\src\EffekseerRendererDX9 *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerRendererDX11
robocopy Dev\Cpp\EffekseerRendererDX11 %RDIR_R%\src\EffekseerRendererDX11 *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerRendererGL
robocopy Dev\Cpp\EffekseerRendererGL %RDIR_R%\src\EffekseerRendererGL *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerRendererCommon
robocopy Dev\Cpp\EffekseerRendererCommon %RDIR_R%\src\EffekseerRendererCommon *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerSoundXAudio2
robocopy Dev\Cpp\EffekseerSoundXAudio2 %RDIR_R%\src\EffekseerSoundXAudio2 *.h *.cpp *.filters *.vcxproj *.fx /S

mkdir %RDIR_R%\src\EffekseerSoundAL
robocopy Dev\Cpp\EffekseerSoundAL %RDIR_R%\src\EffekseerSoundAL *.h *.cpp *.filters *.vcxproj *.fx /S


echo Samples for runtimes
robocopy Release\RuntimeSample %RDIR_R%\RuntimeSample\ *.h *.cpp *.filters *.vcxproj *.fx *.efk *.wav *.png *.sln /S
rmdir %RDIR_R%\RuntimeSample\include /S /Q
rmdir %RDIR_R%\RuntimeSample\lib /S /Q

mkdir %RDIR_R%\RuntimeSample\include
mkdir %RDIR_R%\RuntimeSample\lib

copy Dev\Cpp\Effekseer\Effekseer.h %RDIR_R%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR_R%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h %RDIR_R%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR_R%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR_R%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR_R%\RuntimeSample\include\.

copy Dev\Cpp\lib\x86\Effekseer.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererDX9.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererDX11.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererGL.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerSoundXAudio2.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerSoundAL.Debug.lib %RDIR_R%\RuntimeSample\lib\.

copy Dev\Cpp\lib\x86\Effekseer.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererDX9.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererDX11.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerRendererGL.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerSoundXAudio2.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\x86\EffekseerSoundAL.Release.lib %RDIR_R%\RuntimeSample\lib\.

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

mkdir %RDIR_R%\Compiled\

echo Compile VS15
rmdir /S /Q VS15
mkdir VS15

cd VS15
call cmake.bat -G "Visual Studio 15"  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../
cmake.exe -G "Visual Studio 15"  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe" Effekseer.sln /p:configuration=Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe" Effekseer.sln /p:configuration=Release
cd ..

echo Compile VS15WIN64
rmdir /S /Q VS15WIN64
mkdir VS15WIN64

cd VS15WIN64
call cmake.bat -G "Visual Studio 15 Win64"  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../
cmake.exe -G "Visual Studio 15 Win64"  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe" Effekseer.sln /p:configuration=Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe" Effekseer.sln /p:configuration=Release
cd ..


echo Compile VS16
rmdir /S /Q VS16
mkdir VS16

cd VS16
call cmake.bat -G "Visual Studio 16" -A Win32 -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../
cmake.exe -G "Visual Studio 16" -A Win32 -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" Effekseer.sln /p:configuration=Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" Effekseer.sln /p:configuration=Release
cd ..

echo Compile VS16WIN64
rmdir /S /Q VS16WIN64
mkdir VS16WIN64

cd VS16WIN64
call cmake.bat -G "Visual Studio 16" -A x64  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../
cmake.exe -G "Visual Studio 16" -A x64  -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_XAUDIO2=ON ../

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" Effekseer.sln /p:configuration=Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" Effekseer.sln /p:configuration=Release
cd ..


mkdir %RDIR_R%\Compiled\include\
mkdir %RDIR_R%\Compiled\lib\

mkdir %RDIR_R%\Compiled\lib\VS2017\
mkdir %RDIR_R%\Compiled\lib\VS2017WIN64\

robocopy VS15\Dev\Cpp\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

robocopy VS15\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2017\Debug *.lib /S
robocopy VS15\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2017\Release *.lib /S

copy VS15\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017\Debug\.
copy VS15\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017\Release\.

robocopy VS15WIN64\Dev\Cpp\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2017WIN64\Debug *.lib /S
robocopy VS15WIN64\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2017WIN64\Release *.lib /S

copy VS15WIN64\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017WIN64\Debug\.
copy VS15WIN64\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2017WIN64\Release\.

mkdir %RDIR_R%\Compiled\lib\VS2019\
mkdir %RDIR_R%\Compiled\lib\VS2019WIN64\

robocopy VS16\Dev\Cpp\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16\Dev\Cpp\3rdParty\LLGI\src\Debug %RDIR_R%\Compiled\lib\VS2019\Debug *.lib /S
robocopy VS16\Dev\Cpp\3rdParty\LLGI\src\Release %RDIR_R%\Compiled\lib\VS2019\Release *.lib /S

robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\EffekseerRendererDX12\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

copy VS16\Dev\Cpp\EffekseerSoundXAudio2\Debug\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019\Debug\.
copy VS16\Dev\Cpp\EffekseerSoundXAudio2\Release\EffekseerSoundXAudio2.lib %RDIR_R%\Compiled\lib\VS2019\Release\.

robocopy VS16WIN64\Dev\Cpp\Debug %RDIR_R%\Compiled\lib\VS2019WIN64\Debug *.lib /S
robocopy VS16WIN64\Dev\Cpp\Release %RDIR_R%\Compiled\lib\VS2019WIN64\Release *.lib /S

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

pause

