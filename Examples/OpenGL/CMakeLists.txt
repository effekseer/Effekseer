cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_OpenGL main.cpp)

target_include_directories(
    Example_OpenGL
    PRIVATE
    ../../Dev/Cpp/Effekseer
    ../../Dev/Cpp/EffekseerRendererGL
    ../Utils
	${EFK_THIRDPARTY_INCLUDES}
)

target_link_libraries(
    Example_OpenGL
    PRIVATE
    Effekseer
    EffekseerRendererGL
    Example_Utils
    debug "${GLFW3_LIB_DEBUG}"
    optimized "${GLFW3_LIB_RELEASE}"
)

target_link_directories(
    Example_OpenGL
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)