SET RDIR=Effekseer0501

rmdir %RDIR%
mkdir %RDIR%

mkdir %RDIR%\RuntimeSample\lib
mkdir %RDIR%\RuntimeSample\include
mkdir %RDIR%\Runtime\

echo アプリケーションコピー
mkdir %RDIR%\Bin
copy Dev\release\Effekseer.exe %RDIR%\Bin\.
copy Dev\release\Effekseer.exe.config %RDIR%\Bin\.
copy Dev\release\EffekseerCore.dll %RDIR%\Bin\.
copy Dev\release\EffekseerInterface.dll %RDIR%\Bin\.
copy Dev\release\EffekseerViewer.dll %RDIR%\Bin\.
copy Dev\release\IronPython.dll %RDIR%\Bin\.
copy Dev\release\IronPython.Modules.dll %RDIR%\Bin\.
copy Dev\release\Microsoft.Dynamic.dll %RDIR%\Bin\.
copy Dev\release\Microsoft.Scripting.dll %RDIR%\Bin\.
copy Dev\release\Viewer.dll %RDIR%\Bin\.
copy Dev\release\WeifenLuo.WinFormsUI.Docking.dll %RDIR%\Bin\.

mkdir %RDIR%\Bin\scripts
mkdir %RDIR%\Bin\scripts\command
mkdir %RDIR%\Bin\scripts\export
mkdir %RDIR%\Bin\scripts\import
mkdir %RDIR%\Bin\scripts\selected
copy Dev\release\scripts\export\Default.cs %RDIR%\Bin\scripts\export\.

echo ランタイムコピー
mkdir %RDIR%\Runtime
mkdir %RDIR%\Runtime\include
mkdir %RDIR%\Runtime\lib

copy Dev\Cpp\Effekseer\Effekseer.h %RDIR%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX11.h %RDIR%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR%\Runtime\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR%\Runtime\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR%\Runtime\include\.

copy Dev\Cpp\EffekseerRendererDX9.sln %RDIR%\Runtime\.
copy Dev\Cpp\EffekseerRendererDX11.sln %RDIR%\Runtime\.
copy Dev\Cpp\EffekseerRendererGL.sln %RDIR%\Runtime\.

mkdir %RDIR%\Runtime\Effekseer
robocopy Dev\Cpp\Effekseer %RDIR%\Runtime\Effekseer *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerRendererDX9
robocopy Dev\Cpp\EffekseerRendererDX9 %RDIR%\Runtime\EffekseerRendererDX9 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerRendererDX11
robocopy Dev\Cpp\EffekseerRendererDX11 %RDIR%\Runtime\EffekseerRendererDX11 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerRendererGL
robocopy Dev\Cpp\EffekseerRendererGL %RDIR%\Runtime\EffekseerRendererGL *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerRendererCommon
robocopy Dev\Cpp\EffekseerRendererCommon %RDIR%\Runtime\EffekseerRendererCommon *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerSoundXAudio2
robocopy Dev\Cpp\EffekseerSoundXAudio2 %RDIR%\Runtime\EffekseerSoundXAudio2 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR%\Runtime\EffekseerSoundAL
robocopy Dev\Cpp\EffekseerSoundAL %RDIR%\Runtime\EffekseerSoundAL *.h *.cpp *.filters *.vcxproj *.fx /mir /S


echo ランタイム組み込みサンプル
robocopy Release\RuntimeSample %RDIR%\RuntimeSample\ *.h *.cpp *.filters *.vcxproj *.fx *.efk *.wav *.png *.sln /mir /S
rmdir %RDIR%\RuntimeSample\include /S /Q
rmdir %RDIR%\RuntimeSample\lib /S /Q

mkdir %RDIR%\RuntimeSample\include
mkdir %RDIR%\RuntimeSample\lib

copy Dev\Cpp\Effekseer\Effekseer.h %RDIR%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h %RDIR%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR%\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR%\RuntimeSample\include\.

copy Dev\Cpp\lib\Effekseer.Debug.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Debug.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Debug.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Debug.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Debug.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Debug.lib %RDIR%\RuntimeSample\lib\.

copy Dev\Cpp\lib\Effekseer.Release.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Release.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Release.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Release.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Release.lib %RDIR%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Release.lib %RDIR%\RuntimeSample\lib\.

cp CMakeLists.txt %RDIR%/Runtime/CMakeLists.txt

echo サンプル

mkdir %RDIR%\Sample
robocopy Release\Sample %RDIR%\Sample *.efkproj *.efkmodel *.txt *.png /mir /S

cp Release/MIT-LICENSE.txt %RDIR%/MIT-LICENSE.txt

mkdir %RDIR%\Help
robocopy Release\Help %RDIR%\Help *.html *.css *.efkproj *.png /mir /S

mkdir %RDIR%\Tool
robocopy Release\Tool %RDIR%\Tool *.exe /mir /S

mkdir %RDIR%\GameEngine
robocopy Release\GameEngine %RDIR%\GameEngine *.unitypackage *.zip /mir /S

pause

