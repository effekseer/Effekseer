mkdir build_macOS
cd build_macOS
cmake -G "Xcode" -DCMAKE_INSTALL_PREFIX=../install_macOS ..
cmake --build . --config Debug --target install
cmake --build . --config Release --target install
