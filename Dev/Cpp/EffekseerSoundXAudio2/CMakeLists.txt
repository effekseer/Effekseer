cmake_minimum_required(VERSION 3.15)
project(EffekseerSoundXAudio2 CXX)

set(effekseer_sound_h
    EffekseerSound/EffekseerSoundXAudio2.SoundImplemented.h
    EffekseerSound/EffekseerSoundXAudio2.SoundLoader.h
    EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.h
    EffekseerSound/EffekseerSoundXAudio2.SoundVoice.h)

set(effekseer_sound_src
    EffekseerSound/EffekseerSoundXAudio2.Sound.cpp
    EffekseerSound/EffekseerSoundXAudio2.SoundLoader.cpp
    EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.cpp
    EffekseerSound/EffekseerSoundXAudio2.SoundVoice.cpp)

set(PublicHeaders
    EffekseerSoundXAudio2.h)

set(files
    ${PublicHeaders}
    ${effekseer_sound_h}
    ${effekseer_sound_src})


FilterFolder("${files}")

add_library(EffekseerSoundXAudio2 STATIC
    ${effekseer_sound_h}
    ${effekseer_sound_src})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/EffekseerSoundXAudio2>)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

WarnError(${PROJECT_NAME})

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerSoundXAudio2/${dir})
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
