msbuild Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86
msbuild Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86

rm -rf Temp

mkdir Temp

(cd Temp;
cmake -G "Unix Makefiles" -D BUILD_VIEWER=ON -D CMAKE_BUILD_TYPE=Release -D "CMAKE_OSX_ARCHITECTURES=x86_64;i386" ../Cpp/;
make;)

cp Temp/Viewer/libViewer.dylib release/

