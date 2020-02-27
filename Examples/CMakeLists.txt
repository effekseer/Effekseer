cmake_minimum_required (VERSION 3.0.0)

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

add_definitions(-DEFK_EXAMPLE_ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/Resources/\")
add_definitions(-DEFK_EXAMPLE_ASSETS_DIR_U16=u\"${CMAKE_CURRENT_SOURCE_DIR}/Resources/\")

add_subdirectory(Utils)

if(WIN32)
    add_subdirectory(DirectX9)
    add_subdirectory(DirectX11)
    add_subdirectory(DirectX12)
endif()

if (BUILD_VULKAN)
    add_subdirectory(Vulkan)
endif()

