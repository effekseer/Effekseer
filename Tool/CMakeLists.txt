cmake_policy(SET CMP0091 NEW)

project(Tools)

cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Release")
endif()

add_subdirectory(fbxToEffekseerModelConverter)
add_subdirectory(fbxToEffekseerCurveConverter)

if(WIN32)

  target_include_directories(fbxToEffekseerModelConverter
                             PRIVATE fbxsdk/windows/include/)
  target_link_directories(fbxToEffekseerModelConverter PRIVATE
                          fbxsdk/windows/lib/vs2017/x86/)

  target_include_directories(fbxToEffekseerCurveConverter
                             PRIVATE fbxsdk/windows/include/)
  target_link_directories(fbxToEffekseerCurveConverter PRIVATE
                          fbxsdk/windows/lib/vs2017/x86/)

elseif(APPLE)

  target_include_directories(fbxToEffekseerModelConverter
                             PRIVATE fbxsdk/mac/include/)
  target_link_directories(fbxToEffekseerModelConverter PRIVATE fbxsdk/mac/lib/clang/release/)
  target_link_libraries(fbxToEffekseerModelConverter PRIVATE fbxsdk)

  target_include_directories(fbxToEffekseerCurveConverter
                             PRIVATE fbxsdk/mac/include/)
  target_link_directories(fbxToEffekseerCurveConverter PRIVATE fbxsdk/mac/lib/clang/release/)
  target_link_libraries(fbxToEffekseerCurveConverter PRIVATE fbxsdk)

  set_target_properties(fbxToEffekseerModelConverter PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
  set_target_properties(fbxToEffekseerCurveConverter PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)

else()

  target_include_directories(fbxToEffekseerModelConverter
                             PRIVATE fbxsdk/linux/include/)
  target_link_directories(fbxToEffekseerModelConverter PRIVATE
                          fbxsdk/linux/lib/gcc/x64/release/)
  target_link_libraries(fbxToEffekseerModelConverter PRIVATE fbxsdk dl ltdl lzma pthread xml2 z)

  target_include_directories(fbxToEffekseerCurveConverter
                             PRIVATE fbxsdk/linux/include/)
  target_link_directories(fbxToEffekseerCurveConverter PRIVATE
                          fbxsdk/linux/lib/gcc/x64/release/)
  target_link_libraries(fbxToEffekseerCurveConverter PRIVATE fbxsdk dl ltdl lzma pthread xml2 z)

endif()

if(WIN32)
  add_custom_target(fbxdll)
  add_custom_command(
    TARGET fbxdll
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/Debug
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/Release
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/fbxsdk/windows/lib/vs2017/x86/debug/libfbxsdk.dll
      ${CMAKE_SOURCE_DIR}/Debug
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/fbxsdk/windows/lib/vs2017/x86/release/libfbxsdk.dll
      ${CMAKE_SOURCE_DIR}/Release)

  add_dependencies(fbxToEffekseerCurveConverter fbxdll)
  add_dependencies(fbxToEffekseerModelConverter fbxdll)
endif()
