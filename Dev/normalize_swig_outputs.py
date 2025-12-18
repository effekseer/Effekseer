#!/usr/bin/env python3

import argparse
import glob
import pathlib
import sys
from typing import Iterable, List


def normalize_line_endings(text: str) -> str:
    """Convert CRLF/CR newlines to LF only."""
    return text.replace("\r\n", "\n").replace("\r", "\n")


def resolve_paths(raw_paths: Iterable[str]) -> List[pathlib.Path]:
    """Expand glob patterns while preserving literal paths when needed."""
    resolved: List[pathlib.Path] = []
    for raw in raw_paths:
        matches = glob.glob(raw)
        if matches:
            resolved.extend(pathlib.Path(match) for match in matches)
        else:
            resolved.append(pathlib.Path(raw))
    # Use sorted paths for deterministic output when globs are expanded.
    return sorted(pathlib.Path(path) for path in resolved)


def convert_file(path: pathlib.Path, bom: bool) -> None:
    if not path.exists():
        raise FileNotFoundError(f"{path} not found")
    if path.is_dir():
        raise IsADirectoryError(f"{path} is a directory")

    raw = path.read_bytes()
    try:
        text = raw.decode("utf-8-sig")
    except UnicodeDecodeError:
        text = raw.decode("utf-8", errors="replace")

    text = normalize_line_endings(text)
    encoding = "utf-8-sig" if bom else "utf-8"
    with path.open("w", encoding=encoding, newline="\n") as fp:
        fp.write(text)


def main(argv: Iterable[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Normalize line endings to LF and re-encode files.",
    )
    parser.add_argument(
        "--bom",
        action="store_true",
        help="Write files as UTF-8 with BOM (utf-8-sig). Without this, UTF-8 without BOM is used.",
    )
    parser.add_argument(
        "paths",
        nargs="+",
        help="Target files or glob patterns to convert.",
    )

    args = parser.parse_args(list(argv))
    targets = resolve_paths(args.paths)

    if not targets:
        print("No targets to process.", file=sys.stderr)
        return 1

    for target in targets:
        try:
            convert_file(target, bom=args.bom)
            print(f"Normalized {target}")
        except Exception as exc:  # noqa: BLE001
            print(f"Failed to normalize {target}: {exc}", file=sys.stderr)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
