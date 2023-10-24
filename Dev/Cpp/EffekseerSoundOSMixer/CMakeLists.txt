cmake_minimum_required(VERSION 3.15)
project(EffekseerSoundOSMixer CXX)

set(effekseer_sound_h
    EffekseerSound/EffekseerSoundOSMixer.SoundImplemented.h
    EffekseerSound/EffekseerSoundOSMixer.SoundLoader.h
    EffekseerSound/EffekseerSoundOSMixer.SoundPlayer.h)

set(effekseer_sound_src
    EffekseerSound/EffekseerSoundOSMixer.Sound.cpp
    EffekseerSound/EffekseerSoundOSMixer.SoundLoader.cpp
    EffekseerSound/EffekseerSoundOSMixer.SoundPlayer.cpp)

set(PublicHeaders
    EffekseerSoundOSMixer.h)

set(files
    ${PublicHeaders}
    ${effekseer_sound_h}
    ${effekseer_sound_src})


FilterFolder("${files}")

add_library(EffekseerSoundOSMixer STATIC
    ${effekseer_sound_h}
    ${effekseer_sound_src})

target_link_directories(
    EffekseerSoundOSMixer
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    debug "${OSM_LIB_DEBUG}"
    optimized "${OSM_LIB_RELEASE}")

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/EffekseerSoundOSMixer>)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

WarnError(${PROJECT_NAME})

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerSoundOSMixer/${dir})
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
