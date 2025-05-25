#!/bin/bash
set -e
set +x

if [ -z "$RDIR_R" ]; then
    export RDIR_R="$PWD/EffekseerForCpp"
fi

if ! command -v robocopy >/dev/null 2>&1; then
    robocopy() {
        local from=$1
        local to=$2
        shift 2
        local include=()
        for i in "$@"; do
            [ "$i" = "/S" ] && continue
            include+=(--include="$i")
        done
        echo "Copy from $from to $to"
        rsync -av "${include[@]}" --exclude="*" "$from/" "$to"
    }
fi



rm -R $RDIR_R || true
mkdir -p  $RDIR_R

mkdir -p  $RDIR_R/cmake

echo Copy runtime
mkdir -p  "$RDIR_R/Examples"

mkdir -p "$RDIR_R/src"
mkdir -p "$RDIR_R/src/include"
mkdir -p "$RDIR_R/src/lib"

header_files=(
  Dev/Cpp/Effekseer/Effekseer.h
  Dev/Cpp/EffekseerRendererDX9/EffekseerRendererDX9.h
  Dev/Cpp/EffekseerRendererDX11/EffekseerRendererDX11.h
  Dev/Cpp/EffekseerRendererGL/EffekseerRendererGL.h
  Dev/Cpp/EffekseerSoundXAudio2/EffekseerSoundXAudio2.h
  Dev/Cpp/EffekseerSoundAL/EffekseerSoundAL.h
)

for f in "${header_files[@]}"; do
    cp "$f" "$RDIR_R/src/include/"
done


copy_runtime() {
    local dir=$1
    shift
    mkdir -p "$RDIR_R/src/$dir"
    robocopy "Dev/Cpp/$dir" "$RDIR_R/src/$dir" "$@" /S
}

copy_runtime Effekseer *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerRendererDX9 *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerRendererDX11 *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerRendererDX12 *.h *.cpp *.fx *.bat CMakeLists.txt
copy_runtime EffekseerRendererGL *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerRendererLLGI *.h *.cpp CMakeLists.txt
copy_runtime EffekseerRendererCommon *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerRendererMetal *.h *.cpp *.mm CMakeLists.txt
copy_runtime EffekseerRendererVulkan *.h *.cpp *.vert *.frag *.inl *.py CMakeLists.txt
copy_runtime EffekseerSoundXAudio2 *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerSoundAL *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerSoundDSound *.h *.cpp *.fx CMakeLists.txt
copy_runtime EffekseerMaterialCompiler *.h *.cpp CMakeLists.txt

mkdir -p "$RDIR_R/src/3rdParty/LLGI"
robocopy Dev/Cpp/3rdParty/LLGI "$RDIR_R/src/3rdParty/LLGI" /S

mkdir -p "$RDIR_R/src/3rdParty/glslang"
robocopy Dev/Cpp/3rdParty/glslang "$RDIR_R/src/3rdParty/glslang" /S

mkdir -p "$RDIR_R/src/3rdParty/stb_effekseer"
robocopy Dev/Cpp/3rdParty/stb_effekseer "$RDIR_R/src/3rdParty/stb_effekseer" *.h

echo Samples for runtimes
robocopy Examples $RDIR_R/Examples/ *.h *.cpp *.mm *.txt *.fx *.efk *.wav *.png /S

cp Dev/Cpp/CMakeLists.txt $RDIR_R/src/.

cp cmake/FilterFolder.cmake $RDIR_R/cmake/.

echo Sample
# Release/Sample is omitted per user request

echo License
cp LICENSE $RDIR_R/LICENSE.txt
cp LICENSE_RUNTIME_DIRECTX $RDIR_R/LICENSE_RUNTIME_DIRECTX.txt
cp LICENSE_RUNTIME_VULKAN $RDIR_R/LICENSE_RUNTIME_VULKAN.txt

echo Readme
cp docs/readme_cpp_ja.txt $RDIR_R/readme_cpp_ja.txt
cp docs/readme_cpp_en.txt $RDIR_R/readme_cpp_en.txt
cp docs/releasenotes_cpp.txt $RDIR_R/releasenotes.txt

cp Release/CMakeLists.txt $RDIR_R/.
robocopy Downloads/glfw $RDIR_R/Examples/Utils/glfw/. /S
cp Release/build_msvc.bat $RDIR_R/build_msvc.bat

cp Release/build_macOS.sh $RDIR_R/build_macOS.sh

