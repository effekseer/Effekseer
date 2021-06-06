cmake_minimum_required (VERSION 3.0.0)

project(Example_DirectX9)

add_executable(Example_DirectX9 main.cpp)

target_include_directories(
    Example_DirectX9
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererDX9
    ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2
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