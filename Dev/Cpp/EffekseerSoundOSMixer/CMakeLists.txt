cmake_minimum_required(VERSION 3.15)
project(EffekseerSoundOSMixer CXX)

set(effekseer_sound_h
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.SoundImplemented.h
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.SoundLoader.h
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.SoundPlayer.h)

set(effekseer_sound_src
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.Sound.cpp
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.SoundLoader.cpp
    EffekseerSoundOSMixer/EffekseerSoundOSMixer.SoundPlayer.cpp)

set(PublicHeaders EffekseerSoundOSMixer.h)

set(files ${PublicHeaders} ${effekseer_sound_h} ${effekseer_sound_src})

filterfolder("${files}")

add_library(EffekseerSoundOSMixer STATIC ${effekseer_sound_h}
                                         ${effekseer_sound_src})

target_include_directories(EffekseerSoundOSMixer
                           PRIVATE ${EFK_THIRDPARTY_INCLUDES})

target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)

target_link_directories(EffekseerSoundOSMixer PRIVATE
                        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

target_link_libraries(${PROJECT_NAME} PUBLIC debug "${OSM_LIB_DEBUG}" optimized
                                             "${OSM_LIB_RELEASE}")

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
