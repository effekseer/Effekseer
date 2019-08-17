# The project's name
project(EffekseerMaterial)

# Check cmake version
cmake_minimum_required(VERSION 3.0)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

# For Mac
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

if (MSVC)

elseif (MINGW)
    set(CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})
else()
    set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC" ${CMAKE_CXX_FLAGS})
endif()

file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE hs *.h)

# set include directories
include_directories(../include/)

# Add compile targets
add_library( EffekseerMaterial STATIC ${hs} ${srcs} )

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

set(files ${srcs} ${hs})

foreach(FILE files) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.c")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.cxx")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.mm")
        set(GROUP "Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()