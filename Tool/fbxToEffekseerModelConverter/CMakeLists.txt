set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

project(fbxToEffekseerModelConverter)

cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

set(srcs
	fbxToEfkMdl.FBXConverter.cpp
	fbxToMdl.VertexAnimation.cpp
	Utils.cpp
	main.cpp)

# Flags
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


include_directories($ENV{FBX_INCLUDE})

link_directories($ENV{FBX_LIB})

add_executable(fbxToEffekseerModelConverter ${srcs})

if (WIN32)

target_link_libraries(fbxToEffekseerModelConverter fbxsdk-mt)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

target_link_libraries(fbxToEffekseerModelConverter fbxsdk)

endif()



