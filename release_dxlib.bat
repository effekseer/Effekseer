
SET RDIR_R=EffekseerRuntime_DXLib

rmdir /s /q %RDIR_R%
mkdir %RDIR_R%

mkdir %RDIR_R%\vs2022_x64\
cmake -G "Visual Studio 17 2022" -A x64 -D "CMAKE_INSTALL_PREFIX=%RDIR_R%/vs2022_x64/install/" -D USE_XAUDIO2=ON -D BUILD_EXAMPLES=OFF -D CMAKE_DEBUG_POSTFIX=_d -S ./ -B %RDIR_R%/vs2022_x64/
cmake --build %RDIR_R%\vs2022_x64\ --config Debug --target INSTALL
cmake --build %RDIR_R%\vs2022_x64\ --config Release --target INSTALL

mkdir %RDIR_R%\vs2022_x86\
cmake -G "Visual Studio 17 2022" -A Win32 -D "CMAKE_INSTALL_PREFIX=%RDIR_R%/vs2022_x86/install/" -D USE_XAUDIO2=ON -D BUILD_EXAMPLES=OFF -D CMAKE_DEBUG_POSTFIX=_d -S ./ -B %RDIR_R%/vs2022_x86/
cmake --build %RDIR_R%\vs2022_x86\ --config Debug --target INSTALL
cmake --build %RDIR_R%\vs2022_x86\ --config Release --target INSTALL

mkdir %RDIR_R%\vs2019_x64\
cmake -G "Visual Studio 16" -A x64 -D "CMAKE_INSTALL_PREFIX=%RDIR_R%/vs2019_x64/install/" -D USE_XAUDIO2=ON -D BUILD_EXAMPLES=OFF -D CMAKE_DEBUG_POSTFIX=_d -S ./ -B %RDIR_R%/vs2019_x64/
cmake --build %RDIR_R%\vs2019_x64\ --config Debug --target INSTALL
cmake --build %RDIR_R%\vs2019_x64\ --config Release --target INSTALL

mkdir %RDIR_R%\vs2019_x86\
cmake -G "Visual Studio 16" -A Win32 -D "CMAKE_INSTALL_PREFIX=%RDIR_R%/vs2019_x86/install/" -D USE_XAUDIO2=ON -D BUILD_EXAMPLES=OFF -D CMAKE_DEBUG_POSTFIX=_d -S ./ -B %RDIR_R%/vs2019_x86/
cmake --build %RDIR_R%\vs2019_x86\ --config Debug --target INSTALL
cmake --build %RDIR_R%\vs2019_x86\ --config Release --target INSTALL

pause