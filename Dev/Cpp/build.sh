rm -rf cmake

mkdir cmake

(cd cmake;
cmake -G "Unix Makefiles" ../;
make;)

mkdir lib

cp cmake/libEffekseer.a lib/libEffekseer.a
cp cmake/libEffekseerRendererGL.a lib/libEffekseerRendererGL.a
cp cmake/libEffekseerSoundAL.a lib/libEffekseerSoundAL.a

(cd Test;
cmake -G "Unix Makefiles" ./;
make clean;
make;)


