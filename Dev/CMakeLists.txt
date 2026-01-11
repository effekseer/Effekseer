add_subdirectory(Cpp)

if(BUILD_EDITOR AND NOT FROM_CI)
  add_subdirectory(Editor)
endif()
