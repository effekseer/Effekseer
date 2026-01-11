# The project's name
project(EffekseerMaterialEditor)

# Check cmake version
cmake_minimum_required(VERSION 3.15)

# For MSVC
if(MSVC)
  option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
      "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
      "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
endif()

file(GLOB srcs *.cpp Graphics/*.cpp)

file(GLOB hs *.h Dialog/*.h Graphics/*.h)

# Add compile targets
add_executable(EffekseerMaterialEditor ${hs} ${srcs})

# Flags
if(MSVC)
  # CMAKE_MSVC_RUNTIME_LIBRARY can be only used if minimum required is larger
  # than 3.17
  if(USE_MSVC_RUNTIME_LIBRARY_DLL)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  else()
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

if(WIN32)
  add_dependencies(EffekseerMaterialEditor ExternalProject_glfw)
endif()

target_compile_definitions(EffekseerMaterialEditor PRIVATE NOMINMAX)

target_include_directories(
  EffekseerMaterialEditor
  PRIVATE ${EFK_THIRDPARTY_INCLUDES} ../include/ ../EffekseerMaterial/
          ../OpenGLHelper/ ThirdParty/picojson/)

target_link_directories(EffekseerMaterialEditor PRIVATE
                        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

target_link_libraries(
  EffekseerMaterialEditor
  PRIVATE Effekseer
          EffekseerRendererGL
          OpenGLHelper
          EffekseerMaterial
          EditorCommon
          Boxer
          nfd
          imgui
          imgui_platform
          imgui-node-editor
          ImGradientHDR
          IPC)

if(USE_LIBPNG_LOADER)
  target_link_directories(${PROJECT_NAME} PRIVATE
                          ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE debug
            "${LIBPNG_LIB_DEBUG}"
            optimized
            "${LIBPNG_LIB_RELEASE}"
            debug
            "${ZLIB_LIB_DEBUG}"
            optimized
            "${ZLIB_LIB_RELEASE}")

endif()

if(APPLE)

  find_package(Threads REQUIRED)
  find_package(OpenGL REQUIRED)

  target_link_libraries(EffekseerMaterialEditor
                        PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES})

  linkapplelibs(${PROJECT_NAME})
  set_target_properties(EffekseerMaterialEditor
                        PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)

elseif(UNIX AND NOT APPLE)

  # For Linux

  find_package(Threads REQUIRED)
  find_package(OpenGL REQUIRED)

  find_package(X11 REQUIRED)
  find_package(Threads REQUIRED)

  target_include_directories(EffekseerMaterialEditor
                             PRIVATE ${OPENGL_INCLUDE_DIR} ${X11_INCLUDE_DIR})

  target_link_libraries(
    EffekseerMaterialEditor PRIVATE dl stdc++fs ${CMAKE_THREAD_LIBS_INIT}
                                    ${OPENGL_LIBRARY} ${X11_LIBRARIES})

endif()

set_target_properties(EffekseerMaterialEditor PROPERTIES LINKER_LANGUAGE CXX)

set(files ${hs} ${srcs})

set(files ${srcs} ${hs})

if(CLANG_FORMAT_ENABLED)
  clang_format(EffekseerMaterialEditor)
endif()

filterfolder("${files}")

set_target_properties(
  EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                     ${CMAKE_SOURCE_DIR}/Dev/release)
set_target_properties(
  EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
                                     ${CMAKE_SOURCE_DIR}/Dev/release)
set_target_properties(
  EffekseerMaterialEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
                                     ${CMAKE_SOURCE_DIR}/Dev/release)
