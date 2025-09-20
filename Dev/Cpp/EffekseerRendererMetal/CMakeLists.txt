cmake_minimum_required (VERSION 3.15)
project(EffekseerRendererMetal)

option(USE_INTERNAL_COMMAND_BUFFER "Use Internal Command Buffer" ON)
option(USE_INTERNAL_RENDER_PASS "Use Internal Render Pass" ON)

if (USE_INTERNAL_COMMAND_BUFFER)
	add_definitions(-D__EFFEKSEER_RENDERERMETAL_INTERNAL_COMMAND_BUFFER__)
endif()

if (USE_INTERNAL_RENDER_PASS)
	add_definitions(-D__EFFEKSEER_RENDERERMETAL_INTERNAL_RENDER_PASS__)
endif()

file(GLOB_RECURSE LOCAL_HEADERS_Metal *.h)
file(GLOB_RECURSE LOCAL_SOURCES_Metal *.cpp *.mm)
list(APPEND LOCAL_HEADERS_Metal ../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.h)
list(APPEND LOCAL_SOURCES_Metal ../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.cpp)
list(APPEND LOCAL_SOURCES_Metal ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_Metal ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

FilterFolder("${LOCAL_HEADERS_Metal}")
FilterFolder("${LOCAL_SOURCES_Metal}")

set(LOCAL_SOURCES
    ${LOCAL_HEADERS_Metal}
    ${LOCAL_SOURCES_Metal})


set(PublicHeaders
    EffekseerRendererMetal.h
    EffekseerRendererMetal/EffekseerRendererMetal.Base.Pre.h
    EffekseerRendererMetal/EffekseerRendererMetal.Renderer.h)

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PublicHeaders}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon EffekseerRendererLLGI LLGI)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

# WarnError(${PROJECT_NAME})

#--------------------
# Install

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
