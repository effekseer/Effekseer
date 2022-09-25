# OpenGL (OpenGL is a code as binary)

add_library(
    EffekseerMaterialCompilerCommon
    STATIC
    Common/ShaderGeneratorCommon.h
    Common/ShaderGeneratorCommon.cpp
)

target_link_libraries(
    EffekseerMaterialCompilerCommon
    PRIVATE 
    Effekseer
)

set_property(TARGET EffekseerMaterialCompilerCommon PROPERTY FOLDER Libraries) 

add_library(
    EffekseerMaterialCompilerGLSLGenerator
    STATIC
    GLSLGenerator/ShaderGenerator.h
    GLSLGenerator/ShaderGenerator.cpp
)
set_property(TARGET EffekseerMaterialCompilerGLSLGenerator PROPERTY FOLDER Libraries) 

target_link_libraries(
    EffekseerMaterialCompilerGLSLGenerator
    PUBLIC
    EffekseerMaterialCompilerCommon
)

add_library(
    EffekseerMaterialCompilerGL
    SHARED
    OpenGL/EffekseerMaterialCompilerGL.h
    OpenGL/EffekseerMaterialCompilerGL.cpp
)

target_link_libraries(
    EffekseerMaterialCompilerGL
    PRIVATE 
    Effekseer
    EffekseerMaterialCompilerGLSLGenerator
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

if(CLANG_FORMAT_ENABLED)
    clang_format(EffekseerMaterialCompilerGL)
endif()

set_property(TARGET EffekseerMaterialCompilerGL PROPERTY FOLDER Libraries) 

# Metal (Metal is a code as binary)

add_library(
    EffekseerMaterialCompilerMetal
    SHARED
    Metal/EffekseerMaterialCompilerMetal.h
    Metal/EffekseerMaterialCompilerMetal.cpp
)

target_link_libraries(
    EffekseerMaterialCompilerMetal
    PRIVATE 
    Effekseer
    EffekseerMaterialCompilerCommon
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

if(CLANG_FORMAT_ENABLED)
    clang_format(EffekseerMaterialCompilerMetal)
endif()

set_property(TARGET EffekseerMaterialCompilerMetal PROPERTY FOLDER Libraries) 

if(WIN32)
    add_library(
        EffekseerMaterialCompilerHLSLGenerator
        STATIC
        HLSLGenerator/ShaderGenerator.h
        HLSLGenerator/ShaderGenerator.cpp
    )
    set_property(TARGET EffekseerMaterialCompilerHLSLGenerator PROPERTY FOLDER Libraries) 

    target_link_libraries(
        EffekseerMaterialCompilerHLSLGenerator
        PUBLIC
        EffekseerMaterialCompilerCommon
    )
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
        PRIVATE 
        Effekseer
        EffekseerMaterialCompilerHLSLGenerator
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
    set_property(TARGET EffekseerMaterialCompilerDX11 PROPERTY FOLDER Libraries) 

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerDX11)
    endif()

    add_library(
        EffekseerMaterialCompilerDX9
        SHARED
        DirectX9/EffekseerMaterialCompilerDX9.h
        DirectX9/EffekseerMaterialCompilerDX9.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerDX9
        PRIVATE 
        Effekseer
        EffekseerMaterialCompilerHLSLGenerator
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
    set_property(TARGET EffekseerMaterialCompilerDX9 PROPERTY FOLDER Libraries)

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerDX9)
    endif()

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
        PRIVATE 
        Effekseer
        EffekseerMaterialCompilerHLSLGenerator
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

    set_property(TARGET EffekseerMaterialCompilerDX12 PROPERTY FOLDER Libraries)

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerDX12)
    endif()
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/Dev/Cpp/EffekseerMaterialCompiler/Switch")
    add_library(
        EffekseerMaterialCompilerSwitch
        SHARED
        Switch/EffekseerMaterialCompilerSwitch.h
        Switch/EffekseerMaterialCompilerSwitch.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerSwitch
        PRIVATE
        EffekseerMaterialCompilerGLSLGenerator
    )


    target_compile_definitions(EffekseerMaterialCompilerSwitch PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerSwitch
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    )

    set_target_properties(EffekseerMaterialCompilerSwitch
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

    set_property(TARGET EffekseerMaterialCompilerSwitch PROPERTY FOLDER Libraries)

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerSwitch)
    endif()
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/Dev/Cpp/EffekseerMaterialCompiler/PS4/")
    add_library(
        EffekseerMaterialCompilerPS4
        SHARED
        PS4/EffekseerMaterialCompilerPS4.h
        PS4/EffekseerMaterialCompilerPS4.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerPS4
        PRIVATE
        EffekseerMaterialCompilerHLSLGenerator
    )

    target_compile_definitions(EffekseerMaterialCompilerPS4 PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerPS4
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    )

    set_target_properties(EffekseerMaterialCompilerPS4
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

    set_property(TARGET EffekseerMaterialCompilerPS4 PROPERTY FOLDER Libraries)

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerPS4)
    endif()
endif()


if(EXISTS "${CMAKE_SOURCE_DIR}/Dev/Cpp/EffekseerMaterialCompiler/PS5/")
    add_library(
        EffekseerMaterialCompilerPS5
        SHARED
        PS5/EffekseerMaterialCompilerPS5.h
        PS5/EffekseerMaterialCompilerPS5.cpp
    )

    target_link_libraries(
        EffekseerMaterialCompilerPS5
        PRIVATE
        EffekseerMaterialCompilerHLSLGenerator
    )

    target_compile_definitions(EffekseerMaterialCompilerPS5 PRIVATE __SHARED_OBJECT__=1)

    set_target_properties(
        EffekseerMaterialCompilerPS5
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Dev/release/tools/"
    )

    set_target_properties(EffekseerMaterialCompilerPS5
        PROPERTIES
        DEBUG_POSTFIX ".Debug")

    set_property(TARGET EffekseerMaterialCompilerPS5 PROPERTY FOLDER Libraries)

    if(CLANG_FORMAT_ENABLED)
        clang_format(EffekseerMaterialCompilerPS5)
    endif()
endif()

if(APPLE)
    set_target_properties(EffekseerMaterialCompilerMetal PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
    set_target_properties(EffekseerMaterialCompilerGL PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()