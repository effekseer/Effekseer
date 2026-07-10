import os
import platform
import plistlib
import re
import shutil
import subprocess
import sys
import urllib.request
from pathlib import Path



def run_command(cmd, env=None):
    """Run a shell command and raise an exception if it fails."""

    print(cmd if isinstance(cmd, str) else subprocess.list2cmdline(cmd))
    completed = subprocess.run(cmd, shell=isinstance(cmd, str), env=env)
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


def get_editor_version():
    core_cs = Path('Dev/Editor/EffekseerCore/Core.cs')
    text = core_cs.read_text(encoding='utf-8-sig')
    match = re.search(r'public const string Version = "([^"]+)"', text)
    if not match:
        raise RuntimeError(f'Failed to read editor version from {core_cs}')
    return match.group(1)


def update_mac_bundle_version(plist_path, version):
    with plist_path.open('rb') as fp:
        info = plistlib.load(fp)

    info['CFBundleShortVersionString'] = version
    info['CFBundleVersion'] = version

    with plist_path.open('wb') as fp:
        plistlib.dump(info, fp, sort_keys=False)


MAC_RUNTIME_IDENTIFIERS = {
    'arm64': 'osx-arm64',
    'x64': 'osx-x64',
}

MAC_SHARED_RELEASE_PATHS = [
    'Effekseer.config.Dock.xml',
    'EffekseerCore.deps.json',
    'EffekseerCoreGUI.deps.json',
    'EffekseerMaterialEditor',
    'config.network.xml',
    'config.option.xml',
    'config.recent.xml',
    'libViewer.dylib',
    'resources',
    'scripts',
    'tools',
]


def get_mac_publish_dir(runtime_identifier):
    return Path('build/macos-publish') / runtime_identifier


def prepare_mac_publish_dir(runtime_identifier):
    """Create a self-contained editor tree for one macOS architecture."""

    publish_dir = get_mac_publish_dir(runtime_identifier)
    if publish_dir.exists():
        shutil.rmtree(publish_dir)

    # The project's OutputPath causes dotnet to leave RID-specific
    # intermediates under Dev/release even when --output is supplied. Do not
    # let an earlier RID leak into the next architecture's package.
    for rid in MAC_RUNTIME_IDENTIFIERS.values():
        intermediate_dir = Path('Dev/release') / rid
        if intermediate_dir.exists():
            shutil.rmtree(intermediate_dir)

    run_command([
        'dotnet',
        'publish',
        'Dev/Editor/Effekseer/Effekseer.csproj',
        '-c',
        'Release',
        '--self-contained',
        '-r',
        runtime_identifier,
        '--output',
        str(publish_dir),
    ])

    intermediate_dir = Path('Dev/release') / runtime_identifier
    if intermediate_dir.exists():
        shutil.rmtree(intermediate_dir)

    # Native tools and resources are emitted into Dev/release by the C++ and
    # regular .NET builds. Add only files that dotnet publish did not produce;
    # overwriting an existing file can replace a RID-specific runtime with a
    # stale artifact from a previous local build.
    shared_dir = Path('Dev/release')
    for shared_relative in MAC_SHARED_RELEASE_PATHS:
        shared_path = shared_dir / shared_relative
        if not shared_path.exists():
            continue

        sources = shared_path.rglob('*') if shared_path.is_dir() else [shared_path]
        for source in sources:
            relative = source.relative_to(shared_dir)
            destination = publish_dir / relative
            if source.is_dir():
                destination.mkdir(parents=True, exist_ok=True)
            elif not destination.exists():
                destination.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(source, destination)


def make_mac_app(runtime_identifier, editor_version):
    """Build an unsigned, architecture-specific app ready for signing."""

    package_root = Path('build/macos-packaging') / runtime_identifier
    app_path = package_root / 'Effekseer.app'
    if package_root.exists():
        shutil.rmtree(package_root)

    shutil.copytree('Dev/Mac/Effekseer.app', app_path)
    resources_dir = app_path / 'Contents/Resources'
    resources_dir.mkdir(parents=True, exist_ok=True)
    shutil.copytree(get_mac_publish_dir(runtime_identifier), resources_dir, dirs_exist_ok=True)

    macos_dir = app_path / 'Contents/MacOS'
    macos_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(
        'Tool/EffekseerLauncher/build_macosx/EffekseerLauncher',
        macos_dir / 'EffekseerLauncher')

    update_mac_bundle_version(app_path / 'Contents/Info.plist', editor_version)

    for executable in [
        macos_dir / 'EffekseerLauncher',
        resources_dir / 'Effekseer',
        resources_dir / 'EffekseerMaterialEditor',
        resources_dir / 'createdump',
        resources_dir / 'tools/EffekseerResourceConverter',
    ]:
        if executable.exists():
            executable.chmod(executable.stat().st_mode | 0o111)

    return app_path


def populate_mac_tool_dir(tool_dir, app_path):
    """Create one architecture-specific macOS distribution directory."""

    if tool_dir.exists():
        shutil.rmtree(tool_dir)
    tool_dir.mkdir(parents=True)

    dmg_staging = app_path.parent / 'dmg-staging'
    if dmg_staging.exists():
        shutil.rmtree(dmg_staging)
    dmg_staging.mkdir()
    shutil.copytree(app_path, dmg_staging / 'Effekseer.app')
    (dmg_staging / 'Applications').symlink_to('/Applications', target_is_directory=True)

    run_command([
        'hdiutil',
        'create',
        '-ov',
        str(tool_dir / 'Effekseer.dmg'),
        '-volname',
        'Effekseer',
        '-srcfolder',
        str(dmg_staging),
    ])

    shutil.copy('docs/Help_Ja.html', tool_dir)
    shutil.copy('docs/Help_En.html', tool_dir)
    shutil.copy('LICENSE_TOOL', tool_dir / 'LICENSE_TOOL')
    shutil.copy('readme_tool_mac.txt', tool_dir / 'readme.txt')

    sample_dir = tool_dir / 'Sample'
    sample_dir.mkdir(parents=True)
    copy_tree('Release/Sample', sample_dir)
    copy_tree('ResourceData/samples', sample_dir)
    shutil.copy('docs/readme_sample.txt', sample_dir / 'readme.txt')


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
            for runtime_identifier in MAC_RUNTIME_IDENTIFIERS.values():
                prepare_mac_publish_dir(runtime_identifier)

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
            run_command('chmod +x Dev/release/tools/EffekseerResourceConverter')
            # run_command('chmod +x Dev/release/tools/fbxToEffekseerCurveConverter')
            # run_command('chmod +x Dev/release/tools/fbxToEffekseerModelConverter')
            # run_command('chmod +x Dev/release/tools/libfbxsdk.so')
            run_command('cp -r Dev/release/linux-x64/publish/* Dev/release/')
            run_command('rm -rf -r Dev/release/linux-x64')

    if env['PACKAGEING_FOR_MAC'] == '1' and is_mac():
        editor_version = get_editor_version()
        for architecture, runtime_identifier in MAC_RUNTIME_IDENTIFIERS.items():
            publish_dir = get_mac_publish_dir(runtime_identifier)
            if not publish_dir.exists():
                raise RuntimeError(
                    f'Missing {publish_dir}. Run build.py without IGNORE_BUILD first.')

            app_path = make_mac_app(runtime_identifier, editor_version)
            populate_mac_tool_dir(
                Path('EffekseerTool') / f'Mac-{architecture}',
                app_path)


if __name__ == '__main__':
    main()
