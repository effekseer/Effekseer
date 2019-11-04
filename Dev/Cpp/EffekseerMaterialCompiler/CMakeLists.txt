
add_library(
    EffekseerMaterialCompilerGL
    SHARED
    OpenGL/EffekseerMaterialCompilerGL.h
    OpenGL/EffekseerMaterialCompilerGL.cpp
)

target_link_libraries(
    EffekseerMaterialCompilerGL
    Effekseer
)

target_compile_definitions(EffekseerMaterialCompilerGL PRIVATE __SHARED_OBJECT__=1)

set_target_properties( EffekseerMaterialCompilerGL
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
)

if(WIN32)
    add_library(
        EffekseerMaterialCompilerDX11
        SHARED
        DirectX11/EffekseerMaterialCompilerDX11.h
        DirectX11/EffekseerMaterialCompilerDX11.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerDX11
        Effekseer
    )

    target_compile_definitions(EffekseerMaterialCompilerDX11 PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerDX11
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
)

endif()
