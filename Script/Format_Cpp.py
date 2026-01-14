#!/usr/bin/env python3
import os
import subprocess
import sys


CPP_EXTENSIONS = {
    ".cc",
    ".cpp",
    ".cxx",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
    ".inl",
    ".ipp",
    ".mm",
}

IGNORED_DIR_NAMES = {
    ".git",
    "downloads",
    "build",
    "thirdparty",
    "3rdparty",
}

IGNORED_DIR_PREFIXES = {"thirdparty", "3rdparty"}

IGNORED_PATH_SEQUENCES = [
    ["include", "al"],
    ["effekseerresourceconverter", "external"],
]


def is_ignored_path(root_dir, path):
    rel = os.path.relpath(path, root_dir)
    parts = rel.split(os.sep)
    lower_parts = [part.lower() for part in parts]
    if any(part in IGNORED_DIR_NAMES for part in lower_parts):
        return True
    if any(part.startswith(prefix) for part in lower_parts for prefix in IGNORED_DIR_PREFIXES):
        return True
    if any(contains_sequence(lower_parts, sequence) for sequence in IGNORED_PATH_SEQUENCES):
        return True
    if "shaderheader" in lower_parts:
        return True
    if "shaders" in lower_parts:
        return True
    return False


def contains_sequence(parts, sequence):
    if not sequence or len(sequence) > len(parts):
        return False
    limit = len(parts) - len(sequence) + 1
    for index in range(limit):
        if parts[index : index + len(sequence)] == sequence:
            return True
    return False


def find_cpp_files(root_dir):
    for current_root, dirs, files in os.walk(root_dir):
        if is_ignored_path(root_dir, current_root):
            dirs[:] = []
            continue
        for name in files:
            lower_name = name.lower()
            if lower_name in {
                "dll_cs.cxx",
                "dll_cs.h",
                "stb_image_utils.h",
                "profiler_generated.h",
                "reload_generated.h",
            }:
                continue
            _, ext = os.path.splitext(name)
            if ext in CPP_EXTENSIONS:
                path = os.path.join(current_root, name)
                if not is_ignored_path(root_dir, path):
                    yield path


def main():
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    files = list(find_cpp_files(root_dir))
    if not files:
        return 0
    command = ["clang-format", "-i", "-style=file"]
    for path in files:
        print(path)
        subprocess.run(command + [path], check=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
