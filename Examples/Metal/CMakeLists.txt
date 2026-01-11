cmake_minimum_required(VERSION 3.15)

project(Example_Metal)

add_executable(Example_Metal main.mm DeviceMetal.mm DeviceMetal.h)

target_include_directories(
  Example_Metal
  PRIVATE ${EFK_THIRDPARTY_INCLUDES} ${EFK_RUNTIME_ROOT}/Effekseer
          ${EFK_RUNTIME_ROOT}/EffekseerRendererMetal
          ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src/ ../Utils)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Effekseer LLGI EffekseerRendererMetal Example_Utils
                          ${EFK_EXAMPLES_GLFW})

linkapplelibs(${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Examples")

if(USE_LIBPNG_LOADER)
  target_link_directories(${PROJECT_NAME} PRIVATE
                          ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE debug
            "${LIBPNG_LIB_DEBUG}"
            optimized
            "${LIBPNG_LIB_RELEASE}"
            debug
            "${ZLIB_LIB_DEBUG}"
            optimized
            "${ZLIB_LIB_RELEASE}")
endif()
