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

if(USE_LIBPNG_LOADER)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
	    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
	)
endif()
