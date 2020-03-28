cmake_minimum_required(VERSION 3.12)
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
option(BUILD_VERSION16 "is built as version1.6" OFF)
option(BUILD_UNITYPLUGIN "is built as unity plugin" OFF)
option(FROM_CI "from ci" OFF)
option(SANITIZE_ENABLED "is sanitizer enabled" OFF)
option(USE_LIBPNG_LOADER "use libpng in an internal loader (if it builds viewer except windows, it becomes on)" OFF)

option(BUILD_VULKAN "Build vulkan modules" OFF)

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

if (WIN32 AND NOT FROM_CI)
    set(BUILD_DX12 ON)
else()
    set(BUILD_DX12 OFF)
endif()

if (APPLE)
    set(BUILD_METAL ON)
else()
    set(BUILD_METAL OFF)
endif()

if (BUILD_DX12)
    add_definitions(-D__EFFEKSEER_BUILD_DX12__)
endif()

if (BUILD_VERSION16)
    add_definitions(-D__EFFEKSEER_BUILD_VERSION16__)
endif()

# for linux
if(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# detect IDE
if(CMAKE_GENERATOR MATCHES "Visual Studio|Xcode")
    message("Build with postfix")
    set(BUILD_WITH_POSTFIX ON)
else()
    set(BUILD_WITH_POSTFIX OFF)
endif()

if(BUILD_VIEWER)

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
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/zlib/lib)
    
    if(MSVC)
        set(ZLIB_LIB_DEBUG "zlibstaticd")
        set(ZLIB_LIB_RELEASE "zlibstatic")
    else()
        set(ZLIB_LIB_DEBUG "z")
        set(ZLIB_LIB_RELEASE "z")
    endif()

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
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/include)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/libpng/lib)
    
    if(MSVC)
        set(LIBPNG_LIB_DEBUG "libpng16_staticd")
        set(LIBPNG_LIB_RELEASE "libpng16_static")
    elseif(BUILD_WITH_POSTFIX)
        set(LIBPNG_LIB_DEBUG "png16")
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
    if(NOT APPLE)
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
endif()
    
if(BUILD_VIEWER OR BUILD_EXAMPLES)
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
    
    # OpenSoundMixer
    ExternalProject_Add(
        ExternalProject_OpenSoundMixer
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer
        PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Build/OpenSoundMixer
        INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer
                        -DCMAKE_DEBUG_POSTFIX=d
                        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL}
                        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    )
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/OpenSoundMixer/src)
    list(APPEND EFK_THIRDPARTY_LIBRARY_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/Install/OpenSoundMixer/lib)
endif()


# a function to generate filters by folders
function(FilterFolder srcs)
    foreach(FILE ${srcs}) 
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    
        # Remove common directory prefix to make the group
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    
        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")
    
        # Group into "Source Files" and "Header Files"
        if ("${FILE}" MATCHES ".*\\.cpp")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.c")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.cxx")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.h")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.mm")
           set(GROUP "${GROUP}")
        endif()
    
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction(FilterFolder)

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

add_subdirectory(Dev)

if(BUILD_EXAMPLES)
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
