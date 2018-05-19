set PATH=C:\Windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

msbuild Cpp\Effekseer\Effekseer.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\Effekseer\Effekseer.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerRendererDX9\EffekseerRendererDX9.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerRendererDX9\EffekseerRendererDX9.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerRendererDX11\EffekseerRendererDX11.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerRendererDX11\EffekseerRendererDX11.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerRendererGL\EffekseerRendererGL.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerRendererGL\EffekseerRendererGL.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerSoundAL\EffekseerSoundAL.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerSoundAL\EffekseerSoundAL.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerSoundDSound\EffekseerSoundDSound.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerSoundDSound\EffekseerSoundDSound.vcxproj /p:Configuration=Release /t:Clean,Build

msbuild Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.vcxproj /p:Configuration=Debug /t:Clean,Build
msbuild Cpp\EffekseerSoundXAudio2\EffekseerSoundXAudio2.vcxproj /p:Configuration=Release /t:Clean,Build
