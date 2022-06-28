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
	3rdParty/imgui_addon/fcurve/*.cpp
	3rdParty/imgui_addon/ImGuizmo/*.cpp
	3rdParty/imgui_addon/implot/*.cpp
	EffekseerTool/*.cpp
	Graphics/*.cpp
	GUI/*.cpp
	*.cpp
	*.cxx
)

list(APPEND cpps
    3D/Effect.cpp
    3D/EffectFactory.cpp
    3D/EffectRenderer.cpp
    3D/EffectSetting.cpp
    3D/FileInterface.cpp
    3D/MainScreenEffectRenderer.cpp
    3D/ViewPointController.cpp
    Graphics/StaticMeshRenderer.cpp
    Graphics/LineRenderer.cpp
    Graphics/ImageRenderer.cpp
    Graphics/PostProcess.cpp
    Graphics/PostEffects.cpp
    Graphics/GraphicsDevice.cpp
    GUI/Image.cpp
    GUI/ReloadableImage.cpp
    GUI/RenderImage.cpp
    GUI/GradientHDRState.cpp
    Network/Network.cpp
    Recorder/EffectRecorder.cpp
    Recorder/AVIExporter.cpp
    Recorder/GifHelper.cpp
    Recorder/PNGHelper.cpp
    Sound/SoundDevice.cpp
    Utils/Logger.cpp
    ProcessConnector.cpp
)

list(APPEND hs
    3D/Effect.h
    3D/EffectFactory.h
    3D/EffectRenderer.h
    3D/EffectSetting.h
    3D/FileInterface.h
    3D/MainScreenEffectRenderer.h
    3D/ViewPointController.h
    Graphics/Color.h
    Graphics/StaticMeshRenderer.h
    Graphics/LineRenderer.h
    Graphics/ImageRenderer.h
    Graphics/PostProcess.h
    Graphics/PostEffects.h
    Graphics/GraphicsDevice.h
    GUI/Image.h
    GUI/ReloadableImage.h
    GUI/RenderImage.h
    GUI/GradientHDRState.h
    Network/Network.h
    Math/Matrix44F.h
    Math/Vector2I.h
    Math/Vector3F.h
    Recorder/EffectRecorder.h
    Sound/SoundDevice.h
    Utils/Logger.h
    ProcessConnector.h
)

# OS Dependency
if (WIN32)

list(APPEND cpps
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
	3rdParty/imgui_platform/imgui_impl_dx11.cpp
	Recorder/Windows/RecorderCallbackH264.cpp
)

list(APPEND hs
    Recorder/Windows/RecorderCallbackH264.h
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
    ImGradientHDR
    debug "${GD_LIB_DEBUG}"
    optimized "${GD_LIB_RELEASE}"
    debug "${LIBPNG_LIB_DEBUG}"
    optimized "${LIBPNG_LIB_RELEASE}"
    debug "${ZLIB_LIB_DEBUG}"
    optimized "${ZLIB_LIB_RELEASE}"
    debug "${OSM_LIB_DEBUG}"
    optimized "${OSM_LIB_RELEASE}"
)

add_dependencies(Viewer 
	ExternalProject_zlib 
	ExternalProject_libpng 
	ExternalProject_libgd 
	ExternalProject_glfw 
	ExternalProject_OpenSoundMixer)

if(WIN32)
    add_dependencies(Viewer ExternalProject_glew)
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
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
        EffekseerRendererDX11
        EffekseerRendererGL
        EffekseerSoundOSMixer
        debug libglew32d
        optimized libglew32
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

if (WIN32)
    target_compile_definitions(Viewer PRIVATE NOMINMAX)
endif()

set_target_properties(Viewer PROPERTIES LINKER_LANGUAGE CXX)


set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release )

set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release )
set_target_properties(Viewer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release )
