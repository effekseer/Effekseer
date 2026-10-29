#!/usr/bin/env python3
"""Build launcher artifacts in a fresh source/build tree for CI comparison."""

import argparse
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    env = dict(os.environ, LC_ALL="C", TZ="UTC")
    if not env.get("SOURCE_DATE_EPOCH"):
        env["SOURCE_DATE_EPOCH"] = subprocess.check_output(
            ["git", "log", "-1", "--format=%ct"], cwd=root, text=True).strip()
    args.output.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="effekseer-launcher-") as temp:
        source = Path(temp) / "source"
        source.mkdir()
        original = root / "Tool/EffekseerLauncher"
        shutil.copy2(original / "CMakeLists.txt", source)
        shutil.copytree(original / "src", source / "src")
        for configuration in ("Release", "RelWithDebInfo"):
            build = Path(temp) / configuration
            subprocess.run(["cmake", "-S", str(source), "-B", str(build),
                            "-G", "Unix Makefiles", "-DCMAKE_C_COMPILER=gcc",
                            "-DCMAKE_CXX_COMPILER=g++", f"-DCMAKE_BUILD_TYPE={configuration}"],
                           env=env, check=True)
            subprocess.run(["cmake", "--build", str(build)], env=env, check=True)
            executable = build / "EffekseerLauncher"
            subprocess.run([sys.executable, str(root / "Script/test_launcher.py"), str(executable)],
                           env=env, check=True)
            output = args.output / configuration
            output.mkdir(exist_ok=True)
            shutil.copy2(executable, output / executable.name)
            print(configuration, hashlib.sha256(executable.read_bytes()).hexdigest(), flush=True)


if __name__ == "__main__":
    main()
