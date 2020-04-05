xbuild Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86
xbuild Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86

rm -rf Temp

mkdir Temp

(cd Temp;
cmake -G "Unix Makefiles" -D BUILD_VIEWER=ON -D CMAKE_BUILD_TYPE=Release ../Cpp/;
make;)

cp Temp/Viewer/libViewer.so release/

