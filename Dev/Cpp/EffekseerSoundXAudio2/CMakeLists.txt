cmake_minimum_required(VERSION 3.15)
project(EffekseerSoundXAudio2 CXX)

set(effekseer_sound_h
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundImplemented.h
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundLoader.h
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundPlayer.h
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundVoice.h)

set(effekseer_sound_src
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.Sound.cpp
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundLoader.cpp
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundPlayer.cpp
    EffekseerSoundXAudio2/EffekseerSoundXAudio2.SoundVoice.cpp)

set(PublicHeaders EffekseerSoundXAudio2.h)

set(files ${PublicHeaders} ${effekseer_sound_h} ${effekseer_sound_src})

filterfolder("${files}")

add_library(EffekseerSoundXAudio2 STATIC ${effekseer_sound_h}
                                         ${effekseer_sound_src})

target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include>)

if(CLANG_FORMAT_ENABLED)
  clang_format(${PROJECT_NAME})
endif()

warnerror(${PROJECT_NAME})

foreach(file ${PublicHeaders})
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
