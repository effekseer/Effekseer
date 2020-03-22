mkdir build_macOS
cd build_macOS
cmake -G "Xcode" -DCMAKE_INSTALL_PREFIX=../install_macOS -DUSE_LLGI=ON -DBUILD_METAL=ON .. -DCMAKE_DEBUG_POSTFIX=d
cmake --build . --config Debug --target install
cmake --build . --config Release --target install

