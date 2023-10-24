cmake_minimum_required(VERSION 3.15)
project(EffekseerSoundDSound CXX)

set(effekseer_sound_h
    EffekseerSound/EffekseerSoundDSound.SoundImplemented.h
    EffekseerSound/EffekseerSoundDSound.SoundLoader.h
    EffekseerSound/EffekseerSoundDSound.SoundPlayer.h
    EffekseerSound/EffekseerSoundDSound.SoundVoice.h)

set(effekseer_sound_src
    EffekseerSound/EffekseerSoundDSound.Sound.cpp
    EffekseerSound/EffekseerSoundDSound.SoundLoader.cpp
    EffekseerSound/EffekseerSoundDSound.SoundPlayer.cpp
    EffekseerSound/EffekseerSoundDSound.SoundVoice.cpp)

set(PublicHeaders
    EffekseerSoundDSound.h)

set(files
    ${PublicHeaders}
    ${effekseer_sound_h}
    ${effekseer_sound_src})


FilterFolder("${files}")

add_library(EffekseerSoundDSound STATIC
    ${effekseer_sound_h}
    ${effekseer_sound_src})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/EffekseerSoundDSound>)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

WarnError(${PROJECT_NAME})

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerSoundDSound/${dir})
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
