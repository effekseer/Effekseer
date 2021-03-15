import subprocess
import sys
import os
import shutil
import urllib.request
import platform
from distutils.spawn import find_executable
import distutils
from distutils import dir_util


def call(cmd, env=None):
    """ call command line.
    """

    print(cmd)
    p = subprocess.Popen(cmd, shell=True, env=env)
    ret = p.wait()
    if ret != 0:
        print("Failed {}".format(cmd))
        raise Exception


def get_files(path):
    """ get files.
    """

    def getlistdir(path, l):
        dirs = os.listdir(path)
        for d in dirs:
            newpath = os.path.join(path, d)
            if os.path.isdir(newpath):
                getlistdir(newpath, l)
            else:
                l.append(newpath)

    ret = []
    getlistdir(path, ret)
    return ret


def copytreeWithExt(src, dst, exts):
    files = get_files(src)
    for _from in files:

        root, ext = os.path.splitext(_from)
        if not ext in exts:
            continue
        _to = dst + _from[len(src):]
        print(_from + '\t:\t' + _to)

        if not os.path.exists(os.path.dirname(_to)):
            os.makedirs(os.path.dirname(_to))

        shutil.copy(_from, _to)


def isWin():
    return platform.system() == 'Windows'


def isMac():
    return platform.system() == 'Darwin'

def wget(address):
    urllib.request.urlretrieve(address, os.path.basename(address))

def rm(path):
    if os.path.exists(path):
        os.remove(path)

def rmdir(path):
    if os.path.exists(path):
        print("rmdir : " + path)
        shutil.rmtree(path)
    else:
        print("rmdir : not found " + path)


def cd(path):
    os.chdir(path)


def cdToScript():
    cd(os.path.dirname(os.path.abspath(__file__)))


def mkdir(path):
    if not os.path.exists(path):
        os.mkdir(path)

def copy(src, dst):
    print("copying from {0} to {1}".format(src, dst))
    shutil.copy(src, dst)


def copytree(src, dst, change=False, ignoreList=None):
    print("copying tree from {0} to {1}".format(src, dst))
    if change and os.path.exists(dst):
        rmdir(dst)

    if not os.path.exists(dst):
        shutil.copytree(src, dst, ignore=ignoreList)


class CurrentDir:
    def __init__(self, path):
        self.prev = os.getcwd()
        self.path = path

    def __enter__(self):
        cd(self.path)
        #print("cd: " + os.getcwd())
        return self

    def __exit__(self, type, value, traceback):
        cd(self.prev)
        #print("cd: " + os.getcwd())


env = os.environ.copy()

env = os.environ.copy()
env["PKG_CONFIG_PATH"] = os.getenv(
    'PKG_CONFIG_PATH', '/Library/Frameworks/Mono.framework/Versions/Current/lib/pkgconfig')
env["AS"] = os.getenv('AS', 'as -arch i386')
env["CC"] = os.getenv('CC', 'clang -arch i386 -mmacosx-version-min=10.6')

env["MONO_SDK_PATH"] = os.getenv(
    'MONO_SDK_PATH', '/Library/Frameworks/Mono.framework/Versions/Current')
env["PACKAGEING_FOR_MAC"] = os.getenv('PACKAGEING_FOR_MAC', '0')
env["PACKAGEING_FOR_LINUX"] = os.getenv('PACKAGEING_FOR_LINUX', '0')
env['X86'] = os.getenv('X86', '0')
env["IGNORE_BUILD"] = os.getenv('IGNORE_BUILD', '0')


is_x86 = env['X86'] == '1'
is_from_ci = 'from_ci' in sys.argv

