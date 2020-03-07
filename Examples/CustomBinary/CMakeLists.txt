cmake_minimum_required (VERSION 3.0.0)
project(Example_CustomBinary)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererGL
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Effekseer
    EffekseerRendererGL
)

# for Visual C++
target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    UNICODE
    _UNICODE)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Examples") 
