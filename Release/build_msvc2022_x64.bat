mkdir build_msvc2022_x64
cd build_msvc2022_x64
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=../install_msvc2022_x64 -DUSE_LLGI=ON -DBUILD_DX12=ON .. -DCMAKE_DEBUG_POSTFIX=d
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL
