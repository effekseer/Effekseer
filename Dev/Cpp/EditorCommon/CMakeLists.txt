
cmake_minimum_required(VERSION 3.0)

project(EditorCommon)

set(srcs
    Platform/PlatformMisc.h
    GUI/JapaneseFont.h
)

if(WIN32)

    list(APPEND srcs
        Platform/Windows/PlatformMisc.cpp
    )

elseif(APPLE)

    list(APPEND srcs
        Platform/MacOSX/PlatformMisc.mm
    )

else()

    list(APPEND srcs
        Platform/Linux/PlatformMisc.cpp
    )

endif()

FilterFolder("${srcs}")

add_library(EditorCommon STATIC ${srcs})
set_target_properties(EditorCommon PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EditorCommon
    PUBLIC ${PROJECT_SOURCE_DIR}
	)