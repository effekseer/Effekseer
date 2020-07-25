cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererMetal)

option(USE_INTERNAL_COMMAND_BUFFER "Use Internal Command Buffer" ON)
option(USE_INTERNAL_RENDER_PASS "Use Internal Render Pass" ON)

if (USE_INTERNAL_COMMAND_BUFFER)
	add_definitions(-D__EFFEKSEER_RENDERERMETAL_INTERNAL_COMMAND_BUFFER__)
endif()

if (USE_INTERNAL_RENDER_PASS)
	add_definitions(-D__EFFEKSEER_RENDERERMETAL_INTERNAL_RENDER_PASS__)
endif()

#--------------------
# Files

file(GLOB_RECURSE LOCAL_SOURCES_Common ../EffekseerRendererCommon/*.h ../EffekseerRendererCommon/*.cpp)

list(REMOVE_ITEM LOCAL_SOURCES_Common
  ${CMAKE_CURRENT_SOURCE_DIR}/../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.cpp
)

if(NOT USE_INTERNAL_LOADER)
    list(REMOVE_ITEM LOCAL_SOURCES_Common
        ${PROJECT_SOURCE_DIR}/../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp
        ${PROJECT_SOURCE_DIR}/../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h
    )
endif()

source_group("EffekseerRendererCommon" FILES ${LOCAL_SOURCES_Common})

file(GLOB_RECURSE LOCAL_HEADERS_Metal *.h)
file(GLOB_RECURSE LOCAL_SOURCES_Metal *.cpp *.mm)
list(APPEND LOCAL_HEADERS_Metal ../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.h)
list(APPEND LOCAL_SOURCES_Metal ../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.cpp)
FilterFolder("${LOCAL_HEADERS_Metal}")
FilterFolder("${LOCAL_SOURCES_Metal}")

file(GLOB_RECURSE LOCAL_SOURCES_LLGI
    ../EffekseerRendererLLGI/*.h
    ../EffekseerRendererLLGI/*.cpp)

source_group("EffekseerRendererLLGI" FILES ${LOCAL_SOURCES_LLGI})

set(LOCAL_SOURCES
    ${LOCAL_SOURCES_Common}
    ${LOCAL_HEADERS_Metal}
    ${LOCAL_SOURCES_Metal}
    ${LOCAL_SOURCES_LLGI})

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} LLGI)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LOCAL_HEADERS_Metal}")
add_dependencies(${PROJECT_NAME} Effekseer)

if(USE_LIBPNG_LOADER AND USE_INTERNAL_LOADER)
    add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng) 
endif()

#--------------------
# Install

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererMetal
    PUBLIC_HEADER DESTINATION include/EffekseerRendererMetal
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
