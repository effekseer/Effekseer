import subprocess
import Script.aceutils as aceutils

aceutils.mkdir('build')

with aceutils.CurrentDir('build'):
    # for auto restore of .csproj 
    aceutils.wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

    if aceutils.isWin():
        aceutils.call('cmake .. -G "Visual Studio 15 2017" -DBUILD_VIEWER=ON')
    elif aceutils.isMac():
        aceutils.call('cmake .. -G "Xcode" -DBUILD_VIEWER=ON')
    aceutils.call('cmake --build . --config Release')


if aceutils.isWin():
    aceutils.call('build\\nuget.exe restore Dev/Editor/Effekseer.sln')
else:
    aceutils.call('mono ./build/nuget.exe restore Dev/Editor/Effekseer.sln')

aceutils.call('msbuild Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86')
aceutils.call('msbuild Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86')

