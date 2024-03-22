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

if isWin():
    with CurrentDir('Tool/EffekseerLauncher'):
        call('call build_windows.bat')

if isMac():
    with CurrentDir('Tool/EffekseerLauncher'):
        call('sh build_macosx.sh')

if env['IGNORE_BUILD'] == '0':
    os.makedirs('build', exist_ok=True)

    with CurrentDir('build'):

        if isWin() or isMac():
            # for auto restore of .csproj
            wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

        if isWin():
            suffix = ''
            if is_from_ci:
                suffix += ' -D FROM_CI=ON'
            
            # run tests on x64
            call('cmake .. -A x64 -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON' + suffix)

        elif isMac():
            call('cmake .. -G "Xcode" -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON')
        elif find_executable('ninja'):
            call('cmake .. -G Ninja -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON')
        else:
            call('cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON')
        call('cmake --build . --config Release')

    if isMac():
        call('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
        call('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r osx.10.11-x64')
        call('cp -r Dev/release/osx.10.11-x64/publish/* Dev/release/')
        call('rm -rf -r Dev/release/osx.10.11-x64')

    elif isWin():
        call('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
        call('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r win-x64')
        shutil.copytree('Dev/release/win-x64/publish', 'Dev/release', dirs_exist_ok=True)
        shutil.rmtree('Dev/release/win-x64')
    else:
        call('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
        call('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r linux-x64')
        call('chmod +x Dev/release/Effekseer')
        call('chmod +x Dev/release/EffekseerMaterialEditor')
        call('chmod +x Dev/release/tools/fbxToEffekseerCurveConverter')
        call('chmod +x Dev/release/tools/fbxToEffekseerModelConverter')
        call('chmod +x Dev/release/tools/libfbxsdk.so')
        call('cp -r Dev/release/linux-x64/publish/* Dev/release/')
        call('rm -rf -r Dev/release/linux-x64')
    
if env['PACKAGEING_FOR_MAC'] == '1' and isMac():
    cd('Dev')
    mkdir('Mac/Effekseer.app/Contents/Resources/')
    distutils.dir_util.copy_tree('release/', 'Mac/Effekseer.app/Contents/Resources/')

    mkdir('Mac/Effekseer.app/Contents/MacOS/')
    shutil.copy('../Tool/EffekseerLauncher/build_macosx/EffekseerLauncher', 'Mac/Effekseer.app/Contents/MacOS/')

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

