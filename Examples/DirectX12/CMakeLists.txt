cmake_minimum_required (VERSION 3.0.0)

project(Example_DirectX12)

add_executable(Example_DirectX12 main.cpp)

target_include_directories(
    Example_DirectX12
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererDX12
    ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2
    ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src/
    ../Utils
)

target_link_libraries(
    Example_DirectX12
    PRIVATE
    Effekseer
    LLGI
    EffekseerRendererDX12
    EffekseerSoundXAudio2
    Example_Utils
)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Examples") 

if(USE_LIBPNG_LOADER)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
	)
endif()
