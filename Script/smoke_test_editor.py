#!/usr/bin/env python3
"""Check editor startup, project saving/loading and binary export without a GUI.

For macOS, run against a disposable copy of the signed app bundle because the
editor can write settings alongside its executable.
"""

import argparse
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("launcher", type=Path)
    args = parser.parse_args()
    launcher = args.launcher.resolve()
    sample = Path(__file__).resolve().parent.parent / "TestData/Effects/14/Model_Parameters1.efkproj"
    if not sample.is_file():
        raise RuntimeError(f"Missing smoke-test effect: {sample}")
    with tempfile.TemporaryDirectory(prefix="effekseer smoke ") as temp:
        directory = Path(temp)
        project = directory / "保存 test.efkefc"
        empty = directory / "empty.efk"
        first = directory / "first.efk"
        second = directory / "second.efk"
        for arguments in (
            ["-cui", "-e", str(empty)],
            ["-cui", "-in", str(sample), "-o", str(project), "-e", str(first)],
            ["-cui", "-in", str(project), "-e", str(second)],
        ):
            subprocess.run([str(launcher), *arguments], cwd=directory, check=True, timeout=120)
        if (not project.is_file() or not first.read_bytes()
                or first.read_bytes() == empty.read_bytes()
                or first.read_bytes() != second.read_bytes()):
            raise RuntimeError("Project save/load changed the exported effect")
    print("Editor startup, project save/load and export passed")


if __name__ == "__main__":
    main()
