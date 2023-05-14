mkdir build_msvc2017_x86
cd build_msvc2017_x86
cmake -G "Visual Studio 15 2017" -A Win32 -DCMAKE_INSTALL_PREFIX=../install_msvc2017_x86 -DUSE_LLGI=ON -DBUILD_DX12=ON .. -DCMAKE_DEBUG_POSTFIX=d
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL
