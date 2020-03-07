mkdir build_msvc2017
cd build_msvc2017
cmake -G "Visual Studio 15 2017" -DCMAKE_INSTALL_PREFIX=../install_msvc2017 ..
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL
