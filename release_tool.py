#!/usr/bin/env python3
import shutil
import sys
import stat
from pathlib import Path
import os


def copy_patterns(src: Path, dst: Path, patterns):
    """Copy files matching *patterns* from *src* to *dst* recursively."""
    for pattern in patterns:
        for f in src.rglob(pattern):
            if f.is_file():
                rel = f.relative_to(src)
                dest = dst / rel
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(f, dest)


def main():
    rdir = Path(os.environ.get("RDIR", "EffekseerTool"))
    if rdir.exists():
        shutil.rmtree(rdir)

    is_windows = sys.platform.startswith("win")
    is_linux = sys.platform.startswith("linux")

    bin_dir = rdir / "Tool" / "bin"
    script_dir = bin_dir / "scripts" / "export"
    tools_dir = bin_dir / "tools"
    res_dir = bin_dir / "resources"

    script_dir.mkdir(parents=True, exist_ok=True)
    tools_dir.mkdir(parents=True, exist_ok=True)
    res_dir.mkdir(parents=True, exist_ok=True)

    print("Compile Editor")

    print("Copy application")
    shutil.copytree("Dev/release", bin_dir, dirs_exist_ok=True)
    if is_linux:
        shutil.rmtree(bin_dir / "linux-x64", ignore_errors=True)
        shutil.rmtree(bin_dir / "publish", ignore_errors=True)

    runtimes_src = Path("Dev/release/runtimes")
    if runtimes_src.exists():
        shutil.copytree(runtimes_src, bin_dir / "runtimes", dirs_exist_ok=True)

    script_src = Path("Dev/release/scripts/export/Default.cs")
    if script_src.exists():
        shutil.copy(script_src, script_dir / "Default.cs")

    if is_windows:
        tool_names = [
            "fbxToEffekseerCurveConverter.exe",
            "fbxToEffekseerModelConverter.exe",
            "libfbxsdk.dll",
        ]
    else:
        tool_names = [
            "fbxToEffekseerCurveConverter",
            "fbxToEffekseerModelConverter",
            "libfbxsdk.so",
        ]
    for f in tool_names:
        src = Path("Dev/release/tools") / f
        if src.exists():
            shutil.copy(src, tools_dir / f)

    if is_windows:
        compilers = ["DX9", "DX11", "DX12", "GL", "Metal"]
        for compiler in compilers:
            name = f"EffekseerMaterialCompiler{compiler}.dll"
            src = Path("Dev/release/tools") / name
            if src.exists():
                shutil.copy(src, tools_dir / name)
    else:
        for compiler in ["GL", "Metal"]:
            dll = Path("Dev/release/tools") / f"EffekseerMaterialCompiler{compiler}.dll"
            so = Path("Dev/release/tools") / f"libEffekseerMaterialCompiler{compiler}.so"
            if dll.exists():
                shutil.copy(dll, tools_dir / dll.name)
            if so.exists():
                shutil.copy(so, tools_dir / so.name)

    shutil.copytree("Dev/release/resources", res_dir, dirs_exist_ok=True)

    if is_windows:
        launcher = Path("Tool/EffekseerLauncher/build_windows/Release/EffekseerLauncher.exe")
        dest = rdir / "Tool" / "Effekseer.exe"
        if launcher.exists():
            shutil.copy(launcher, dest)
    else:
        launcher = Path("Tool/EffekseerLauncher/build_linux/EffekseerLauncher")
        dest = rdir / "Tool" / "Effekseer"
        if launcher.exists():
            shutil.copy(launcher, dest)
            dest.chmod(dest.stat().st_mode | stat.S_IEXEC)

    print("Sample")
    sample_dir = rdir / "Sample"
    sample_dir.mkdir(parents=True, exist_ok=True)
    patterns = ["*.efkproj", "*.efkmodel", "*.efkmat", "*.efkefc", "*.txt", "*.png", "*.mqo", "*.fbx"]
    copy_patterns(Path("Release/Sample"), sample_dir, patterns)
    copy_patterns(Path("ResourceData/samples"), sample_dir, patterns)

    print("Readme")
    shutil.copy("readme_tool_win.txt", rdir / "readme.txt")
    shutil.copy("docs/readme_sample.txt", sample_dir / "readme.txt")
    shutil.copy("docs/Help_Ja.html", rdir / "Help_Ja.html")
    shutil.copy("docs/Help_En.html", rdir / "Help_En.html")
    shutil.copy("LICENSE_TOOL", rdir / "LICENSE_TOOL")


if __name__ == "__main__":
    main()

