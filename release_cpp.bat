@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
SET RDIR_R=EffekseerForCpp

if exist "%RDIR_R%" rmdir /s /q "%RDIR_R%"
mkdir "%RDIR_R%"
mkdir "%RDIR_R%\cmake"

echo Copy runtime
mkdir "%RDIR_R%\Examples"
mkdir "%RDIR_R%\src"
mkdir "%RDIR_R%\src\include"
mkdir "%RDIR_R%\src\lib"

for %%F in (
    "Dev\Cpp\Effekseer\Effekseer.h"
    "Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h"
    "Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h"
    "Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h"
    "Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h"
    "Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h"
) do copy %%F "%RDIR_R%\src\include\"


call :copy_runtime Effekseer *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerRendererDX9 *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerRendererDX11 *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerRendererDX12 *.h *.cpp *.fx *.bat CMakeLists.txt
call :copy_runtime EffekseerRendererGL *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerRendererLLGI *.h *.cpp CMakeLists.txt
call :copy_runtime EffekseerRendererCommon *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerRendererMetal *.h *.cpp *.mm CMakeLists.txt
call :copy_runtime EffekseerRendererVulkan *.h *.cpp *.vert *.frag *.inl *.py CMakeLists.txt
call :copy_runtime EffekseerSoundXAudio2 *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerSoundAL *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerSoundDSound *.h *.cpp *.fx CMakeLists.txt
call :copy_runtime EffekseerMaterialCompiler *.h *.cpp CMakeLists.txt

mkdir %RDIR_R%\src\3rdParty\LLGI
robocopy Dev\Cpp\3rdParty\LLGI %RDIR_R%\src\3rdParty\LLGI /S

mkdir %RDIR_R%\src\3rdParty\glslang
robocopy Dev\Cpp\3rdParty\glslang %RDIR_R%\src\3rdParty\glslang /S

echo for github actions
rmdir /s /q %RDIR_R%\src\3rdParty\glslang\Test


mkdir %RDIR_R%\src\3rdParty\stb_effekseer
robocopy Dev\Cpp\3rdParty\stb_effekseer %RDIR_R%\src\3rdParty\stb_effekseer *.h

echo Samples for runtimes
robocopy Examples %RDIR_R%\Examples\ *.h *.cpp *.mm *.txt *.fx *.efk *.wav *.png /S

copy Dev\Cpp\CMakeLists.txt %RDIR_R%\src\.

copy cmake\FilterFolder.cmake %RDIR_R%\cmake\.

echo Sample

mkdir %RDIR_R%\Sample
robocopy Release\Sample %RDIR_R%\Sample *.efkproj *.efkefc *.efkmodel *.txt *.png *.mqo *.fbx /S

echo License
copy LICENSE %RDIR_R%\LICENSE.txt
copy LICENSE_RUNTIME_DIRECTX %RDIR_R%\LICENSE_RUNTIME_DIRECTX.txt
copy LICENSE_RUNTIME_VULKAN %RDIR_R%\LICENSE_RUNTIME_VULKAN.txt

echo Readme
copy docs\Help_Cpp_Ja.html %RDIR_R%\Help_Cpp_Ja.html
copy docs\Help_Cpp_En.html %RDIR_R%\Help_Cpp_En.html

copy Release\CMakeLists.txt %RDIR_R%\.
robocopy Downloads\glfw %RDIR_R%\Examples\Utils\glfw\. /S
copy Release\build_msvc.bat %RDIR_R%\build_msvc.bat

copy Release\build_macOS.sh %RDIR_R%\build_macOS.sh

goto :eof

:copy_runtime
set DIR=%1
shift
if not exist "%RDIR_R%\src\%DIR%" mkdir "%RDIR_R%\src\%DIR%"
robocopy "Dev\Cpp\%DIR%" "%RDIR_R%\src\%DIR%" %* /S
goto :eof

:eof
pause