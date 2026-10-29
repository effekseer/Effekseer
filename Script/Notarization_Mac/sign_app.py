#!/usr/bin/env python3
"""Sign and audit every Mach-O file, granting JIT only to the .NET apphost."""

import argparse
from pathlib import Path
import plistlib
import re
import subprocess


MACHO_MAGIC = {
    b"\xfe\xed\xfa\xce", b"\xce\xfa\xed\xfe",  # 32-bit
    b"\xfe\xed\xfa\xcf", b"\xcf\xfa\xed\xfe",  # 64-bit
    b"\xca\xfe\xba\xbe", b"\xbe\xba\xfe\xca",  # universal
    b"\xca\xfe\xba\xbf", b"\xbf\xba\xfe\xca",  # universal 64-bit
}
DOTNET_HOST = Path("Contents/Resources/Effekseer")
LAUNCHER = Path("Contents/MacOS/EffekseerLauncher")
DOTNET_ENTITLEMENTS = {"com.apple.security.cs.allow-jit": True}


def macho_files(app):
    # Inspect file contents rather than executable bits: native libraries may
    # lack those bits, and executable scripts/data must not be codesigned.
    result = []
    for path in sorted((app / "Contents").rglob("*")):
        if path.is_symlink() or not path.is_file():
            continue
        with path.open("rb") as stream:
            if stream.read(4) in MACHO_MAGIC:
                result.append(path)
    for required in (DOTNET_HOST, LAUNCHER):
        if app / required not in result:
            raise RuntimeError(f"Missing Mach-O executable: {app / required}")
    return result


def verify(app, binaries):
    for path in binaries:
        subprocess.run(["codesign", "--verify", "--strict", str(path)], check=True)
        expected = DOTNET_ENTITLEMENTS if path == app / DOTNET_HOST else {}
        architectures = subprocess.check_output(
            ["lipo", "-archs", str(path)], text=True).split()
        for architecture in architectures:
            result = subprocess.run(
                ["codesign", "--display", "--arch", architecture, "--verbose=2",
                 "--entitlements", ":-", str(path)],
                check=True, capture_output=True)
            actual = plistlib.loads(result.stdout) if result.stdout.strip() else {}
            if actual != expected:
                raise RuntimeError(f"Unexpected entitlements in {path} ({architecture}): {actual}")
            flags = re.search(rb"flags=0x([0-9a-fA-F]+)", result.stderr)
            if not flags or not int(flags[1], 16) & 0x10000:
                raise RuntimeError(f"Hardened Runtime missing from {path} ({architecture})")
    subprocess.run(
        ["codesign", "--verify", "--deep", "--strict", "--verbose=4", str(app)], check=True)


def sign(app, identity):
    binaries = macho_files(app)
    entitlements = Path(__file__).with_name("entitlements-dotnet.plist")
    with entitlements.open("rb") as stream:
        if plistlib.load(stream) != DOTNET_ENTITLEMENTS:
            raise RuntimeError("Unexpected .NET entitlement policy")
    for path in binaries + [app]:
        command = ["codesign", "--force", "--sign", identity, "--options", "runtime",
                   "--timestamp" if identity != "-" else "--timestamp=none"]
        if path == app / DOTNET_HOST:
            command += ["--entitlements", str(entitlements)]
        print(f"Signing {path}", flush=True)
        subprocess.run(command + [str(path)], check=True)
    # Signing the bundle replaces the launcher's signature. Audit afterwards
    # so neither the launcher nor a native helper can retain old exceptions.
    verify(app, binaries)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("app", type=Path)
    parser.add_argument("identity", nargs="?", help="Developer ID, or '-' for local tests")
    parser.add_argument("--verify-only", action="store_true")
    args = parser.parse_args()
    if args.verify_only:
        verify(args.app, macho_files(args.app))
    elif args.identity:
        sign(args.app, args.identity)
    else:
        parser.error("identity is required for signing")


if __name__ == "__main__":
    main()
