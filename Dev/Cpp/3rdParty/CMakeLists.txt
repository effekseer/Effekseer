if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()


if (BUILD_VIEWER OR BUILD_EDITOR)
    if (WIN32)
        set(boxer_src Boxer/boxer_win.cpp)
    elseif(APPLE)
        set(boxer_src Boxer/boxer_osx.mm)
    else()
        find_package(X11 REQUIRED)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
        set(boxer_src Boxer/boxer_linux.cpp)
    endif()

    add_library(Boxer STATIC ${boxer_src})

    target_include_directories(Boxer PUBLIC Boxer/)

    if(UNIX AND NOT APPLE)
        target_include_directories(Boxer 
            PRIVATE
            ${GTK3_INCLUDE_DIRS}
            ${X11_INCLUDE_DIR})

        target_link_directories(Boxer PRIVATE ${GTK3_LIBRARY_DIRS})
        add_definitions(${GTK3_CFLAGS_OTHER})
    endif()
endif()

