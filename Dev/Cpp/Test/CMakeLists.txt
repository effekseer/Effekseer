cmake_minimum_required(VERSION 3.10)

project(TestCpp)

if(BUILD_WEBGPU_BROWSER_TEST AND NOT EMSCRIPTEN)
  message(FATAL_ERROR "BUILD_WEBGPU_BROWSER_TEST requires an Emscripten CMake toolchain.")
endif()
if(BUILD_WEBGPU_BROWSER_TEST AND NOT BUILD_WEBGPU)
  message(FATAL_ERROR "BUILD_WEBGPU_BROWSER_TEST requires BUILD_WEBGPU.")
endif()

if(BUILD_WEBGPU_BROWSER_TEST)
  set(effekseer_test_src main.cpp TestHelper.h TestHelper.cpp
                         Runtime/WebGPUBrowserRendering.cpp)
  set(effekseer_browser_player_src Runtime/WebGPUBrowserPlayer.cpp)
else()
  set(effekseer_test_src
      main.cpp
      TestHelper.h
      TestHelper.cpp
      Runtime/BasicRendering.h
      Runtime/BasicRendering.cpp
      Runtime/RuntimeTest.cpp
      Runtime/Performance.cpp
      Runtime/TextureFormats.cpp
      Runtime/Vertex.cpp
      Runtime/ResourceManager.cpp
      Runtime/String.cpp
      Runtime/Network.cpp
      Runtime/Math.cpp
      Runtime/Misc.cpp
      Runtime/VectorField.cpp
      Backend/Helper.h
      Backend/Helper.cpp
      Backend/Textures.cpp
      Backend/Mesh.cpp
      Backend/RenderPass.cpp
      Backend/GPUParticle.cpp
      Backend/OpenGL.cpp)
endif()

include_directories(${EFK_THIRDPARTY_INCLUDES} ../Effekseer/
                    ../EffekseerRendererGL/ ../EffekseerSoundAL/)

if(WIN32)
  include_directories(${EFK_THIRDPARTY_INCLUDES} ../EffekseerRendererDX11/)
endif()

set(common_lib)
set(common_inc)

if(NOT BUILD_WEBGPU_BROWSER_TEST)
  list(APPEND common_lib EffekseerRendererGL)
endif()

if(WIN32)
  list(APPEND common_lib EffekseerRendererDX11)
  list(APPEND common_lib EffekseerRendererDX9)
endif()

if(BUILD_DX12
   OR BUILD_METAL
   OR BUILD_VULKAN
   OR BUILD_WEBGPU)
  list(APPEND common_lib LLGI)
endif()

if(BUILD_DX12)
  list(APPEND common_lib EffekseerRendererDX12)
endif()

if(BUILD_METAL)
  list(APPEND common_lib EffekseerRendererMetal)
endif()

if(BUILD_VULKAN)
  list(APPEND common_lib EffekseerRendererVulkan)
endif()

if(BUILD_WEBGPU AND NOT BUILD_WEBGPU_BROWSER_TEST)
  list(APPEND effekseer_test_src Runtime/BasicRenderingWebGPU.cpp)
  list(APPEND common_lib EffekseerRendererWebGPU)
elseif(BUILD_WEBGPU)
  list(APPEND common_lib EffekseerRendererWebGPU)
endif()

list(APPEND common_lib Effekseer)

if(BUILD_WEBGPU_BROWSER_TEST)
elseif(MSVC)

else()
  find_package(Threads REQUIRED)
  list(APPEND common_lib ${CMAKE_THREAD_LIBS_INIT})

  find_package(OpenGL)
  list(APPEND common_inc ${OPENGL_INCLUDE_DIR})
  list(APPEND common_lib ${OPENGL_LIBRARY})

  find_package(OpenAL)
  list(APPEND common_inc ${OPENAL_INCLUDE_DIR})
  list(APPEND common_lib ${OPENAL_LIBRARY})

  if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(APPKIT_LIBRARY AppKit)
    find_library(METALKIT_LIBRARY MetalKit)
    find_library(QUARTZ_CORE_LIBRARY QuartzCore)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
    list(
      APPEND
      common_lib
      ${COCOA_FRAMEWORK}
      ${APPKIT_LIBRARY}
      ${METAL_LIBRARY}
      ${METALKIT_LIBRARY}
      ${QUARTZ_CORE_LIBRARY}
      ${OPENGL_LIBRARIES}
      ${IOKIT_FRAMEWORK}
      ${CORE_FOUNDATION_FRAMEWORK}
      ${CORE_VIDEO_FRAMEWORK})
  endif()
