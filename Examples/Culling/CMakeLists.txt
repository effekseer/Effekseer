cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_Culling main.cpp)
target_include_directories(
    Example_Culling
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererDX9
    ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2
)

target_link_libraries(
    Example_Culling
    PRIVATE
    Effekseer
    EffekseerRendererDX9
    EffekseerSoundXAudio2
)

# for Visual C++
target_compile_definitions(
    Example_Culling
    PRIVATE
    UNICODE
    _UNICODE)

set_property(TARGET Example_Culling PROPERTY FOLDER "Examples") 
