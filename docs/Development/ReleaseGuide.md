# Release Guide

This document describes release steps for Effekseer packages.

## Pre-release checks

- Verify numeric keypad Enter works on Windows.
- Verify text boxes can be edited with single click in Effekseer.
- Verify `Ctrl + S` save works in Effekseer Editor.

These behaviors depend on customizations in the imgui submodule.
Details are documented in `docs/Development/ImGuiCustomization.md`.

### macOS launcher and signing

For both arm64 on Apple Silicon and x64 on Intel or Rosetta:

- Launch the Developer ID signed app from the final DMG.
- Load an effect, edit it, save to a Unicode/space-containing path, and reopen it.
- Open Material Editor, edit a material, and confirm the editor receives the change.
- Connect a network client and server and send an updated effect.
- Confirm `python3 Script/Notarization_Mac/sign_app.py /path/to/Effekseer.app --verify-only`
  succeeds, including signatures for `createdump` and all native tools.
- Confirm `codesign --verify --deep --strict`, `spctl --assess --type execute`,
  and `xcrun stapler validate` succeed for the distributed app; also validate the DMG ticket.
- Retain the app and DMG notarization logs. Both must be Accepted with no issues.

CI checks signing policy and performs a headless project save/load/export test on
copies of the signed packages. These checks do not exercise the GUI, Material
Editor or networking. Record those results on the release; do not infer them from
successful signing or the headless test.

## Package generation

### Tool package

Run from repository root:

```bash
python3 release_tool.py
```

- Output directory default: `EffekseerTool`
- Override output directory: set `RDIR`

Example:

```bash
RDIR=EffekseerTool python3 release_tool.py
```

### C++ package

Run from repository root:

```bash
python3 release_cpp.py
```

- Output directory default: `EffekseerForCpp`
- Override output directory: set `RDIR_R`

Example:

```bash
RDIR_R=EffekseerForCpp python3 release_cpp.py
```

## Notes

- Windows packaging historically referenced `release.bat`, but current repository release scripts are `release_tool.py` and `release_cpp.py`.
- If release scripts change, update this file together with script changes.

## Linux reproducibility and release verification

The workflow pins the official Rocky Linux 8 container by digest, GCC RPM version
and CMake version (`Script/install_linux_toolchain.sh`), and .NET SDK version
(`.github/workflows/build.yml`). Update the pins deliberately and rerun the checks.
The package includes `build-info/` with the source commit, installed RPM versions,
and CMake/.NET information. Other editor dependencies are not fully locked; this
is not a claim that the entire editor or every platform ZIP is byte-reproducible.

Two separate container jobs copy launcher sources into fresh, differently named
directories and build Release and RelWithDebInfo. CI tests literal argument
forwarding, empty arguments, spawn failures, exit codes and signals, then compares
both builds byte-for-byte and checks their SHA-256 hashes. The packaged Release
launcher must also match. Source/build paths are remapped, date/time macros are
rejected, and `SOURCE_DATE_EPOCH` comes from the source commit. No executable
compression is used.

To reproduce the comparison locally with Docker, run from the repository root:

```sh
mkdir -p build-launcher-checks
for copy in a b; do
  docker run --rm --platform linux/amd64 \
    -v "$PWD:/src:ro" -v "$PWD/build-launcher-checks:/out" \
    -e SOURCE_DATE_EPOCH="$(git log -1 --format=%ct)" \
    rockylinux:8@sha256:9794037624aaa6212aeada1d28861ef5e0a935adaf93e4ef79837119f2a2d04c \
    sh -c 'cd /src && sh Script/install_linux_toolchain.sh && python3.12 Script/build_linux_launcher.py "/out/$1"' sh "$copy"
done
for config in Release RelWithDebInfo; do
  cmp "build-launcher-checks/a/$config/EffekseerLauncher" "build-launcher-checks/b/$config/EffekseerLauncher"
  sha256sum build-launcher-checks/*/"$config"/EffekseerLauncher
done
```

CI packages distribution directories with `Script/package_release.py`, sorting
entries and normalizing timestamps and permissions. It publishes `SHA256SUMS`
alongside the ZIPs. On push/release builds, GitHub's Sigstore-backed attestation
covers both the ZIPs and the checksum manifest; `provenance.sigstore.json` is
included in `EffekseerReleaseAll` and GitHub Release assets. Pull-request builds
produce checksums without signing attestations.

Download the archive, `SHA256SUMS`, and `provenance.sigstore.json` from the same
release. Verify both content and workflow identity before extracting:

```sh
sha256sum --check --ignore-missing SHA256SUMS
gh attestation verify EffekseerVERSIONLinux.zip \
  --repo effekseer/Effekseer \
  --signer-workflow effekseer/Effekseer/.github/workflows/build.yml \
  --bundle provenance.sigstore.json
gh attestation verify SHA256SUMS \
  --repo effekseer/Effekseer \
  --signer-workflow effekseer/Effekseer/.github/workflows/build.yml \
  --bundle provenance.sigstore.json
```

Replace `VERSION` with the release version. For a published release, also check
the attestation's source commit/ref against the intended release tag. Checksums
alone detect corruption but do not authenticate the publisher. GitHub documents
the [attestation format and verification](https://docs.github.com/en/actions/how-tos/secure-your-work/use-artifact-attestations/use-artifact-attestations).
