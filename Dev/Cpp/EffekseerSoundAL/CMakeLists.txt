cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundAL CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundAL.Sound.cpp
	EffekseerSound/EffekseerSoundAL.SoundLoader.cpp
	EffekseerSound/EffekseerSoundAL.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundAL.SoundVoice.cpp)

add_library(EffekseerSoundAL STATIC ${effekseer_sound_src})

include_directories(../include/)

set_target_properties(EffekseerSoundAL PROPERTIES LINKER_LANGUAGE CXX)

if (MSVC)
set_target_properties(EffekseerSoundAL PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundAL" )
set_target_properties(EffekseerSoundAL PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundAL" )
else()
set_target_properties(EffekseerSoundAL PROPERTIES OUTPUT_NAME "EffekseerSoundAL" )
endif()

if (MSVC)
install(
	TARGETS EffekseerSoundAL
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()
