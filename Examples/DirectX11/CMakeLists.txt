cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_DirectX11 main.cpp)

target_include_directories(
    Example_DirectX11
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererDX11
    ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2
    ../Utils
)

target_link_libraries(
    Example_DirectX11
    PRIVATE
    Effekseer
    EffekseerRendererDX11
    EffekseerSoundXAudio2
    Example_Utils
)