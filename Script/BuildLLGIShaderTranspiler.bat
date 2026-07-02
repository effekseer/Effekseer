@echo off
setlocal

set "CONFIG=Release"
set "PLATFORM=x64"

if not "%~1"=="" set "CONFIG=%~1"
if not "%~2"=="" set "PLATFORM=%~2"

if /I "%CONFIG%"=="Debug" set "CONFIG=Debug"
if /I "%CONFIG%"=="Release" set "CONFIG=Release"

if not "%CONFIG%"=="Debug" if not "%CONFIG%"=="Release" goto Usage

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"

set "LLGI_DIR=%ROOT_DIR%\Dev\Cpp\3rdParty\LLGI"
set "BUILD_DIR=%LLGI_DIR%\build-tool"

if not exist "%LLGI_DIR%\CMakeLists.txt" (
  echo LLGI directory was not found: "%LLGI_DIR%"
  exit /b 1
)

cmake --version >nul 2>nul
if errorlevel 1 (
  echo CMake was not found. Please install CMake or add it to PATH.
  exit /b 1
)

echo Configuring LLGI ShaderTranspiler...
cmake -S "%LLGI_DIR%" -B "%BUILD_DIR%" -A %PLATFORM% -D BUILD_TOOL=ON -D BUILD_TEST=OFF -D BUILD_EXAMPLE=OFF
if errorlevel 1 exit /b 1

echo Building ShaderTranspiler (%CONFIG%)...
cmake --build "%BUILD_DIR%" --target ShaderTranspiler --config "%CONFIG%"
if errorlevel 1 exit /b 1

set "BUILT_EXE=%BUILD_DIR%\tools\ShaderTranspiler\%CONFIG%\ShaderTranspiler.exe"
if not exist "%BUILT_EXE%" (
  echo ShaderTranspiler.exe was not found: "%BUILT_EXE%"
  exit /b 1
)

echo Copying ShaderTranspiler.exe...
call :CopyExe "%BUILT_EXE%" "%BUILD_DIR%\tools\ShaderTranspiler\Debug\ShaderTranspiler.exe"
if errorlevel 1 exit /b 1
call :CopyExe "%BUILT_EXE%" "%LLGI_DIR%\build\tools\ShaderTranspiler\Debug\ShaderTranspiler.exe"
if errorlevel 1 exit /b 1
call :CopyExe "%BUILT_EXE%" "%ROOT_DIR%\build\Dev\Cpp\3rdParty\LLGI\tools\ShaderTranspiler\Debug\ShaderTranspiler.exe"
if errorlevel 1 exit /b 1

echo Done.
echo Built: "%BUILT_EXE%"
exit /b 0

:Usage
echo Usage: %~nx0 [Debug^|Release] [x64^|Win32]
exit /b 1

:CopyExe
set "SRC=%~1"
set "DST=%~2"
if /I "%SRC%"=="%DST%" (
  echo Skipped: "%DST%"
  exit /b 0
)
for %%D in ("%DST%") do (
  if not exist "%%~dpD" mkdir "%%~dpD"
)
copy /Y "%SRC%" "%DST%" >nul
if errorlevel 1 (
  echo Failed to copy to "%DST%"
  exit /b 1
)
echo Copied: "%DST%"
exit /b 0
