cmake_minimum_required (VERSION 3.0.0)

project(Example_Metal)

add_executable(Example_Metal main.mm)

target_include_directories(
    Example_Metal
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererMetal
    ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src/
    ../Utils
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Effekseer
    LLGI
    EffekseerRendererMetal
    Example_Utils
    ${Vulkan_LIBRARIES}
)


find_library(COCOA_LIBRARY Cocoa)
find_library(METAL_LIBRARY Metal)
find_library(APPKIT_LIBRARY AppKit)
find_library(METALKIT_LIBRARY MetalKit)
find_library(QUARTZ_CORE_LIBRARY QuartzCore)

set(EXTRA_LIBS ${COCOA_LIBRARY} ${APPKIT_LIBRARY} ${METAL_LIBRARY} ${METALKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})

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