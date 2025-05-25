#!/usr/bin/env python3
import shutil
import os
from pathlib import Path
import argparse


def copy_filtered(src: Path, dst: Path, patterns):
    src = Path(src)
    dst = Path(dst)
    for pattern in patterns:
        for f in src.rglob(pattern):
            if f.is_file():
                rel = f.relative_to(src)
                dest = dst / rel
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(f, dest)


def main():
    parser = argparse.ArgumentParser(description="Prepare EffekseerTool package")
    parser.add_argument("--dest", default=os.environ.get("RDIR", "EffekseerTool"), help="output directory")
    args = parser.parse_args()

    rdir = Path(args.dest).resolve()
    if os.name == "nt":
        bin_dir = rdir / "Tool" / "bin"
    else:
        bin_dir = rdir / "Tool"
    sample_dir = rdir / "Sample"

    if rdir.exists():
        shutil.rmtree(rdir)
    bin_dir.mkdir(parents=True)

    # copy application
    shutil.copytree("Dev/release", bin_dir, dirs_exist_ok=True)
    for d in ["linux-x64", "publish"]:
        shutil.rmtree(bin_dir / d, ignore_errors=True)

    # copy sample resources
    sample_dir.mkdir(parents=True)
    patterns = ["*.efkproj", "*.efkmodel", "*.efkmat", "*.efkefc", "*.txt", "*.png", "*.mqo", "*.fbx"]
    for base in ["Release/Sample", "ResourceData/samples"]:
        copy_filtered(Path(base), sample_dir, patterns)

    # copy readmes and licenses
    shutil.copy("readme_tool_win.txt", rdir / "readme.txt")
    shutil.copy("docs/readme_sample.txt", sample_dir / "readme.txt")
    shutil.copy("docs/Help_Ja.html", rdir / "Help_Ja.html")
    shutil.copy("docs/Help_En.html", rdir / "Help_En.html")
    shutil.copy("LICENSE_TOOL", rdir / "LICENSE_TOOL")


if __name__ == "__main__":
    main()
