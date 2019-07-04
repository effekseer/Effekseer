cmake_minimum_required(VERSION 3.12)

# solution name
project(Effekseer)

add_subdirectory(Dev)


add_subdirectory(Downloads/zlib)
set_target_properties(zlib PROPERTIES FOLDER "ThirdParty/zlib")
set_target_properties(zlibstatic PROPERTIES FOLDER "ThirdParty/zlib")
set_target_properties(minigzip PROPERTIES FOLDER "ThirdParty/zlib")
set_target_properties(example PROPERTIES FOLDER "ThirdParty/zlib")

set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Downloads/zlib ${CMAKE_CURRENT_BINARY_DIR}/Downloads/zlib)
set(PNG_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(Downloads/libpng)
set_target_properties(png_static PROPERTIES FOLDER "ThirdParty/png")
set_target_properties(genfiles PROPERTIES FOLDER "ThirdParty/png")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(Downloads/glfw)
set_target_properties(glfw PROPERTIES FOLDER "ThirdParty/glfw")

