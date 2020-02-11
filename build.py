import subprocess
import Script.aceutils as aceutils

import os

def call( cmd , env=None):
	""" call command line.
	"""

	print( cmd )
	p = subprocess.Popen(cmd, shell=True, env=env)
	ret = p.wait()
	if ret != 0:
		raise Exception

env = os.environ.copy()

env = os.environ.copy()
env["PKG_CONFIG_PATH"] = os.getenv('PKG_CONFIG_PATH', '/Library/Frameworks/Mono.framework/Versions/Current/lib/pkgconfig')
env["AS"] = os.getenv('AS', 'as -arch i386') 
env["CC"] = os.getenv('CC', 'clang -arch i386 -mmacosx-version-min=10.6')

env["MONO_SDK_PATH"] = os.getenv('MONO_SDK_PATH', '/Library/Frameworks/Mono.framework/Versions/Current')
env["PACKAGEING_FOR_MAC"] = os.getenv('PACKAGEING_FOR_MAC', '0')
env["IGNORE_BUILD"] = os.getenv('IGNORE_BUILD', '0')




if env['IGNORE_BUILD'] == '0':
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

    call('"' + msbuild_path + '"' + ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64')
    call('"' + msbuild_path + '"' + ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64')

if env['PACKAGEING_FOR_MAC'] == '1':

    if aceutils.isMac():
        aceutils.cd('Dev')

        aceutils.mkdir('Mac/Effekseer.app/Contents/Resources/')
        aceutils.call('cd release;mkbundle -o Effekseer Effekseer.exe --deps --sdk $MONO_SDK_PATH;otool -L Effekseer', env=env)
        aceutils.call('cd release;mkbundle -o tools/mqoToEffekseerModelConverter tools/mqoToEffekseerModelConverter.exe --deps --sdk $MONO_SDK_PATH;otool -L tools/mqoToEffekseerModelConverter;', env=env)

        aceutils.copy('release/Effekseer', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/Effekseer.exe', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/libViewer.dylib', 'Mac/Effekseer.app/Contents/Resources/')
        aceutils.copy('release/EffekseerCore.dll', 'Mac/Effekseer.app/Contents/Resources/')
        
        aceutils.copytree('release/resources', 'Mac/Effekseer.app/Contents/Resources/resources')
        aceutils.copytree('release/scripts', 'Mac/Effekseer.app/Contents/Resources/scripts')
        
        aceutils.mkdir('Mac/Effekseer.app/Contents/Resources/tools')

        aceutils.copy('release/tools/mqoToEffekseerModelConverter', 'Mac/Effekseer.app/Contents/Resources/tools')
        aceutils.rmdir('fbxToEffekseerModelConverterMac')
        aceutils.rmdir('__MACOSX')
        aceutils.wget('https://github.com/effekseer/Effekseer/releases/download/Prebuild/fbxToEffekseerModelConverterMac.zip')
        aceutils.unzip('fbxToEffekseerModelConverterMac.zip')

        aceutils.copy('fbxToEffekseerModelConverterMac/fbxToEffekseerModelConverter', 'Mac/Effekseer.app/Contents/Resources/tools/')
        aceutils.copy('fbxToEffekseerModelConverterMac/libfbxsdk.dylib', 'Mac/Effekseer.app/Contents/Resources/tools/')
        aceutils.call('chmod +x Mac/Effekseer.app/Contents/MacOS/script.sh')