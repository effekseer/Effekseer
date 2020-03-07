mkdir build_msvc2019
cd build_msvc2019
cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=../install_msvc2019 ..
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL
