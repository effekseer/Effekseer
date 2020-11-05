cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundDSound CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundDSound.Sound.cpp
	EffekseerSound/EffekseerSoundDSound.SoundLoader.cpp
	EffekseerSound/EffekseerSoundDSound.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundDSound.SoundVoice.cpp)

add_library(EffekseerSoundDSound STATIC ${effekseer_sound_src})
if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

include_directories(../include/)

set_target_properties(EffekseerSoundDSound PROPERTIES LINKER_LANGUAGE CXX)

if (MSVC)
set_target_properties(EffekseerSoundDSound PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundDSound" )
set_target_properties(EffekseerSoundDSound PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundDSound" )
else()
set_target_properties(EffekseerSoundDSound PROPERTIES OUTPUT_NAME "EffekseerSoundDSound" )
endif()

if (MSVC)
install(
	TARGETS EffekseerSoundDSound
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()