if env['IGNORE_BUILD'] == '0':
    os.makedirs('build', exist_ok=True)
    if isWin():
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

    elif isMac():
        msbuild_path = 'msbuild'

    with CurrentDir('build'):

        if isWin() or isMac():
            # for auto restore of .csproj
            wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

        if isWin():
            suffix = ''
            if is_from_ci:
                suffix += ' -D FROM_CI=ON'
            if is_x86:
                call('cmake .. -A Win32 -DBUILD_VIEWER=ON' + suffix)
            else:
                # run tests on x64
                call('cmake .. -A x64 -DBUILD_VIEWER=ON -D BUILD_TEST=ON' + suffix)

        elif isMac():
            call('cmake .. -G "Xcode" -DBUILD_VIEWER=ON')
        elif find_executable('ninja'):
            call('cmake .. -G Ninja -DBUILD_VIEWER=ON')
        else:
            call('cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON')
        call('cmake --build . --config Release')

    if isWin():
        call('build\\nuget.exe restore Dev/Editor/Effekseer.sln')

    if isMac():
        call('dotnet build Dev/Editor/Effekseer/Effekseer.Std.csproj')
        call('dotnet publish Dev/Editor/Effekseer/Effekseer.Std.csproj -c Release --self-contained -r osx.10.11-x64')
        call('cp -r Dev/release/osx.10.11-x64/publish/* Dev/release/')
        call('rm -rf -r Dev/release/osx.10.11-x64')

    elif isWin():
        if is_x86:
            call('"' + msbuild_path + '"' +
                 ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86')
            call('"' + msbuild_path + '"' +
                 ' Dev/Editor/EffekseerCoreGUI/EffekseerCoreGUI.csproj /t:build /p:Configuration=Release /p:Platform=x86')
            call('"' + msbuild_path + '"' +
                 ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86')
        else:
            call('"' + msbuild_path + '"' +
                 ' Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64')
            call('"' + msbuild_path + '"' +
                 ' Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64')
    else:
        call('dotnet build Dev/Editor/Effekseer/Effekseer.Std.csproj')
        call('dotnet publish Dev/Editor/Effekseer/Effekseer.Std.csproj -c Release --self-contained -r linux-x64')
        call('chmod +x Dev/release/tools/fbxToEffekseerCurveConverter')
        call('chmod +x Dev/release/tools/fbxToEffekseerModelConverter')
        call('chmod +x Dev/release/tools/libfbxsdk.so')
        call('cp -r Dev/release/linux-x64/publish/* Dev/release/')
        call('rm -rf -r Dev/release/linux-x64')

if env['PACKAGEING_FOR_MAC'] == '1' and isMac():
    cd('Dev')
    mkdir('Mac/Effekseer.app/Contents/Resources/')
    distutils.dir_util.copy_tree('release/', 'Mac/Effekseer.app/Contents/Resources/')

    call('chmod +x Mac/Effekseer.app/Contents/MacOS/script.sh')
    call('chmod +x Mac/Effekseer.app/Contents/Resources/tools/fbxToEffekseerCurveConverter')
    call('chmod +x Mac/Effekseer.app/Contents/Resources/tools/fbxToEffekseerModelConverter')

    os.makedirs('Mac/Package', exist_ok=True)

    distutils.dir_util.copy_tree('Mac/Effekseer.app', 'Mac/Package/Effekseer.app')
    call('ln -s /Applications Applications > /dev/null 2>&1')
    call('mv Applications Mac/Package/')
    call('hdiutil create Effekseer.dmg -volname "Effekseer" -srcfolder "Mac/Package"')

    cd('../')
    os.makedirs('EffekseerTool', exist_ok=True)
    shutil.copy('Dev/Effekseer.dmg', 'EffekseerTool/')
    shutil.copy('docs/Help_Ja.html', 'EffekseerTool/')
    shutil.copy('docs/Help_En.html', 'EffekseerTool/')
    shutil.copy('LICENSE_TOOL', 'EffekseerTool/LICENSE_TOOL')
    shutil.copy('readme_tool_mac.txt', 'EffekseerTool/readme.txt')

    os.makedirs('EffekseerTool/Sample/', exist_ok=True)
    distutils.dir_util.copy_tree('Release/Sample', 'EffekseerTool/Sample')
    distutils.dir_util.copy_tree(
        'ResourceData/samples', 'EffekseerTool/Sample')
    shutil.copy('docs/readme_sample.txt', 'EffekseerTool/Sample/readme.txt')

