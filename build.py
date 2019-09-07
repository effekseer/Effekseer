import subprocess
import Script.aceutils as aceutils

aceutils.mkdir('build')

if aceutils.isWin():
    import winreg
    reg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE)
    key = winreg.OpenKey(reg, r"SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0")
    msbuild_path = winreg.QueryValueEx(key, 'MSBuildToolsPath')[0] + 'MSBuild.exe'
else:
    msbuild_path = 'msbuild'

with aceutils.CurrentDir('build'):
    # for auto restore of .csproj 
    aceutils.wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

    if aceutils.isWin():
        aceutils.call('cmake .. -A x64 -DBUILD_VIEWER=ON')
    elif aceutils.isMac():
        aceutils.call('cmake .. -G "Xcode" -DBUILD_VIEWER=ON')
    else:
        aceutils.call('cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON')
    aceutils.call('cmake --build . --config Release')

if aceutils.isWin():
    aceutils.call('build\\nuget.exe restore Dev/Editor/Effekseer.sln')
else:
    aceutils.call('mono ./build/nuget.exe restore Dev/Editor/Effekseer.sln')

aceutils.call('"' + msbuild_path + '"' + ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64')
aceutils.call('"' + msbuild_path + '"' + ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64')

