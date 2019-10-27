
option(BUILD_VIEWER "Build viewer" OFF)
option(BUILD_EDITOR "Build editor" OFF)

add_subdirectory(Cpp)

if(BUILD_EDITOR AND NOT FROM_CI)
    add_subdirectory(Editor)
endif()