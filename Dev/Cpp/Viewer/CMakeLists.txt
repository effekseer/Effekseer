cmake_minimum_required(VERSION 2.8)
project(Viewer CXX)

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
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_win.cpp
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_gl3.cpp
	3rdParty/imgui_platform/imgui_impl_dx9.cpp
)

elseif(APPLE)
list(APPEND hs
	3rdParty/nfd/nfd.h
	3rdParty/nfd/nfd_common.h
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_cocoa.m
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_gl3.cpp
)
else()
list(APPEND hs
	3rdParty/nfd/nfd.h
	3rdParty/nfd/nfd_common.h
)

list(APPEND cpps
	3rdParty/nfd/nfd_common.c
	3rdParty/nfd/nfd_gtk.c
	3rdParty/imgui_platform/imgui_impl_glfw.cpp
	3rdParty/imgui_platform/imgui_impl_gl3.cpp
)
endif()

# Graphics
if (WIN32)
FILE(GLOB hs_g
	Graphics/Platform/DX9/*.h
	Graphics/Platform/GL/*.h
)

FILE(GLOB cpps_g
	Graphics/Platform/DX9/*.cpp
	Graphics/Platform/GL/*.cpp
)
else()
FILE(GLOB hs_g
	Graphics/Platform/GL/*.h
)

FILE(GLOB cpps_g
	Graphics/Platform/GL/*.cpp
	3rdParty/imgui_glfw_gl3/*.cpp
)

endif()

list(APPEND hs ${hs_g})
list(APPEND cpps ${cpps_g})




foreach(FILE ${hs}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${cpps}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()


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

add_library(Viewer SHARED ${cpps} ${hs})
add_executable(ViewerTest ${cpps} ${hs} test.cpp)

include_directories(
	3rdParty/imgui/
	../include/
	../EffekseerRendererDX9/
	../EffekseerRendererGL/
	../EffekseerSoundOSMixer/
)

link_directories(

    ${PROJECT_SOURCE_DIR}/../lib/

)

if (WIN32)

target_link_libraries(Viewer effekseer effekseer_renderer_dx9 effekseer_renderer_gl EffekseerSoundOSMixer)
target_link_libraries(ViewerTest effekseer effekseer_renderer_dx9 effekseer_renderer_gl EffekseerSoundOSMixer)

else()

FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# For Mac
find_library(COCOA_FRAMEWORK Cocoa)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

target_link_libraries(
	Viewer
	effekseer
	effekseer_renderer_gl
	EffekseerSoundOSMixer
	glfw3
	gd
	z
	png16
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

target_link_libraries(
	ViewerTest
	effekseer
	effekseer_renderer_gl
	EffekseerSoundOSMixer
	glfw3
	gd
	z
	png16
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

endif()

set_target_properties(Viewer PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ViewerTest PROPERTIES LINKER_LANGUAGE CXX)
