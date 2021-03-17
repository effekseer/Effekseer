cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundOSMixer CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundOSMixer.Sound.cpp
	EffekseerSound/EffekseerSoundOSMixer.SoundLoader.cpp
	EffekseerSound/EffekseerSoundOSMixer.SoundPlayer.cpp)

add_library(EffekseerSoundOSMixer STATIC ${effekseer_sound_src})

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

include_directories(
	../include/
	../../../Downloads/OpenSoundMixer/src)

set_target_properties(EffekseerSoundOSMixer PROPERTIES LINKER_LANGUAGE CXX)
