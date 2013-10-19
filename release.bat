rmdir Release\RuntimeSample\lib /q /s
mkdir Release\RuntimeSample\lib

rmdir Release\RuntimeSample\include /q /s
mkdir Release\RuntimeSample\include

rmdir Release\Runtime\ /q /s
mkdir Release\Runtime\

mkdir Release Release/

echo アプリケーションコピー
mkdir Release\Bin
copy Dev\release\Effekseer.exe Release\Bin\.
copy Dev\release\Effekseer.exe.config Release\Bin\.
copy Dev\release\EffekseerCore.dll Release\Bin\.
copy Dev\release\EffekseerInterface.dll Release\Bin\.
copy Dev\release\EffekseerViewer.dll Release\Bin\.
copy Dev\release\IronPython.dll Release\Bin\.
copy Dev\release\IronPython.Modules.dll Release\Bin\.
copy Dev\release\Microsoft.Dynamic.dll Release\Bin\.
copy Dev\release\Microsoft.Scripting.dll Release\Bin\.
copy Dev\release\Viewer.dll Release\Bin\.
copy Dev\release\WeifenLuo.WinFormsUI.Docking.dll Release\Bin\.

mkdir Release\Bin\scripts
mkdir Release\Bin\scripts\command
mkdir Release\Bin\scripts\export
mkdir Release\Bin\scripts\import
mkdir Release\Bin\scripts\selected
copy Dev\release\scripts\export\Default.cs Release\Bin\scripts\export\.

echo ランタイムコピー
mkdir Release\Runtime
mkdir Release\Runtime\include
mkdir Release\Runtime\lib

copy Dev\Cpp\Effekseer\Effekseer.h Release\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h Release\Runtime\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX11.h Release\Runtime\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h Release\Runtime\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h Release\Runtime\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h Release\Runtime\include\.

copy Dev\Cpp\EffekseerRendererDX9.sln Release\Runtime\.
copy Dev\Cpp\EffekseerRendererDX11.sln Release\Runtime\.
copy Dev\Cpp\EffekseerRendererGL.sln Release\Runtime\.

TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\Effekseer" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerRendererDX9" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerRendererDX11" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerRendererGL" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerRendererCommon" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerSoundXAudio2" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1
TortoiseProc.exe /Command:dropexport /path:"%~dp0Dev\Cpp\EffekseerSoundAL" /droptarget:"%~dp0Release\Runtime\" /notempfile /closeonend:1

echo ランタイム組み込みサンプル
copy Dev\Cpp\Effekseer\Effekseer.h Release\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX9\EffekseerRendererDX9.h Release\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererDX11\EffekseerRendererDX11.h Release\RuntimeSample\include\.
copy Dev\Cpp\EffekseerRendererGL\EffekseerRendererGL.h Release\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.h Release\RuntimeSample\include\.
copy Dev\Cpp\EffekseerSoundAL\EffekseerSoundAL.h Release\RuntimeSample\include\.

copy Dev\Cpp\lib\Effekseer.Debug.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Debug.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Debug.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Debug.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Debug.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Debug.lib Release\RuntimeSample\lib\.

copy Dev\Cpp\lib\Effekseer.Release.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX9.Release.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererDX11.Release.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerRendererGL.Release.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundXAudio2.Release.lib Release\RuntimeSample\lib\.
copy Dev\Cpp\lib\EffekseerSoundAL.Release.lib Release\RuntimeSample\lib\.

cp CMakeLists.txt Release/Runtime/CMakeLists.txt

pause

