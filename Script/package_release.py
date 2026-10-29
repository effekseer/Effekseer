#!/usr/bin/env python3
"""Create release ZIPs with stable metadata and a SHA-256 manifest."""

import argparse
import hashlib
import os
from pathlib import Path
import stat
import time
import zipfile


def package(source, destination, epoch):
    timestamp = time.gmtime(max(epoch, 315532800))[:6]  # ZIP starts in 1980.
    with zipfile.ZipFile(destination, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for path in [source, *sorted(source.rglob("*"))]:
            name = path.relative_to(source.parent).as_posix()
            mode = path.lstat().st_mode
            if stat.S_ISLNK(mode):
                data = os.readlink(path).encode("utf-8")
                attributes = stat.S_IFLNK | 0o777
            elif path.is_dir():
                name += "/"
                data = b""
                attributes = stat.S_IFDIR | 0o755
            else:
                data = path.read_bytes()
                attributes = stat.S_IFREG | (0o755 if mode & 0o111 else 0o644)
            info = zipfile.ZipInfo(name, timestamp)
            info.create_system = 3
            info.external_attr = attributes << 16
            archive.writestr(info, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    parser.add_argument("--epoch", type=int, required=True)
    args = parser.parse_args()
    sources = sorted(path for path in args.source.iterdir() if path.is_dir())
    if not sources:
        parser.error("no release directories found")
    args.destination.mkdir(parents=True, exist_ok=True)
    checksums = []
    for source in sources:
        output = args.destination / (source.name + ".zip")
        package(source, output, args.epoch)
        digest = hashlib.sha256()
        with output.open("rb") as stream:
            for block in iter(lambda: stream.read(1024 * 1024), b""):
                digest.update(block)
        checksums.append(f"{digest.hexdigest()}  {output.name}\n")
    (args.destination / "SHA256SUMS").write_text("".join(checksums), encoding="utf-8")


if __name__ == "__main__":
    main()
