SET RDIR=Effekseer0620
SET RDIR_R=EffekseerRuntime0620
SET RDIR_U=EffekseerForUnity0620

rmdir %RDIR%
mkdir %RDIR%

rmdir %RDIR_R%
mkdir %RDIR_R%

rmdir %RDIR_U%
mkdir %RDIR_U%


echo アプリケーションコピー
mkdir %RDIR%\Tool
copy Dev\release\Effekseer.exe %RDIR%\Tool\.
copy Dev\release\Effekseer.exe.config %RDIR%\Tool\.
copy Dev\release\EffekseerCore.dll %RDIR%\Tool\.
copy Dev\release\EffekseerInterface.dll %RDIR%\Tool\.
copy Dev\release\EffekseerViewer.dll %RDIR%\Tool\.
copy Dev\release\IronPython.dll %RDIR%\Tool\.
copy Dev\release\IronPython.Modules.dll %RDIR%\Tool\.
copy Dev\release\Microsoft.Dynamic.dll %RDIR%\Tool\.
copy Dev\release\Microsoft.Scripting.dll %RDIR%\Tool\.
copy Dev\release\Viewer.dll %RDIR%\Tool\.
copy Dev\release\WeifenLuo.WinFormsUI.Docking.dll %RDIR%\Tool\.
copy Dev\release\System.Reactive.Core.dll %RDIR%\Tool\.
copy Dev\release\System.Reactive.Interfaces.dll %RDIR%\Tool\.
copy Dev\release\System.Reactive.Linq.dll %RDIR%\Tool\.

mkdir %RDIR%\Tool\scripts
mkdir %RDIR%\Tool\scripts\export
copy Dev\release\scripts\export\Default.cs %RDIR%\Bin\scripts\export\.

echo ランタイムコピー
mkdir %RDIR_R%\RuntimeSample\
mkdir %RDIR_R%\RuntimeSample\lib
mkdir %RDIR_R%\RuntimeSample\include

mkdir %RDIR_R%\Runtime
mkdir %RDIR_R%\Runtime\include
mkdir %RDIR_R%\Runtime\lib

copy Dev\Cpp\Effekseer\Effekseer.h %RDIR_R%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h %RDIR_R%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX11.h %RDIR_R%\Runtime\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h %RDIR_R%\Runtime\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h %RDIR_R%\Runtime\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h %RDIR_R%\Runtime\include\.

copy Dev\Cpp\EffekseerRendererDX9.sln %RDIR_R%\Runtime\.
copy Dev\Cpp\EffekseerRendererDX11.sln %RDIR_R%\Runtime\.
copy Dev\Cpp\EffekseerRendererGL.sln %RDIR_R%\Runtime\.

mkdir %RDIR_R%\Runtime\Effekseer
robocopy Dev\Cpp\Effekseer %RDIR_R%\Runtime\Effekseer *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerRendererDX9
robocopy Dev\Cpp\EffekseerRendererDX9 %RDIR_R%\Runtime\EffekseerRendererDX9 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerRendererDX11
robocopy Dev\Cpp\EffekseerRendererDX11 %RDIR_R%\Runtime\EffekseerRendererDX11 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerRendererGL
robocopy Dev\Cpp\EffekseerRendererGL %RDIR_R%\Runtime\EffekseerRendererGL *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerRendererCommon
robocopy Dev\Cpp\EffekseerRendererCommon %RDIR_R%\Runtime\EffekseerRendererCommon *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerSoundXAudio2
robocopy Dev\Cpp\EffekseerSoundXAudio2 %RDIR_R%\Runtime\EffekseerSoundXAudio2 *.h *.cpp *.filters *.vcxproj *.fx /mir /S

mkdir %RDIR_R%\Runtime\EffekseerSoundAL
robocopy Dev\Cpp\EffekseerSoundAL %RDIR_R%\Runtime\EffekseerSoundAL *.h *.cpp *.filters *.vcxproj *.fx /mir /S


echo ランタイム組み込みサンプル
robocopy Release\RuntimeSample %RDIR_R%\RuntimeSample\ *.h *.cpp *.filters *.vcxproj *.fx *.efk *.wav *.png *.sln /mir /S
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

copy Dev\Cpp\lib\Effekseer.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Debug.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Debug.lib %RDIR_R%\RuntimeSample\lib\.

copy Dev\Cpp\lib\Effekseer.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Release.lib %RDIR_R%\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Release.lib %RDIR_R%\RuntimeSample\lib\.

copy Dev\Cpp\CMakeLists.txt %RDIR_R%\Runtime\.

echo サンプル

mkdir %RDIR%\Sample
robocopy Release\Sample %RDIR%\Sample *.efkproj *.efkmodel *.txt *.png /mir /S

echo ライセンス
cp Release/MIT-LICENSE.txt %RDIR_R%/MIT-LICENSE.txt
cp Release/MIT-LICENSE.txt %RDIR_U%/MIT-LICENSE.txt

echo Readme
copy readme_tool.txt %RDIR%\readme.txt
copy readme_runtime.txt %RDIR_R%\readme.txt
copy readme_unity.txt %RDIR_U%\readme.txt

echo ヘルプ
mkdir %RDIR%\Help
robocopy Release\Help_Tool %RDIR%\Help *.html *.css *.efkproj *.png /mir /S

mkdir %RDIR_R%\Help
robocopy Release\Help_Runtime %RDIR_R%\Help *.html *.css *.efkproj *.png /mir /S

mkdir %RDIR_U%\Help
robocopy Release\Help_Unity %RDIR_U%\Help *.html *.css *.efkproj *.png /mir /S

echo ツール
mkdir %RDIR%\Tool
robocopy Release\Tool %RDIR%\Tool *.exe /mir /S

echo Unity
mkdir %RDIR_U%\GameEngine
robocopy Release\GameEngine %RDIR_U%\GameEngine *.unitypackage *.zip /mir /S

echo doxygen
doxygen doxygen.template
mkdir %RDIR_R%\Help\html\doxygen
robocopy html %RDIR_R%\Help\html\doxygen /mir /S

pause

