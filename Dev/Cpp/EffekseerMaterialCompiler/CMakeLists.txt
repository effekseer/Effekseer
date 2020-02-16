
# OpenGL (OpenGL is a code as binary)

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
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
)

if(MSVC OR APPLE)
    set_target_properties(EffekseerMaterialCompilerGL
        PROPERTIES
        DEBUG_POSTFIX ".Debug")
endif()

# Metal (Metal is a code as binary)

add_library(
    EffekseerMaterialCompilerMetal
    SHARED
    Metal/EffekseerMaterialCompilerMetal.h
    Metal/EffekseerMaterialCompilerMetal.cpp
)

target_link_libraries(
    EffekseerMaterialCompilerMetal
    Effekseer
)

target_compile_definitions(EffekseerMaterialCompilerMetal PRIVATE __SHARED_OBJECT__=1)

set_target_properties( EffekseerMaterialCompilerMetal
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
)

if(MSVC OR APPLE)
    set_target_properties(EffekseerMaterialCompilerMetal
        PROPERTIES
        DEBUG_POSTFIX ".Debug")
endif()

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

    set_target_properties(EffekseerMaterialCompilerDX11
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

    add_library(
        EffekseerMaterialCompilerDX9
        SHARED
        DirectX9/EffekseerMaterialCompilerDX9.h
        DirectX9/EffekseerMaterialCompilerDX9.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerDX9
        Effekseer
    )

    target_compile_definitions(EffekseerMaterialCompilerDX9 PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerDX9
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    )

    set_target_properties(EffekseerMaterialCompilerDX9
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

endif()

if(WIN32 AND BUILD_DX12)
    add_library(
        EffekseerMaterialCompilerDX12
        SHARED
        DirectX12/EffekseerMaterialCompilerDX12.h
        DirectX12/EffekseerMaterialCompilerDX12.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerDX12
        Effekseer
        LLGI
    )

    target_compile_definitions(EffekseerMaterialCompilerDX12 PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerDX12
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    )

    set_target_properties(EffekseerMaterialCompilerDX12
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

endif()

