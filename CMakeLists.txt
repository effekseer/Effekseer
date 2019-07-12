cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

# solution name
project(Effekseer)

# For MSVC
if (MSVC)
	option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

# zlib
ExternalProject_Add(
    ExternalProject_zlib
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/zlib
    PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/zlib
    INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib
                    -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                    -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                    -DASM686=off
                    -DAMD64=off
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/include)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/lib)

# libpng
ExternalProject_Add(
    ExternalProject_libpng
    DEPENDS         ExternalProject_zlib
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/libpng
    PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/libpng
    INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng
                    -DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/Dev/CFlagOverrides.cmake
                    -DEFK_USE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                    -DZLIB_INCLUDE_DIR=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/include
                    -DPNG_BUILD_ZLIB=ON
                    -DPNG_SHARED=OFF
                    -DPNG_TESTS=OFF
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/include)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/lib)

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
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd/include)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libgd/lib)

# glew
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
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew/include)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glew/lib)

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
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw/include)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/glfw/lib)

# OpenSoundMixer
ExternalProject_Add(
    ExternalProject_OpenSoundMixer
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer
    PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/OpenSoundMixer
    INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
                    -DCMAKE_DEBUG_POSTFIX=d
                    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
)
list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer/src)
list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer/lib)

add_subdirectory(Dev)
