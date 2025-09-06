function(LinkAppleLibs PROJECT_NAME)
  if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(APPKIT_LIBRARY AppKit)
    find_library(METALKIT_LIBRARY MetalKit)
    find_library(QUARTZ_CORE_LIBRARY QuartzCore)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

    target_link_libraries(${PROJECT_NAME} PRIVATE
      ${COCOA_LIBRARY}
      ${APPKIT_LIBRARY}
      ${METAL_LIBRARY}
      ${METALKIT_LIBRARY}
      ${QUARTZ_CORE_LIBRARY}
      ${IOKIT_FRAMEWORK}
      ${OPENGL_LIBRARIES}
      ${CORE_FOUNDATION_FRAMEWORK}
      ${CORE_VIDEO_FRAMEWORK})
  endif()
endfunction()
