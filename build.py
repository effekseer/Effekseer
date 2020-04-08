import subprocess
import Script.aceutils as aceutils
import sys
import os

def call( cmd , env=None):
    """ call command line.
    """

    print( cmd )
    p = subprocess.Popen(cmd, shell=True, env=env)
    ret = p.wait()
    if ret != 0:
        print("Failed {}".format(cmd))
        raise Exception

env = os.environ.copy()

env = os.environ.copy()
env["PKG_CONFIG_PATH"] = os.getenv('PKG_CONFIG_PATH', '/Library/Frameworks/Mono.framework/Versions/Current/lib/pkgconfig')
env["AS"] = os.getenv('AS', 'as -arch i386') 
env["CC"] = os.getenv('CC', 'clang -arch i386 -mmacosx-version-min=10.6')

env["MONO_SDK_PATH"] = os.getenv('MONO_SDK_PATH', '/Library/Frameworks/Mono.framework/Versions/Current')
env["PACKAGEING_FOR_MAC"] = os.getenv('PACKAGEING_FOR_MAC', '0')
env["IGNORE_BUILD"] = os.getenv('IGNORE_BUILD', '0')


is_x86 = 'x86' in sys.argv

if env['IGNORE_BUILD'] == '0':
    aceutils.mkdir('build')
    if aceutils.isWin():
        #import winreg
        #reg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE)
        #key = winreg.OpenKey(reg, r"SOFTWARE\Microsoft\MSBuild\ToolsVersions\12.0")
        #msbuild_path = winreg.QueryValueEx(key, 'MSBuildToolsPath')[0] + 'MSBuild.exe'

        candidates = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\MSBuild.exe",
        r"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe",
        ]

        candidate = None
        for candidate in candidates:
            if os.path.exists(candidate):
                msbuild_path = candidate
                break

        if candidate is None:
            raise Exception("MSBuild is not found")

    else:
        msbuild_path = 'msbuild'

    with aceutils.CurrentDir('build'):
        # for auto restore of .csproj 
        aceutils.wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

        if aceutils.isWin():
            if is_x86:
                aceutils.call('cmake .. -A Win32 -DBUILD_VIEWER=ON')
            else:
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

    if is_x86:
        call('"' + msbuild_path + '"' + ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86')
        call('"' + msbuild_path + '"' + ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86')
    else:
        call('"' + msbuild_path + '"' + ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64')
        call('"' + msbuild_path + '"' + ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64')

if env['PACKAGEING_FOR_MAC'] == '1':

    if aceutils.isMac():
        aceutils.cd('Dev')
        aceutils.call('cd release;mkbundle -o Effekseer Effekseer.exe --deps --sdk $MONO_SDK_PATH;otool -L Effekseer', env=env)
        aceutils.mkdir('Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/Effekseer', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/Effekseer.exe', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/libViewer.dylib', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/EffekseerCore.dll', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/EffekseerMaterialEditor', 'Mac/Effekseer.app/Contents/Resources/')
        
        aceutils.copytree('release/resources', 'Mac/Effekseer.app/Contents/Resources/resources')
        aceutils.copytree('release/scripts', 'Mac/Effekseer.app/Contents/Resources/scripts')
        aceutils.copytree('release/tools', 'Mac/Effekseer.app/Contents/Resources/tools')
        
        aceutils.call('chmod +x Mac/Effekseer.app/Contents/MacOS/script.sh')