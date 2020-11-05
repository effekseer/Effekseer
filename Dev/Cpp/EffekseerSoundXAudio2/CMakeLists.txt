cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundXAudio2 CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundXAudio2.Sound.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundLoader.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundVoice.cpp)

add_library(EffekseerSoundXAudio2 STATIC ${effekseer_sound_src})
if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

include_directories(../include/)

set_target_properties(EffekseerSoundXAudio2 PROPERTIES LINKER_LANGUAGE CXX)

if (MSVC)
set_target_properties(EffekseerSoundXAudio2 PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundXAudio2" )
set_target_properties(EffekseerSoundXAudio2 PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundXAudio2" )
else()
set_target_properties(EffekseerSoundXAudio2 PROPERTIES OUTPUT_NAME "EffekseerSoundXAudio2" )
endif()

if (MSVC)
install(
	TARGETS EffekseerSoundXAudio2
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()
