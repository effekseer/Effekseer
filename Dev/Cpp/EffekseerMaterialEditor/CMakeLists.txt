# The project's name
project(EffekseerMaterialEditor)

# Check cmake version
cmake_minimum_required(VERSION 3.0)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
endif()

# For Mac
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

# C++ version
if(WIN32)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

file(GLOB srcs 
    *.cpp
    Graphics/*.cpp)

file(GLOB hs 
    *.h
    Dialog/*.h
    Graphics/*.h)

file(GLOB srcs_third_imgui
    ThirdParty/imgui_main/*.cpp)

file(GLOB srcs_third_imgui_platform
    ThirdParty/imgui_platform/imgui_impl_opengl3.cpp
    ThirdParty/imgui_platform/imgui_impl_glfw.cpp)

file(GLOB srcs_third_node
    ThirdParty/NodeEditor/Source/*.cpp)

if(WIN32)
    file(GLOB srcs_third_nfd
        ThirdParty/nfd/nfd_common.c
        ThirdParty/nfd/nfd_win.cpp)
elseif(APPLE)
    file(GLOB srcs_third_nfd
        ThirdParty/nfd/nfd_common.c
        ThirdParty/nfd/nfd_cocoa.m)
else()
    file(GLOB srcs_third_nfd
        ThirdParty/nfd/nfd_common.c
        ThirdParty/nfd/nfd_gtk.c)
endif()

link_directories(
    ${PROJECT_SOURCE_DIR}/../lib/
    )

# Add compile targets
add_executable(
    EffekseerMaterialEditor
    ${hs} ${srcs} ${srcs_third_imgui} ${srcs_third_imgui_platform} ${srcs_third_node} ${srcs_third_nfd})

# Flags
if (MSVC)
    if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
        foreach (flag CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE)

            if (${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
            if (${flag} MATCHES "/MDd")
                string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
endif()

if(NOT APPLE)
    add_dependencies(EffekseerMaterialEditor ExternalProject_glew ExternalProject_glfw)
endif()

target_include_directories(
    EffekseerMaterialEditor
    PRIVATE
    ${EFK_THIRDPARTY_INCLUDES}
    ../include/
    ${PROJECT_SOURCE_DIR}/../3rdParty/AltseedRHI/include/
    ../EffekseerMaterial/
    ThirdParty/imgui_platform/
    ThirdParty/imgui_main/
    ThirdParty/picojson/
    ThirdParty/stb_image/
    ThirdParty/NodeEditor/Include/)

target_link_directories(
    EffekseerMaterialEditor
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)

target_link_libraries(
    EffekseerMaterialEditor
    PRIVATE
    EffekseerMaterial
    EditorCommon
    AltseedRHI
    debug "${GLFW3_LIB_DEBUG}"
    optimized "${GLFW3_LIB_RELEASE}"
)

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

if (WIN32)

target_link_libraries(
    EffekseerMaterialEditor
    PRIVATE
    Effekseer # to use shader loader
    EffekseerRendererGL # to use shader loader
    IPC
    debug libglew32d
    optimized libglew32)

elseif(APPLE)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# For Mac
find_library(COCOA_FRAMEWORK Cocoa)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

target_link_libraries(
    EffekseerMaterialEditor
    PRIVATE
    Effekseer # to use shader loader
    EffekseerRendererGL # to use shader loader
    IPC
    ${CMAKE_THREAD_LIBS_INIT}
    ${COCOA_FRAMEWORK}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${IOKIT_FRAMEWORK}
    ${CORE_FOUNDATION_FRAMEWORK}
    ${CORE_VIDEO_FRAMEWORK})

else()

# For Linux

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GTK3_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR})

link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

target_link_libraries(
    EffekseerMaterialEditor
    PRIVATE
    Effekseer # to use shader loader
    EffekseerRendererGL # to use shader loader
    IPC
    GLEW
    dl
    stdc++fs
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARY}
    ${GTK3_LIBRARIES}
    ${X11_LIBRARIES})

endif()

set_target_properties(EffekseerMaterialEditor PROPERTIES LINKER_LANGUAGE CXX)

set(files ${hs} ${srcs} ${srcs_third_imgui} ${srcs_third_imgui_platform} ${srcs_third_node} ${srcs_third_nfd})

foreach(FILE ${files}) 
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

set_target_properties( EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties( EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties( EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release )
