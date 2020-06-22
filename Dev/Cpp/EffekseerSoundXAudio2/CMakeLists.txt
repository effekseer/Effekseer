cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundXAudio2 CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundXAudio2.Sound.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundLoader.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundXAudio2.SoundVoice.cpp)

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

add_library(EffekseerSoundXAudio2 STATIC ${effekseer_sound_src})

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
