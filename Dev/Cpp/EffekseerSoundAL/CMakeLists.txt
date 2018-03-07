cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundAL CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundAL.Sound.cpp
	EffekseerSound/EffekseerSoundAL.SoundLoader.cpp
	EffekseerSound/EffekseerSoundAL.SoundPlayer.cpp
	EffekseerSound/EffekseerSoundAL.SoundVoice.cpp)

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
