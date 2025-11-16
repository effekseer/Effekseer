echo set current directory
cd /d %~dp0

mkdir ..\build

cd /d ..\build

cmake -A x64 -D BUILD_VIEWER=ON -D BUILD_EDITOR=ON -D BUILD_TEST=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ../

pause