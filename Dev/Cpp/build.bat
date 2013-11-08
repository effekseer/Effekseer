set PATH=C:\Windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

msbuild Effekseer\Effekseer.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Effekseer\Effekseer.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerRendererDX9\EffekseerRendererDX9.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerRendererDX9\EffekseerRendererDX9.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerRendererDX11\EffekseerRendererDX11.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerRendererDX11\EffekseerRendererDX11.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerRendererGL\EffekseerRendererGL.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerRendererGL\EffekseerRendererGL.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerSoundAL\EffekseerSoundAL.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerSoundAL\EffekseerSoundAL.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerSoundDSound\EffekseerSoundDSound.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerSoundDSound\EffekseerSoundDSound.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild EffekseerSoundXAudio2\EffekseerSoundXAudio2.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild EffekseerSoundXAudio2\EffekseerSoundXAudio2.vcxproj /p:Configuration=Release /t:Clean,Build
