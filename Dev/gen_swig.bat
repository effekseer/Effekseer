@echo off
setlocal
pushd "%~dp0"

swig -c++ -csharp -namespace Effekseer.swig -dllimport Viewer -o Cpp\Viewer\dll_cs.cxx -outdir Editor\EffekseerCoreGUI\swig viewer.i || exit /b 1
python "%~dp0normalize_swig_outputs.py" "Editor\EffekseerCoreGUI\swig\*.cs" || exit /b 1
python "%~dp0normalize_swig_outputs.py" --bom "Cpp\Viewer\dll_cs.c*" || exit /b 1

popd
pause
endlocal
