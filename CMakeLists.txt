cmake_minimum_required(VERSION 3.15)
include(ExternalProject)

# Solution name
project(Effekseer)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

option(BUILD_VIEWER "Build viewer" OFF)
option(BUILD_EDITOR "Build editor" OFF)
option(BUILD_TEST "Build test" OFF)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_VERSION17 "is built as version1.7" OFF)
option(BUILD_UNITYPLUGIN "is built as unity plugin" OFF)
option(BUILD_UNITYPLUGIN_FOR_IOS "is built as unity plugin for ios" OFF)
option(FROM_CI "from ci" OFF)
option(SANITIZE_ENABLED "is sanitizer enabled" OFF)
option(USE_LIBPNG_LOADER "use libpng in an internal loader (if it builds viewer except windows, it becomes on)" OFF)

option(BUILD_VULKAN "Build vulkan modules" OFF)

if (WIN32)
    option(BUILD_DX12 "Build DirectX12 modules" ON)
else()
    option(BUILD_DX12 "Build DirectX12 modules" OFF)
endif()


if (APPLE)
    option(BUILD_METAL "Build metal modules" ON)
else()
    option(BUILD_METAL "Build metal modules" OFF)
endif()

include(cmake/ClangFormat.cmake)

if (BUILD_VIEWER)
    set(USE_LIBPNG_LOADER ON)
endif()

if(USE_LIBPNG_LOADER)
    add_definitions(-D__EFFEKSEER_USE_LIBPNG__)
endif()

if(SANITIZE_ENABLED)
    set(CMAKE_CXX_FLAGS " -fsanitize=undefined,address" ${CMAKE_CXX_FLAGS})
endif()

# find_package(Vulkan)
# if (Vulkan_FOUND)
#     set(BUILD_VULKAN ON)
# else()
#     set(BUILD_VULKAN OFF)
# endif()

if(BUILD_VULKAN)
    set(BUILD_VULKAN ON CACHE BOOL "For LLGI" FORCE)
    set(BUILD_VULKAN_COMPILER ON CACHE BOOL "For LLGI" FORCE)
    add_definitions(-D__EFFEKSEER_BUILD_VULKAN__)
else()
    set(BUILD_VULKAN OFF CACHE BOOL "For LLGI" FORCE)
    set(BUILD_VULKAN_COMPILER OFF CACHE BOOL "For LLGI" FORCE)
endif()

if (BUILD_DX12)
    add_definitions(-D__EFFEKSEER_BUILD_DX12__)
endif()

if (BUILD_VERSION17)
    add_definitions(-D__EFFEKSEER_BUILD_VERSION17__)
endif()

option(BUILD_WITH_EASY_PROFILER "Build with easy_profiler to profile" OFF)

if ()
    add_definitions(-D__EFFEKSEER_BUILD_DX12__)
endif()

# for linux
if(UNIX AND NOT APPLE)
    set(OpenGL_GL_PREFERENCE GLVND)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# detect IDE
if(CMAKE_GENERATOR MATCHES "Visual Studio|Xcode")
    message("Build with postfix")
    set(BUILD_WITH_POSTFIX ON)
else()
    set(BUILD_WITH_POSTFIX OFF)
endif()

if(MSVC)
    add_definitions(/MP)
endif()

if(SANITIZER_ENABLED AND MSVC)
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
endif()

if(BUILD_WITH_EASY_PROFILER)
    ExternalProject_Add(
        ExternalProject_easy_profiler
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Dev/Cpp/3rdParty/easy_profiler
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/easy_profiler
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/easy_profiler
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/easy_profiler
                        -DCMAKE_DEBUG_POSTFIX=d
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DBUILD_SHARED_LIBS=OFF
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )

    if(BUILD_WITH_POSTFIX)
        set(EASY_PROFILER_LIB_DEBUG "easy_profilerd")
        set(EASY_PROFILER_LIB_RELEASE "easy_profiler")
    else()
        set(EASY_PROFILER_LIB_DEBUG "easy_profiler")
        set(EASY_PROFILER_LIB_RELEASE "easy_profiler")
    endif()
    
    list(APPEND EFK_PROFILER_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/easy_profiler/include)
    list(APPEND EFK_PROFILER_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/easy_profiler/lib)

endif()

