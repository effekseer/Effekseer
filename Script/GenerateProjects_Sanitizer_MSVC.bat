echo set current directory
cd /d %~dp0

mkdir ..\build_sanitizer

cd /d ..\build_sanitizer

cmake -A Win32 -D BUILD_VIEWER=ON -D BUILD_EDITOR=ON -D BUILD_TEST=ON ../

cd /d %~dp0

python GenerateProjects_Sanitizer_MSVC.py

pause