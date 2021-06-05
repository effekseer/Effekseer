cmake_minimum_required(VERSION 3.15)

project(Viewer CXX)

if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()


FILE(GLOB hs
	EffekseerTool/*.h
	Graphics/*.h
	GUI/*.h
	*.h
)

FILE(GLOB cpps
	3rdParty/imgui_addon/imguidock/*.cpp
	3rdParty/imgui_addon/imguihelper/*.cpp
	3rdParty/imgui_addon/fcurve/*.cpp
	3rdParty/imgui_addon/ImGuizmo/*.cpp
	EffekseerTool/*.cpp
	Graphics/*.cpp
	GUI/*.cpp
	*.cpp
	*.cxx
)

list(APPEND cpps
    RenderedEffectGenerator.cpp
	Graphics/CommandList.cpp
    Graphics/RenderPass.cpp
    Graphics/StaticMeshRenderer.cpp
    Graphics/PostProcess.cpp
    Recorder/Recorder.cpp
)

list(APPEND hs
    RenderedEffectGenerator.h
	Graphics/CommandList.h
    Graphics/RenderPass.h
    Graphics/StaticMeshRenderer.h
    Graphics/PostProcess.h
    Recorder/Recorder.h
)

# OS Dependency
if (WIN32)

list(APPEND cpps
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
	3rdParty/imgui_platform/imgui_impl_dx11.cpp
)

elseif(APPLE)

list(APPEND cpps
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
)

else()


list(APPEND cpps
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
)
endif()

# Graphics
if (WIN32)
FILE(GLOB hs_g
	Graphics/Platform/DX11/*.h
	Graphics/Platform/GL/*.h
)

FILE(GLOB cpps_g
	Graphics/Platform/DX11/*.cpp
	Graphics/Platform/GL/*.cpp
)
else()
FILE(GLOB hs_g
	Graphics/Platform/GL/*.h
)

FILE(GLOB cpps_g
	Graphics/Platform/GL/*.cpp
)

endif()

list(APPEND hs ${hs_g})
list(APPEND cpps ${cpps_g})


FilterFolder("${hs}")
FilterFolder("${cpps}")

include_directories(
	${EFK_THIRDPARTY_INCLUDES}
	../include/
	../EffekseerRendererDX9/
	../EffekseerRendererGL/
	../EffekseerSoundOSMixer/
	../
)

add_library(Viewer SHARED ${cpps} ${hs})
target_link_directories(
    Viewer
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)
target_link_libraries(
    Viewer
    PRIVATE
    EditorCommon
    Boxer
    imgui
    debug "${GLFW3_LIB_DEBUG}"
    optimized "${GLFW3_LIB_RELEASE}"
    debug "${GD_LIB_DEBUG}"
	optimized "${GD_LIB_RELEASE}"
	debug "${LIBPNG_LIB_DEBUG}"
	optimized "${LIBPNG_LIB_RELEASE}"
	debug "${ZLIB_LIB_DEBUG}"
	optimized "${ZLIB_LIB_RELEASE}"
)

add_dependencies(Viewer 
	ExternalProject_zlib 
	ExternalProject_libpng 
	ExternalProject_libgd 
	ExternalProject_glfw 
	ExternalProject_OpenSoundMixer)

if(WIN32)
    add_dependencies(Viewer ExternalProject_glew)
endif()

link_directories(
    ${PROJECT_SOURCE_DIR}/../lib/
)


if(CLANG_FORMAT_ENABLED)
    clang_format(Viewer)
endif()

target_link_libraries(
    Viewer
    PRIVATE
    Effekseer
    EffekseerSoundOSMixer
    IPC
    nfd)


if (WIN32)

    target_link_libraries(
        Viewer
        PRIVATE
        EffekseerRendererDX9
        EffekseerRendererDX11
        EffekseerRendererGL
        EffekseerSoundOSMixer
        debug libglew32d
        debug OpenSoundMixerd
        optimized libglew32
        optimized OpenSoundMixer
    )

elseif(APPLE)

    FIND_PACKAGE(OpenAL REQUIRED)
    FIND_PACKAGE(Threads REQUIRED)
    FIND_PACKAGE(OpenGL REQUIRED)

    # For Mac
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

    target_link_libraries(
        Viewer
        PRIVATE
        EffekseerRendererGL
        OpenSoundMixer
        ${OpenAL_LIBRARY}
        "-framework OpenAL"
        ${CMAKE_THREAD_LIBS_INIT}
        ${COCOA_FRAMEWORK}
        ${OPENGL_LIBRARIES}
        ${IOKIT_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_VIDEO_FRAMEWORK})

    set_target_properties(Viewer PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)

else()

FIND_PACKAGE(PulseAudio REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${X11_INCLUDE_DIR})

target_link_libraries(
	Viewer
	PRIVATE
	EffekseerRendererGL
	dl
	OpenSoundMixer
	stdc++fs
	${OPENGL_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${PulseAudio_LIBRARY}
	${X11_LIBRARIES})

endif()

add_definitions(-D__EFFEKSEER_VIEWER_BUILD_AS_CMAKE__)

set_target_properties(Viewer PROPERTIES LINKER_LANGUAGE CXX)


set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release )

set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release )
