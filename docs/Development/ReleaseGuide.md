# Release Guide

This document describes release steps for Effekseer packages.

## Pre-release checks

- Verify numeric keypad Enter works on Windows.
- Verify text boxes can be edited with single click in Effekseer.
- Verify `Ctrl + S` save works in Effekseer Editor.

These behaviors depend on customizations in the imgui submodule.
Details are documented in `docs/Development/ImGuiCustomization.md`.

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
