rm -rf cmake

mkdir cmake

(cd cmake;
cmake -G "Unix Makefiles" -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_INTERNAL_LOADER:BOOL=OFF -D USE_GLEW_STATIC:BOOL=OFF -D USE_GLEW_DLL:BOOL=OFF ../;
make;)

mkdir lib

cp cmake/libEffekseer.a lib/libEffekseer.a
cp cmake/libEffekseerRendererGL.a lib/libEffekseerRendererGL.a
cp cmake/libEffekseerSoundAL.a lib/libEffekseerSoundAL.a

(cd Test;
cmake -G "Unix Makefiles" ./;
make clean;
make;)


