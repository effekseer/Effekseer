cmake -B build_macosx -S . -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cd build_macosx
cmake --build . --config Release
