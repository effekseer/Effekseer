cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundDSound CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundDSound.Sound.cpp
	EffekseerSound/EffekseerSoundDSound.SoundLoader.cpp
	EffekseerSound/EffekseerSoundDSound.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundDSound.SoundVoice.cpp)

# MSVC Flag
if (MSVC)
	if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		foreach (flag CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE)

			if (${flag} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
			endif()
			if (${flag} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
			endif()
		endforeach()
	endif()
endif()

add_library(EffekseerSoundDSound STATIC ${effekseer_sound_src})

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
