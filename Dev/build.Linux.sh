MSBUILD_EXISTS=`which msbuild >/dev/null 2>&1 ; echo $?`
if [ ${MSBUILD_EXISTS} -ne 0 ]
then
    echo "You need to install 'msbuild'"
    exit -1
fi

NUGET_EXISTS=`which nuget >/dev/null 2>&1 ; echo $?`
if [ ${NUGET_EXISTS} -ne 0 ]
then
    echo "You need to install 'nuget' (package manager for C#)"
    exit -1
fi

nuget install ./Editor/Effekseer/packages.config -o ./Editor/packages/
nuget install ./Editor/EffekseerCore/packages.config -o ./Editor/packages/

msbuild Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86
msbuild Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86

rm -rf Temp

mkdir Temp

(cd Temp;
cmake -G "Unix Makefiles" -D BUILD_VIEWER=ON -D CMAKE_BUILD_TYPE=Release ../Cpp/;
make;)

cp Temp/Viewer/libViewer.so release/