if(BUILD_VULKAN)
    # TODO : Improve it

    option(USE_EXTERNAL_GLSLANG "Whether do it compile with external glslang" OFF)

    if(USE_EXTERNAL_GLSLANG)
        option(GLSLANG_INCLUDE_DIR "A directory which contains glslang includes" "")
    else()

        # Hack for GLSLANG
        if (MSVC)
            # CMAKE_MSVC_RUNTIME_LIBRARY can be only used if minimum required is larger than 3.17
            if(USE_MSVC_RUNTIME_LIBRARY_DLL)
                cmake_policy(SET CMP0091 NEW)
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
            else()
                cmake_policy(SET CMP0091 NEW)
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        endif()

        set(ENABLE_HLSL OFF CACHE BOOL "For glslang" FORCE)
        set(ENABLE_CTEST OFF CACHE BOOL "For glslang" FORCE)
        set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "For glslang" FORCE)

        set(GLSLANG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dev/Cpp/3rdParty/glslang/" "${CMAKE_CURRENT_SOURCE_DIR}/Dev/Cpp/3rdParty/")
        add_subdirectory(Dev/Cpp/3rdParty/glslang/)

    endif()
endif()

if(BUILD_VIEWER)

    # zlib
    if(BUILD_WITH_POSTFIX)
        set(ZLIB_INSTALL "")
    else()
        set(ZLIB_INSTALL ${CMAKE_COMMAND} --install . --prefix <INSTALL_DIR>)
    endif()

    ExternalProject_Add(
        ExternalProject_zlib
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/zlib
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/zlib
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib
        INSTALL_COMMAND ${ZLIB_INSTALL}
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DASM686=off
                        -DAMD64=off
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/lib)
    
    if(MSVC)
        set(ZLIB_LIB_DEBUG "zlibstaticd")
        set(ZLIB_LIB_RELEASE "zlibstatic")
    elseif(BUILD_WITH_POSTFIX)
        set(ZLIB_LIB_DEBUG "z")
        set(ZLIB_LIB_RELEASE "z")
    else()
        set(ZLIB_LIB_DEBUG "z")
        set(ZLIB_LIB_RELEASE "z")
    endif()

    # libpng
    if(BUILD_WITH_POSTFIX)
        set(LIBPNG_INSTALL "")
    else()
        set(LIBPNG_INSTALL ${CMAKE_COMMAND} --install . --prefix <INSTALL_DIR>)
    endif()

    ExternalProject_Add(
        ExternalProject_libpng
        DEPENDS         ExternalProject_zlib
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/libpng
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/libpng
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng
        INSTALL_COMMAND ${LIBPNG_INSTALL}
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DZLIB_INCLUDE_DIR=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/include
                        -DPNG_BUILD_ZLIB=ON
                        -DPNG_SHARED=OFF
                        -DPNG_TESTS=OFF
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/lib)
    
    if(MSVC)
        set(LIBPNG_LIB_DEBUG "libpng16_staticd")
        set(LIBPNG_LIB_RELEASE "libpng16_static")
    elseif(BUILD_WITH_POSTFIX)
        set(LIBPNG_LIB_DEBUG "png16d")
        set(LIBPNG_LIB_RELEASE "png16")
    else()
        set(LIBPNG_LIB_DEBUG "png16")
        set(LIBPNG_LIB_RELEASE "png16")
    endif()

    # libgd
    ExternalProject_Add(
        ExternalProject_libgd
        DEPENDS         ExternalProject_zlib
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/libgd
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/libgd
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd
        PATCH_COMMAND   ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Dev/3rdPartyPatch/libgd-getopt.c <SOURCE_DIR>/src/getopt.c
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd
                        -DCMAKE_DEBUG_POSTFIX=d
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DZLIB_ROOT=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib
                        -DBUILD_STATIC_LIBS=ON
                        -DBUILD_SHARED_LIBS=OFF
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )

    if(MSVC)
        set(GD_LIB_DEBUG "libgd_staticd")
        set(GD_LIB_RELEASE "libgd_static")
    elseif(BUILD_WITH_POSTFIX)
        set(GD_LIB_DEBUG "gdd")
        set(GD_LIB_RELEASE "gd")
    else()
        set(GD_LIB_DEBUG "gd")
        set(GD_LIB_RELEASE "gd")
    endif()

    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd/lib)
    
    # glew
    if(WIN32)
    ExternalProject_Add(
        ExternalProject_glew
        URL https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip
        DOWNLOAD_DIR    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Download/glew
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/glew
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew
        PATCH_COMMAND   ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Dev/3rdPartyPatch/glew-CMakeLists.txt <SOURCE_DIR>/CMakeLists.txt
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DBUILD_SHARED_LIBS=OFF
                        -DGLEW_BUILD_INFO=OFF
                        -DBUILD_UTILS=OFF
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew/lib)
    endif()

    # OpenSoundMixer
    ExternalProject_Add(
        ExternalProject_OpenSoundMixer
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/OpenSoundMixer
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
                        -DCMAKE_DEBUG_POSTFIX=d
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer/src)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer/lib)
endif()
    
if(BUILD_VIEWER OR BUILD_EXAMPLES OR BUILD_TEST)
    # glfw
    ExternalProject_Add(
        ExternalProject_glfw
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/glfw
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/glfw
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw
                        -DCMAKE_DEBUG_POSTFIX=d
                        -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                        -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DGLFW_BUILD_EXAMPLES=OFF
                        -DGLFW_BUILD_TESTS=OFF
                        -DGLFW_BUILD_DOCS=OFF
                        -DGLFW_INSTALL=ON
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )

    if(BUILD_WITH_POSTFIX)
        set(GLFW3_LIB_DEBUG "glfw3d")
        set(GLFW3_LIB_RELEASE "glfw3")
    else()
        set(GLFW3_LIB_DEBUG "glfw3")
        set(GLFW3_LIB_RELEASE "glfw3")
    endif()

    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw/lib)
endif()

# resource
if (BUILD_VIEWER)
    find_package(PythonInterp 3)

    add_custom_target(ResourceData
        SOURCES ResourceData.dummy
    )

    add_custom_command(
        OUTPUT ResourceData.dummy
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Script/setup.py
    )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FilterFolder)

add_subdirectory(Dev)

if(BUILD_EXAMPLES)
    # To use glfw in OpenGL projects
    add_dependencies(EffekseerRendererGL ExternalProject_glfw)

    set(EFK_RUNTIME_ROOT ${CMAKE_CURRENT_LIST_DIR}/Dev/Cpp)
    add_subdirectory(Examples)
endif()

# specify filters
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (BUILD_VIEWER)
    set_target_properties (ExternalProject_zlib PROPERTIES FOLDER thirdparty)
    set_target_properties (ExternalProject_libpng PROPERTIES FOLDER thirdparty)
    set_target_properties (ExternalProject_libgd PROPERTIES FOLDER thirdparty)
    set_target_properties (ExternalProject_glfw PROPERTIES FOLDER thirdparty)
    set_target_properties (ExternalProject_OpenSoundMixer PROPERTIES FOLDER thirdparty)
endif()
