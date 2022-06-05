
cmake_minimum_required(VERSION 3.0)

project(EditorCommon)

set(srcs
    Common/StringHelper.h
    Platform/PlatformMisc.h
    Platform/FileSystem.h
    GUI/JapaneseFont.h
    GUI/MainWindow.h
    GUI/MainWindow.cpp
    GUI/FramerateController.h
    GUI/FramerateController.cpp
    GUI/Misc.h
    GUI/Misc.cpp
    IO/IO.h
    IO/IO.cpp
    IO/StaticFile.h
    IO/StaticFile.cpp
    IO/FileReader.h
    IO/DefaultFileReader.h
    IO/DefaultFileReader.cpp
    IO/IPCFileReader.h
    IO/IPCFileReader.cpp
    IO/CSVReader.h
    IO/CSVReader.cpp
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
    IPC
    imgui)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

target_link_libraries(
    EditorCommon
    PUBLIC
    debug "${GLFW3_LIB_DEBUG}"
    optimized "${GLFW3_LIB_RELEASE}"
)

if(UNIX AND NOT APPLE)

    FIND_PACKAGE(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(
        EditorCommon
        PUBLIC
        dl
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENGL_LIBRARY}
        ${X11_LIBRARIES})

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