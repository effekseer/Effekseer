# Based on https://qiita.com/tenmyo/items/f8548ee9bab78f18cd25

option(CLANG_FORMAT_ENABLED "Specifies whether clang-format is automatically applied." OFF)
find_program(CLANG_FORMAT_EXE clang-format)

function(clang_format target)
  if(CLANG_FORMAT_EXE AND CLANG_FORMAT_ENABLED)
    message(STATUS "Enable Clang-Format ${target}")
    get_target_property(MY_SOURCES ${target} SOURCES)
    add_custom_target(
      "${target}_format-with-clang-format"
      COMMAND "${CLANG_FORMAT_EXE}" -i -style=file ${MY_SOURCES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
    add_dependencies(${target} "${target}_format-with-clang-format")
  endif()
endfunction()