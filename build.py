import os
import platform
import shutil
import subprocess
import sys
import urllib.request
from pathlib import Path



def run_command(cmd, env=None):
    """Run a shell command and raise an exception if it fails."""

    print(cmd)
    completed = subprocess.run(cmd, shell=True, env=env)
    if completed.returncode != 0:
        raise RuntimeError(f"Failed {cmd}")


def copytree_with_ext(src, dst, exts):
    """Copy files from *src* to *dst* filtering by extension."""

    src_path = Path(src)
    for file in src_path.rglob('*'):
        if file.suffix not in exts:
            continue
        relative = file.relative_to(src_path)
        dest = Path(dst) / relative
        dest.parent.mkdir(parents=True, exist_ok=True)
        print(f"{file}\t:\t{dest}")
        shutil.copy(file, dest)


def is_windows():
    return platform.system() == 'Windows'


def is_mac():
    return platform.system() == 'Darwin'

def is_linux():
    return platform.system() == 'Linux'

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


def copy_tree(src, dst, change=False, ignore=None):
    """Recursively copy *src* to *dst*, overwriting existing files."""

    print(f"copying tree from {src} to {dst}")
    if change and os.path.exists(dst):
        rmdir(dst)

    shutil.copytree(src, dst, dirs_exist_ok=True, ignore=ignore)


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


def main():
    is_x86 = env['X86'] == '1'
    is_from_ci = 'from_ci' in sys.argv

    if is_windows():
        with CurrentDir('Tool/EffekseerLauncher'):
            run_command('call build_windows.bat')

    if is_mac():
        with CurrentDir('Tool/EffekseerLauncher'):
            run_command('sh build_macosx.sh')

    if is_linux():
        with CurrentDir('Tool/EffekseerLauncher'):
            run_command('sh build_linux.sh')

    if env['IGNORE_BUILD'] == '0':
        os.makedirs('build', exist_ok=True)

        with CurrentDir('build'):

            if is_windows() or is_mac():
                # for auto restore of .csproj
                wget(r'https://dist.nuget.org/win-x86-commandline/v5.1.0/nuget.exe')

            if is_windows():
                suffix = ''
                if is_from_ci:
                    suffix += ' -D FROM_CI=ON'

                # run tests on x64
                run_command('cmake .. -A x64 -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON' + suffix)

            elif is_mac():
                run_command('cmake .. -G "Xcode" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON')
            elif shutil.which('ninja'):
                run_command('cmake .. -G Ninja -DBUILD_VIEWER=ON -D BUILD_TEST=ON -D BUILD_EXAMPLES=ON')
            else:
                run_command('cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON')
            run_command('cmake --build . --config Release')

        if is_mac():
            run_command('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
            run_command('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r osx.10.11-x64')
            run_command('cp -r Dev/release/osx.10.11-x64/publish/* Dev/release/')
            run_command('rm -rf -r Dev/release/osx.10.11-x64')

        elif is_windows():
            run_command('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
            run_command('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r win-x64')
            shutil.copytree('Dev/release/win-x64/publish', 'Dev/release', dirs_exist_ok=True)
            shutil.rmtree('Dev/release/win-x64')
        else:
            run_command('dotnet build Dev/Editor/Effekseer/Effekseer.csproj')
            run_command('dotnet publish Dev/Editor/Effekseer/Effekseer.csproj -c Release --self-contained -r linux-x64')
            run_command('chmod +x Dev/release/Effekseer')
            run_command('chmod +x Dev/release/EffekseerMaterialEditor')
            run_command('chmod +x Dev/release/tools/fbxToEffekseerCurveConverter')
            run_command('chmod +x Dev/release/tools/fbxToEffekseerModelConverter')
            run_command('chmod +x Dev/release/tools/libfbxsdk.so')
            run_command('cp -r Dev/release/linux-x64/publish/* Dev/release/')
            run_command('rm -rf -r Dev/release/linux-x64')

    if env['PACKAGEING_FOR_MAC'] == '1' and is_mac():
        cd('Dev')
        mkdir('Mac/Effekseer.app/Contents/Resources/')
        copy_tree('release/', 'Mac/Effekseer.app/Contents/Resources/')

        mkdir('Mac/Effekseer.app/Contents/MacOS/')
        shutil.copy('../Tool/EffekseerLauncher/build_macosx/EffekseerLauncher', 'Mac/Effekseer.app/Contents/MacOS/')

        run_command('chmod +x Mac/Effekseer.app/Contents/Resources/tools/fbxToEffekseerCurveConverter')
        run_command('chmod +x Mac/Effekseer.app/Contents/Resources/tools/fbxToEffekseerModelConverter')

        os.makedirs('Mac/Package', exist_ok=True)

        copy_tree('Mac/Effekseer.app', 'Mac/Package/Effekseer.app')
        link = Path('Applications')
        if link.exists() or link.is_symlink():
            link.unlink()
        link.symlink_to('/Applications', target_is_directory=True)
        shutil.move('Applications', 'Mac/Package/')
        run_command('hdiutil create Effekseer.dmg -volname "Effekseer" -srcfolder "Mac/Package"')

        cd('../')
        os.makedirs('EffekseerTool', exist_ok=True)
        shutil.copy('Dev/Effekseer.dmg', 'EffekseerTool/')
        shutil.copy('docs/Help_Ja.html', 'EffekseerTool/')
        shutil.copy('docs/Help_En.html', 'EffekseerTool/')
        shutil.copy('LICENSE_TOOL', 'EffekseerTool/LICENSE_TOOL')
        shutil.copy('readme_tool_mac.txt', 'EffekseerTool/readme.txt')

        os.makedirs('EffekseerTool/Sample/', exist_ok=True)
        copy_tree('Release/Sample', 'EffekseerTool/Sample')
        copy_tree('ResourceData/samples', 'EffekseerTool/Sample')
        shutil.copy('docs/readme_sample.txt', 'EffekseerTool/Sample/readme.txt')


if __name__ == '__main__':
    main()

