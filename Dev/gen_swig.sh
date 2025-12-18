#!/bin/sh
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PYTHON_BIN=python3
command -v python3 >/dev/null 2>&1 || PYTHON_BIN=python

swig -c++ -csharp -namespace Effekseer.swig -dllimport Viewer \
  -o "${SCRIPT_DIR}/Cpp/Viewer/dll_cs.cxx" \
  -outdir "${SCRIPT_DIR}/Editor/EffekseerCoreGUI/swig" \
  "${SCRIPT_DIR}/viewer.i"

"${PYTHON_BIN}" "${SCRIPT_DIR}/normalize_swig_outputs.py" \
  "${SCRIPT_DIR}/Editor/EffekseerCoreGUI/swig/"*.cs
"${PYTHON_BIN}" "${SCRIPT_DIR}/normalize_swig_outputs.py" --bom \
  "${SCRIPT_DIR}/Cpp/Viewer/dll_cs.c*"
