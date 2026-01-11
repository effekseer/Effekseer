cmake_minimum_required(VERSION 3.10)

project(TestRuntimeFramework)

set(effekseer_test_src
    RenderingEnvironment/RenderingEnvironment.h
    RenderingEnvironment/RenderingEnvironment.cpp
    RenderingEnvironment/RenderingEnvironmentGL.h
    RenderingEnvironment/RenderingEnvironmentGL.cpp
    Runtime/EffectPlatform.h
    Runtime/EffectPlatform.cpp
    Runtime/EffectPlatformGLFW.h
    Runtime/EffectPlatformGLFW.cpp
    Runtime/EffectPlatformGL.h
    Runtime/EffectPlatformGL.cpp)

if(BUILD_DX12
   OR BUILD_METAL
   OR BUILD_VULKAN)
  list(APPEND effekseer_test_src Runtime/EffectPlatformLLGI.h
       Runtime/EffectPlatformLLGI.cpp)
endif()

if(BUILD_DX12)
  list(APPEND effekseer_test_src Runtime/EffectPlatformDX12.h
       Runtime/EffectPlatformDX12.cpp)
endif()

if(BUILD_METAL)
  list(APPEND effekseer_test_src Runtime/EffectPlatformMetal.h
       Runtime/EffectPlatformMetal.mm)
endif()

if(BUILD_VULKAN)
  list(APPEND effekseer_test_src Runtime/EffectPlatformVulkan.h
       Runtime/EffectPlatformVulkan.cpp)
endif()

if(WIN32)
  list(
    APPEND
    effekseer_test_src
    Runtime/EffectPlatformDX9.h
    Runtime/EffectPlatformDX9.cpp
    Runtime/EffectPlatformDX11.h
    Runtime/EffectPlatformDX11.cpp
    RenderingEnvironment/RenderingEnvironmentDX11.h
    RenderingEnvironment/RenderingEnvironmentDX11.cpp)
endif()

include_directories(
  ${EFK_THIRDPARTY_INCLUDES} ../Effekseer/ ../EffekseerRendererGL/
  ../EffekseerSoundAL/ ../OpenGLHelper/)

if(WIN32)
  include_directories(${EFK_THIRDPARTY_INCLUDES} ../EffekseerRendererDX11/)
endif()

set(common_lib)
set(common_inc)

list(APPEND common_lib EffekseerRendererGL)

if(WIN32)
  list(APPEND common_lib EffekseerRendererDX11)
  list(APPEND common_lib EffekseerRendererDX9)
endif()

if(BUILD_DX12
   OR BUILD_METAL
   OR BUILD_VULKAN)
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

list(APPEND common_lib Effekseer)
list(APPEND common_lib OpenGLHelper)

if(MSVC)

else()
  find_package(Threads REQUIRED)
  list(APPEND common_lib ${CMAKE_THREAD_LIBS_INIT})

  find_package(OpenGL)
  list(APPEND common_inc ${OPENGL_INCLUDE_DIR})
  list(APPEND common_lib ${OPENGL_LIBRARY})

  find_package(OpenAL)
  list(APPEND common_inc ${OPENAL_INCLUDE_DIR})
  list(APPEND common_lib ${OPENAL_LIBRARY})
endif()

add_library(${PROJECT_NAME} ${effekseer_test_src})
target_link_directories(${PROJECT_NAME} PUBLIC
                        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})
target_include_directories(${PROJECT_NAME} PUBLIC ./)

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)

  target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

  target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

  # TODO Improve it
  target_link_libraries(
    ${PROJECT_NAME} PUBLIC glslang glslang-default-resource-limits SPIRV
                           OSDependent)

  target_link_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/../3rdParty/LLGI/EP/Install/glslang/lib)
endif()

add_dependencies(${PROJECT_NAME} ExternalProject_glfw)

if(BUILD_WITH_EASY_PROFILER)
  add_dependencies(${PROJECT_NAME} ExternalProject_easy_profiler)
endif()

if(BUILD_WITH_POSTFIX)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${common_lib} debug glfw3d
                                               optimized glfw3)
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC ${common_lib} glfw3 dl)
endif()

linkapplelibs(${PROJECT_NAME})

if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${X11_LIBRARIES})
  find_package(Threads REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

if(FROM_CI)
  target_compile_definitions(${PROJECT_NAME} PUBLIC __FROM_CI__)
endif()

# FIXME: Mixed measures with sln. for VS2015
add_definitions(-D__EFFEKSEER_TEST_BUILD_AS_CMAKE__)

if(APPLE)
  add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

filterfolder("${effekseer_test_src}")

if(CLANG_FORMAT_ENABLED)
  clang_format(${PROJECT_NAME})
endif()

if(USE_LIBPNG_LOADER)
  add_dependencies(${PROJECT_NAME} ExternalProject_libpng)

  target_link_directories(${PROJECT_NAME} PUBLIC
                          ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

  target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC debug
           "${LIBPNG_LIB_DEBUG}"
           optimized
           "${LIBPNG_LIB_RELEASE}"
           debug
           "${ZLIB_LIB_DEBUG}"
           optimized
           "${ZLIB_LIB_RELEASE}")
endif()
