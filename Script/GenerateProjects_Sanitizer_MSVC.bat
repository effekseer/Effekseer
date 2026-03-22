echo set current directory
cd /d %~dp0

mkdir ..\build_sanitizer

cd /d ..\build_sanitizer

cmake -A x64 -D BUILD_VIEWER=ON -D BUILD_EDITOR=ON -D BUILD_TEST=ON -D SANITIZE_ENABLED=ON ../

pause