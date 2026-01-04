@echo off

echo Preset numbers for building Effekseer for cpp:
echo [1] Visual Studio 2017(x86)
echo [2] Visual Studio 2017(x64)
echo [3] Visual Studio 2019(x86)
echo [4] Visual Studio 2019(x64)
echo [5] Visual Studio 2022(x86)
echo [6] Visual Studio 2022(x64)
echo [7] Visual Studio 2019(x86) with Vulkan
echo [8] Visual Studio 2019(x64) with Vulkan
echo [9] Visual Studio 2022(x86) with Vulkan
echo [10] Visual Studio 2022(x64) with Vulkan
echo [11] Visual Studio 2026(x86)
echo [12] Visual Studio 2026(x64)
echo [13] Visual Studio 2026(x86) with Vulkan
echo [14] Visual Studio 2026(x64) with Vulkan
echo ------------------------------------

:REENTER_PRESET
set /p build_preset="Enter preset number: "

set enable_vulkan=OFF

if %build_preset%==1 (
    set generator="Visual Studio 15 2017"
    set generate_dir="msvc2017_x86"
    set platform="Win32"
) else if %build_preset%==2 (
    set generator="Visual Studio 15 2017"
    set generate_dir="msvc2017_x64"
    set platform="x64"
) else if %build_preset%==3 (
    set generator="Visual Studio 16 2019"
    set generate_dir="msvc2019_x86"
    set platform="Win32"
) else if %build_preset%==4 (
    set generator="Visual Studio 16 2019"
    set generate_dir="msvc2019_x64"
    set platform="x64"
) else if %build_preset%==5 (
    set generator="Visual Studio 17 2022"
    set generate_dir="msvc2022_x86"
    set platform="Win32"
) else if %build_preset%==6 (
    set generator="Visual Studio 17 2022"
    set generate_dir="msvc2022_x64"
    set platform="x64"
) else if %build_preset%==7 (
    set generator="Visual Studio 16 2019"
    set generate_dir="vulkan_msvc2019_x86"
    set platform="Win32"
    set enable_vulkan=ON
) else if %build_preset%==8 (
    set generator="Visual Studio 16 2019"
    set generate_dir="vulkan_msvc2019_x64"
    set platform="x64"
    set enable_vulkan=ON
) else if %build_preset%==9 (
    set generator="Visual Studio 17 2022"
    set generate_dir="vulkan_msvc2022_x86"
    set platform="Win32"
    set enable_vulkan=ON
) else if %build_preset%==10 (
    set generator="Visual Studio 17 2022"
    set generate_dir="vulkan_msvc2022_x64"
    set platform="x64"
    set enable_vulkan=ON
) else if %build_preset%==11 (
    set generator="Visual Studio 18 2026"
    set generate_dir="msvc2026_x86"
    set platform="Win32"
) else if %build_preset%==12 (
    set generator="Visual Studio 18 2026"
    set generate_dir="msvc2026_x64"
    set platform="x64"
) else if %build_preset%==13 (
    set generator="Visual Studio 18 2026"
    set generate_dir="vulkan_msvc2026_x86"
    set platform="Win32"
    set enable_vulkan=ON
) else if %build_preset%==14 (
    set generator="Visual Studio 18 2026"
    set generate_dir="vulkan_msvc2026_x64"
    set platform="x64"
    set enable_vulkan=ON
) else (
    echo Invalid preset number.
    goto REENTER_PRESET
)

:REENTER_RTDLLOPTION
set /p enable_rtdll_input="Enable runtime library DLL option?(y/n): "
if %enable_rtdll_input:Y=Y%==Y (
    set enable_rtdll=ON
) else if %enable_rtdll_input:N=N%==N (
    set enable_rtdll=OFF
) else (
    echo Invalid option.
    goto REENTER_RTDLLOPTION
)

echo - generator: %generator%
echo - generate_dir: %generate_dir%
echo - platform: %platform%
echo - enable_vulkan: %enable_vulkan%
echo - enable_rtdll: %enable_rtdll%
echo ------------------------------------

mkdir build_%generate_dir%
cd build_%generate_dir%
cmake -G %generator% -A %platform% -DCMAKE_INSTALL_PREFIX=../install_%generate_dir% -DUSE_MSVC_RUNTIME_LIBRARY_DLL=%enable_rtdll% -DUSE_LLGI=ON -DBUILD_DX12=ON -DBUILD_VULKAN=%enable_vulkan% .. -DCMAKE_DEBUG_POSTFIX=d
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL

@echo on
pause