endif()

add_executable(${PROJECT_NAME} ${effekseer_test_src})
target_link_directories(${PROJECT_NAME} PRIVATE
                        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PRIVATE TestRuntimeFramework)

if(BUILD_WEBGPU_BROWSER_TEST)
  add_executable(EffekseerWebGPUBrowserPlayer ${effekseer_browser_player_src})
  target_link_libraries(EffekseerWebGPUBrowserPlayer PRIVATE TestRuntimeFramework)
  target_link_directories(EffekseerWebGPUBrowserPlayer
                          PRIVATE ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})
endif()

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)

  target_include_directories(TestCpp PRIVATE ${Vulkan_INCLUDE_DIRS})

  target_link_libraries(TestCpp PRIVATE ${Vulkan_LIBRARIES})

  # TODO Improve it
  target_link_libraries(TestCpp PRIVATE glslang SPIRV OSDependent)

  target_link_directories(
    TestCpp PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../3rdParty/LLGI/EP/Install/glslang/lib)
endif()

if(NOT BUILD_WEBGPU_BROWSER_TEST)
  add_dependencies(TestCpp ExternalProject_glfw)
endif()

if(BUILD_WITH_EASY_PROFILER)
  add_dependencies(TestCpp ExternalProject_easy_profiler)
endif()

if(UNIX AND NOT APPLE AND NOT BUILD_WEBGPU_BROWSER_TEST)
  find_package(X11 REQUIRED)
  target_link_libraries(TestCpp PRIVATE ${X11_LIBRARIES})
  find_package(Threads REQUIRED)
  target_link_libraries(TestCpp PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(FROM_CI)
  target_compile_definitions(TestCpp PRIVATE __FROM_CI__)
endif()

# FIXME: Mixed measures with sln. for VS2015
add_definitions(-D__EFFEKSEER_TEST_BUILD_AS_CMAKE__)

if(APPLE)
  add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

if(MSVC)
  set_target_properties(TestCpp PROPERTIES OUTPUT_NAME_DEBUG "TestCpp")
  set_target_properties(TestCpp PROPERTIES OUTPUT_NAME_RELEASE "TestCpp")
else()
  set_target_properties(TestCpp PROPERTIES OUTPUT_NAME "TestCpp")
endif()

if(BUILD_WEBGPU_BROWSER_TEST)
  set_target_properties(TestCpp PROPERTIES SUFFIX ".html")
  target_link_options(
    TestCpp
    PRIVATE
    "--pre-js=${CMAKE_CURRENT_SOURCE_DIR}/Runtime/browser/pre_effekseer_webgpu_test.js"
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../../../TestData@/TestData"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASYNCIFY=1"
    "-sASSERTIONS=1")

  set_target_properties(EffekseerWebGPUBrowserPlayer PROPERTIES SUFFIX ".html")
  target_link_options(
    EffekseerWebGPUBrowserPlayer
    PRIVATE
    "--pre-js=${CMAKE_CURRENT_SOURCE_DIR}/Runtime/browser/pre_effekseer_webgpu_player.js"
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../../../TestData@/TestData"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASYNCIFY=1"
    "-sASSERTIONS=1")

  enable_testing()
  find_program(NODE_EXE node)
  if(NODE_EXE)
    add_test(
      NAME Effekseer_WebGPU_Browser
      COMMAND
        ${NODE_EXE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/LLGI/src_test/browser/run_webgpu_browser_test.mjs
        $<TARGET_FILE:TestCpp> --filter=Runtime.WebGPUBrowser.*)
  endif()
endif()

filterfolder("${effekseer_test_src}")

if(CLANG_FORMAT_ENABLED)
  clang_format(TestCpp)
endif()
