cmake_minimum_required(VERSION 2.8)
project(Viewer CXX)

# For Mac
if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

FILE(GLOB hs
	3rdParty/imgui/*.h
	EffekseerTool/*.h
	Graphics/*.h
	GUI/*.h
	*.h
)

FILE(GLOB cpps
	3rdParty/imgui/*.cpp
	3rdParty/imgui_addon/imguidock/*.cpp
	3rdParty/imgui_addon/imguihelper/*.cpp
	3rdParty/imgui_addon/fcurve/*.cpp
	EffekseerTool/*.cpp
	Graphics/*.cpp
	GUI/*.cpp
	*.cpp
	*.cxx
)

# OS Dependency
if (WIN32)
list(APPEND hs
	3rdParty/nfd/nfd.h
	3rdParty/nfd/nfd_common.h
	3rdParty/Boxer/boxer.h
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_win.cpp
	3rdParty/Boxer/boxer_win.cpp
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
	3rdParty/imgui_platform/imgui_impl_dx11.cpp
)

elseif(APPLE)

list(APPEND hs
	3rdParty/nfd/nfd.h
	3rdParty/nfd/nfd_common.h
	3rdParty/Boxer/boxer.h
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_cocoa.m
	3rdParty/Boxer/boxer_osx.mm
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_opengl3.cpp
)

else()

list(APPEND hs
	3rdParty/nfd/nfd.h
	3rdParty/nfd/nfd_common.h
	3rdParty/Boxer/boxer.h
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_gtk.c
	3rdParty/Boxer/boxer_linux.cpp
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

# MSVC Flag
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

include_directories(
	${EFK_THIRDPARTY_INCLUDES}
	3rdParty/imgui/
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

if(NOT APPLE)
    add_dependencies(Viewer ExternalProject_glew)
endif()

link_directories(
    ${PROJECT_SOURCE_DIR}/../lib/
)

if (WIN32)

    target_link_libraries(
        Viewer
        PRIVATE
        Effekseer
        EffekseerRendererDX9
        EffekseerRendererDX11
        EffekseerRendererGL
        EffekseerSoundOSMixer
        IPC
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
        Effekseer
        EffekseerRendererGL
        EffekseerSoundOSMixer
        IPC
        OpenSoundMixer
        ${OpenAL_LIBRARY}
        "-framework OpenAL"
        ${CMAKE_THREAD_LIBS_INIT}
        ${COCOA_FRAMEWORK}
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        ${IOKIT_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_VIDEO_FRAMEWORK})

else()

FIND_PACKAGE(PulseAudio REQUIRED)
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
	Viewer
	PRIVATE
	Effekseer
	EffekseerRendererGL
	EffekseerSoundOSMixer
	IPC
	dl
	OpenSoundMixer
	${OPENGL_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${PulseAudio_LIBRARY}
	${GTK3_LIBRARIES}
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
