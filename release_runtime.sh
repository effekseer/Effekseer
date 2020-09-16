#!/bin/bash
set -e
set +x
if [ "$RDIR_R" = "" ];
then
    export RDIR_R="$PWD/EffekseerRuntime"
fi

if [ "`which robocopy`" = "" ];
then
    function robocopy { 
        from=$1
        to=$2 
        include=""
        echo "Copy from $from to $to"
        args=(${@:3})
        unset "args[${#args[@]}-1]" 

        for i in "${args[@]}"; 
        do 
            include="$include --include=\"$i\""
            echo "Include $i"
            
        done
        cdir=$PWD
        cd $from
        
        rsync -av --exclude=\"*\" $include . $to

        cd $cdir
    }

fi



rm -R $RDIR_R || true
mkdir -p  $RDIR_R

mkdir -p  $RDIR_R/cmake

echo Copy runtime
mkdir -p  $RDIR_R/Examples

mkdir -p  $RDIR_R/src
mkdir -p  $RDIR_R/src/include
mkdir -p  $RDIR_R/src/lib


cp Dev/Cpp/Effekseer/Effekseer.h $RDIR_R\src\include\.
cp Dev/Cpp/EffekseerRendererDX9/EffekseerRendererDX9.h $RDIR_R/src/include/.
cp Dev/Cpp/EffekseerRendererDX11/EffekseerRendererDX11.h $RDIR_R/src/include/.
cp Dev/Cpp/EffekseerRendererGL/EffekseerRendererGL.h $RDIR_R/src/include/.
cp Dev/Cpp/EffekseerSoundXAudio2/EffekseerSoundXAudio2.h $RDIR_R/src/include/.
cp Dev/Cpp/EffekseerSoundAL/EffekseerSoundAL.h $RDIR_R/src/include/.


mkdir -p  $RDIR_R/src/Effekseer
robocopy Dev/Cpp/Effekseer $RDIR_R/src/Effekseer *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererDX9
robocopy Dev/Cpp/EffekseerRendererDX9 $RDIR_R/src/EffekseerRendererDX9 *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererDX11
robocopy Dev/Cpp/EffekseerRendererDX11 $RDIR_R/src/EffekseerRendererDX11 *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererDX12
robocopy Dev/Cpp/EffekseerRendererDX12 $RDIR_R/src/EffekseerRendererDX12 *.h *.cpp *.fx *.bat CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererGL
robocopy Dev/Cpp/EffekseerRendererGL $RDIR_R/src/EffekseerRendererGL *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererLLGI
robocopy Dev/Cpp/EffekseerRendererLLGI $RDIR_R/src/EffekseerRendererLLGI *.h *.cpp /S

mkdir -p  $RDIR_R/src/EffekseerRendererCommon
robocopy Dev/Cpp/EffekseerRendererCommon $RDIR_R/src/EffekseerRendererCommon *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererMetal
robocopy Dev/Cpp/EffekseerRendererMetal $RDIR_R/src/EffekseerRendererMetal *.h *.cpp *.mm CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerRendererVulkan
robocopy Dev/Cpp/EffekseerRendererVulkan $RDIR_R/src/EffekseerRendererVulkan *.h *.cpp *.vert *.frag *.inl *.py CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerSoundXAudio2
robocopy Dev/Cpp/EffekseerSoundXAudio2 $RDIR_R/src/EffekseerSoundXAudio2 *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerSoundAL
robocopy Dev/Cpp/EffekseerSoundAL $RDIR_R/src/EffekseerSoundAL *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerSoundDSound
robocopy Dev/Cpp/EffekseerSoundDSound $RDIR_R/src/EffekseerSoundDSound *.h *.cpp *.fx CMakeLists.txt /S

mkdir -p  $RDIR_R/src/EffekseerMaterialCompiler
robocopy Dev/Cpp/EffekseerMaterialCompiler $RDIR_R/src/EffekseerMaterialCompiler *.h *.cpp CMakeLists.txt /S

mkdir -p  $RDIR_R/src/3rdParty/LLGI
robocopy Dev/Cpp/3rdParty/LLGI $RDIR_R/src/3rdParty/LLGI /S

mkdir -p  $RDIR_R/src/3rdParty/glslang
robocopy Dev/Cpp/3rdParty/glslang $RDIR_R/src/3rdParty/glslang /S

mkdir -p  $RDIR_R/src/3rdParty/stb_effekseer
robocopy Dev/Cpp/3rdParty/stb_effekseer $RDIR_R/src/3rdParty/stb_effekseer *.h

echo Samples for runtimes
robocopy Examples $RDIR_R/Examples/ *.h *.cpp *.mm *.txt *.fx *.efk *.wav *.png /S

cp Dev/Cpp/CMakeLists.txt $RDIR_R/src/.

cp cmake/FilterFolder.cmake $RDIR_R/cmake/.

echo Sample

mkdir -p  $RDIR_R/Sample
robocopy Release/Sample $RDIR_R/Sample *.efkproj *.efkmodel *.txt *.png *.mqo *.fbx /S

echo License
cp LICENSE $RDIR_R/LICENSE.txt
cp LICENSE_RUNTIME_DIRECTX $RDIR_R/LICENSE_RUNTIME_DIRECTX.txt
cp LICENSE_RUNTIME_VULKAN $RDIR_R/LICENSE_RUNTIME_VULKAN.txt

echo Readme
cp docs/readme_runtime_ja.txt $RDIR_R/readme_runtime_ja.txt
cp docs/readme_runtime_en.txt $RDIR_R/readme_runtime_en.txt
cp docs/releasenotes_runtime.txt $RDIR_R/releasenotes.txt

cp Release/CMakeLists.txt $RDIR_R/.
robocopy Downloads/glfw $RDIR_R/Examples/Utils/glfw/. /S
cp Release/build_msvc2017.bat $RDIR_R/build_msvc2017.bat
cp Release/build_msvc2019.bat $RDIR_R/build_msvc2019.bat
cp Release/build_vulkan_msvc2019.bat $RDIR_R/build_vulkan_msvc2019.bat

cp Release/build_macOS.sh $RDIR_R/build_macOS.sh

