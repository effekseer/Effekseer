cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_DirectX9 main.cpp)

target_include_directories(
    Example_DirectX9
    PRIVATE
     ../../Dev/Cpp/Effekseer
     ../../Dev/Cpp/EffekseerRendererDX9
     ../../Dev/Cpp/EffekseerSoundXAudio2
     ../Utils
)

target_link_libraries(
    Example_DirectX9
    PRIVATE
    Effekseer
    EffekseerRendererDX9
    EffekseerSoundXAudio2
    Example_Utils
)