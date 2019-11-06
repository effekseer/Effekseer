
cmake_minimum_required(VERSION 3.0)

project(EditorCommon)

set(srcs
    Platform/PlatformMisc.h
    GUI/JapaneseFont.h
    IO/IO.h
    IO/StaticFile.h
    IO/FileReader.h
    IO/DefaultFileReader.h
    IO/IPCFileReader.h
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

target_link_libraries(
    EditorCommon
    PRIVATE
    IPC)

set_target_properties(EditorCommon PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EditorCommon
    PUBLIC ${PROJECT_SOURCE_DIR}
	)