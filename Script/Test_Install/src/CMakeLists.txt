cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(install_use)

add_executable(install_use main.cpp)

list(APPEND CMAKE_PREFIX_PATH ../install/lib/cmake)

find_package(Effekseer REQUIRED)
target_link_libraries(install_use PUBLIC Effekseer)

find_package(EffekseerRendererCommon REQUIRED)
target_link_libraries(install_use PUBLIC EffekseerRendererCommon)

if(WIN32)
  find_package(EffekseerRendererDX11 REQUIRED)
  target_link_libraries(install_use PUBLIC EffekseerRendererDX11)

  find_package(EffekseerSoundXAudio2 REQUIRED)
  target_link_libraries(install_use PUBLIC EffekseerSoundXAudio2)
endif()
