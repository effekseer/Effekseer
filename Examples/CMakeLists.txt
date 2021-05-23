cmake_minimum_required (VERSION 3.15)

# C++ version
if(WIN32)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

# MSVC Flag
if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

add_definitions(-DEFK_EXAMPLE_ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/Resources/\")
add_definitions(-DEFK_EXAMPLE_ASSETS_DIR_U16=u\"${CMAKE_CURRENT_SOURCE_DIR}/Resources/\")

if (GLFW3_LIB_DEBUG)
    # Build time.
    set(EFK_EXAMPLES_GLFW debug "${GLFW3_LIB_DEBUG}" optimized "${GLFW3_LIB_RELEASE}")
else()
    # Release package.
    set(EFK_EXAMPLES_GLFW glfw)
endif()

add_subdirectory(Utils)

if(WIN32)
    add_subdirectory(DirectX9)
    add_subdirectory(DirectX11)
    if (BUILD_DX12)
        add_subdirectory(DirectX12)
    endif()

    add_subdirectory(Culling)
    add_subdirectory(Distortion)
    add_subdirectory(Multithread)
    add_subdirectory(NetworkServer)
    add_subdirectory(NetworkClient)
    add_subdirectory(SortHandle)
endif()

add_subdirectory(OpenGL)

if(WIN32)
    add_subdirectory(OpenAL)
endif()

if (BUILD_VULKAN)
    add_subdirectory(Vulkan)
endif()

if(APPLE)
    add_subdirectory(Metal)
endif()

add_subdirectory(CustomLoader)

