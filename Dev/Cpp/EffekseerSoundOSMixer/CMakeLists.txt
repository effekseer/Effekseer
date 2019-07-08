cmake_minimum_required(VERSION 2.8)
project(EffekseerSoundOSMixer CXX)

set(effekseer_sound_src
	EffekseerSound/EffekseerSoundOSMixer.Sound.cpp
	EffekseerSound/EffekseerSoundOSMixer.SoundLoader.cpp
	EffekseerSound/EffekseerSoundOSMixer.SoundPlayer.cpp)

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

add_library(EffekseerSoundOSMixer STATIC ${effekseer_sound_src})

include_directories(
	../include/
	../../../Downloads/OpenSoundMixer/src)

set_target_properties(EffekseerSoundOSMixer PROPERTIES LINKER_LANGUAGE CXX)
