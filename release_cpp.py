#!/usr/bin/env python3
import shutil
import os
from pathlib import Path


def copy_runtime(src_root: Path, dst_root: Path, patterns):
    dst_root.mkdir(parents=True, exist_ok=True)
    for pattern in patterns:
        for f in src_root.rglob(pattern):
            if f.is_file():
                rel = f.relative_to(src_root)
                dest = dst_root / rel
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(f, dest)


def main():
    rdir = Path(os.environ.get("RDIR_R", "EffekseerForCpp"))
    if rdir.exists():
        shutil.rmtree(rdir)
    (rdir / "cmake").mkdir(parents=True)
    (rdir / "Examples").mkdir(parents=True)
    (rdir / "src" / "include").mkdir(parents=True)
    (rdir / "src" / "lib").mkdir(parents=True)

    headers = [
        "Dev/Cpp/Effekseer/Effekseer.h",
        "Dev/Cpp/EffekseerRendererDX9/EffekseerRendererDX9.h",
        "Dev/Cpp/EffekseerRendererDX11/EffekseerRendererDX11.h",
        "Dev/Cpp/EffekseerRendererGL/EffekseerRendererGL.h",
        "Dev/Cpp/EffekseerSoundXAudio2/EffekseerSoundXAudio2.h",
        "Dev/Cpp/EffekseerSoundAL/EffekseerSoundAL.h",
    ]
    for f in headers:
        shutil.copy(f, rdir / "src" / "include")

    runtimes = {
        "Effekseer": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerRendererDX9": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerRendererDX11": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerRendererDX12": ["*.h", "*.cpp", "*.fx", "*.bat", "CMakeLists.txt"],
        "EffekseerRendererGL": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerRendererLLGI": ["*.h", "*.cpp", "CMakeLists.txt"],
        "EffekseerRendererCommon": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerRendererMetal": ["*.h", "*.cpp", "*.mm", "CMakeLists.txt"],
        "EffekseerRendererVulkan": ["*.h", "*.cpp", "*.vert", "*.frag", "*.inl", "*.py", "CMakeLists.txt"],
        "EffekseerSoundXAudio2": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerSoundAL": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerSoundDSound": ["*.h", "*.cpp", "*.fx", "CMakeLists.txt"],
        "EffekseerMaterialCompiler": ["*.h", "*.cpp", "CMakeLists.txt"],
    }
    for name, patterns in runtimes.items():
        copy_runtime(Path("Dev/Cpp")/name, rdir/"src"/name, patterns)

    # 3rd party libraries
    shutil.copytree("Dev/Cpp/3rdParty/LLGI", rdir/"src"/"3rdParty"/"LLGI", dirs_exist_ok=True)
    shutil.copytree("Dev/Cpp/3rdParty/glslang", rdir/"src"/"3rdParty"/"glslang", dirs_exist_ok=True)
    shutil.rmtree(rdir/"src"/"3rdParty"/"glslang"/"Test", ignore_errors=True)
    shutil.copytree("Dev/Cpp/3rdParty/stb_effekseer", rdir/"src"/"3rdParty"/"stb_effekseer", dirs_exist_ok=True)

    # samples
    patterns = ["*.h", "*.cpp", "*.mm", "*.txt", "*.fx", "*.efk", "*.efkefc", "*.wav", "*.png"]
    for pattern in patterns:
        for f in Path("Examples").rglob(pattern):
            if f.is_file():
                dest = rdir/"Examples"/f.relative_to("Examples")
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(f, dest)

    shutil.copy("Dev/Cpp/CMakeLists.txt", rdir/"src")
    shutil.copy("cmake/FilterFolder.cmake", rdir/"cmake")
    shutil.copy("cmake/LinkAppleLibs.cmake", rdir/"cmake")

    shutil.copy("LICENSE", rdir/"LICENSE.txt")
    shutil.copy("LICENSE_RUNTIME_DIRECTX", rdir/"LICENSE_RUNTIME_DIRECTX.txt")
    shutil.copy("LICENSE_RUNTIME_VULKAN", rdir/"LICENSE_RUNTIME_VULKAN.txt")

    shutil.copy("docs/Help_Cpp_Ja.html", rdir/"Help_Cpp_Ja.html")
    shutil.copy("docs/Help_Cpp_En.html", rdir/"Help_Cpp_En.html")

    shutil.copy("Release/CMakeLists.txt", rdir)
    shutil.copytree("Downloads/glfw", rdir/"Examples"/"Utils"/"glfw", dirs_exist_ok=True)
    shutil.copy("Release/build_msvc.bat", rdir/"build_msvc.bat")
    shutil.copy("Release/build_macOS.sh", rdir/"build_macOS.sh")

if __name__ == "__main__":
    main()
