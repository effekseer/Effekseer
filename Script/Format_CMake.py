#!/usr/bin/env python3
import os
import subprocess
import sys


def is_ignored_path(root_dir, path):
    rel = os.path.relpath(path, root_dir)
    parts = rel.split(os.sep)
    if ".git" in parts:
        return True
    if "build" in parts:
        return True
    if "Downloads" in parts:
        return True
    if parts[:3] == ["Dev", "Cpp", "3rdParty"]:
        return True
    return False


def find_cmake_files(root_dir):
    for current_root, dirs, files in os.walk(root_dir):
        if is_ignored_path(root_dir, current_root):
            dirs[:] = []
            continue
        for name in files:
            if name == "CMakeLists.txt" or name.endswith(".cmake"):
                path = os.path.join(current_root, name)
                if not is_ignored_path(root_dir, path):
                    yield path


def main():
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    files = list(find_cmake_files(root_dir))
    if not files:
        return 0
    command = ["cmake-format", "-i"]
    for path in files:
        subprocess.run(command + [path], check=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
