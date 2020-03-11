cmake_minimum_required(VERSION 3.1)
project(Effekseer)

option(BUILD_EXAMPLES "Build examples" ON)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

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

# a function to generate filters by folders
function(FilterFolder srcs)
    foreach(FILE ${srcs}) 
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    
        # Remove common directory prefix to make the group
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    
        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")
    
        # Group into "Source Files" and "Header Files"
        if ("${FILE}" MATCHES ".*\\.cpp")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.c")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.cxx")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.h")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.mm")
           set(GROUP "${GROUP}")
        endif()
    
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction(FilterFolder)

set(EFK_RUNTIME_ROOT ${CMAKE_CURRENT_LIST_DIR}/src)

add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples/Utils/glfw)
    add_subdirectory(Examples)
endif()
