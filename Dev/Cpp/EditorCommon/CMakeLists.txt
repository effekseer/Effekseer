
cmake_minimum_required(VERSION 3.0)

project(EditorCommon)

set(srcs
    Common/StringHelper.h
    Platform/PlatformMisc.h
    Platform/FileSystem.h
    GUI/JapaneseFont.h
    GUI/MainWindow.h
    GUI/MainWindow.cpp
    IO/IO.h
    IO/IO.cpp
    IO/StaticFile.h
    IO/StaticFile.cpp
    IO/FileReader.h
    IO/DefaultFileReader.h
    IO/DefaultFileReader.cpp
    IO/IPCFileReader.h
    IO/IPCFileReader.cpp
)

if(WIN32)

    list(APPEND srcs
        Platform/Windows/PlatformMisc.cpp
        Platform/Windows/FileSystem.cpp
    )

elseif(APPLE)

    list(APPEND srcs
        Platform/MacOSX/PlatformMisc.mm
        Platform/MacOSX/FileSystem.cpp
    )

else()

    list(APPEND srcs
        Platform/Linux/PlatformMisc.cpp
        Platform/Linux/FileSystem.cpp
    )

endif()

FilterFolder("${srcs}")

add_library(EditorCommon STATIC ${srcs})

target_link_libraries(
    EditorCommon
    PRIVATE
    IPC)


if (WIN32)
    target_link_libraries(
        EditorCommon
        PUBLIC
        debug glfw3d
        optimized glfw3
    )
else()
    target_link_libraries(
        EditorCommon
        PUBLIC
        glfw3
    )
endif()

add_dependencies(EditorCommon ExternalProject_glfw)

target_include_directories(
    EditorCommon
    PUBLIC
    ${EFK_THIRDPARTY_INCLUDES})

set_target_properties(EditorCommon PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EditorCommon
    PUBLIC ${PROJECT_SOURCE_DIR}
	)