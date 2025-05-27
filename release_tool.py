#!/usr/bin/env python3
import shutil
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

    shutil.copy("Dev/release/scripts/export/Default.cs", script_dir / "Default.cs")

    for f in [
        "fbxToEffekseerCurveConverter.exe",
        "fbxToEffekseerModelConverter.exe",
        "libfbxsdk.dll",
    ]:
        shutil.copy(Path("Dev/release/tools") / f, tools_dir / f)

    for compiler in ["DX9", "DX11", "DX12", "GL", "Metal"]:
        name = f"EffekseerMaterialCompiler{compiler}.dll"
        shutil.copy(Path("Dev/release/tools") / name, tools_dir / name)

    shutil.copytree("Dev/release/resources", res_dir, dirs_exist_ok=True)

    launcher = Path("Tool/EffekseerLauncher/build_windows/Release/EffekseerLauncher.exe")
    shutil.copy(launcher, rdir / "Tool" / "Effekseer.exe")

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

