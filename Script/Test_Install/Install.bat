cmake -G "Visual Studio 17 2022" -A x64 -D "CMAKE_INSTALL_PREFIX=install/" -D CMAKE_DEBUG_POSTFIX=d -S ../../ -B build
cmake --build build/ --config Debug --target INSTALL
cmake --build build/ --config Release --target INSTALL