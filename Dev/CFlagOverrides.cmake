if(SANITIZE_ENABLED)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address")
  else()
    add_compile_options(-fsanitize=undefined,address)
    add_link_options(-fsanitize=undefined,address)
  endif()
endif()

if(MSVC)
  if(DEFINED EFK_USE_MSVC_RUNTIME_LIBRARY_DLL
     AND EFK_USE_MSVC_RUNTIME_LIBRARY_DLL)
    set(_EFK_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  else()
    set(_EFK_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()

  if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
  endif()

  set(CMAKE_MSVC_RUNTIME_LIBRARY
      "${_EFK_MSVC_RUNTIME_LIBRARY}"
      CACHE STRING "MSVC runtime library" FORCE)
endif()

if(MSVC AND NOT EFK_USE_MSVC_RUNTIME_LIBRARY_DLL)
  foreach(
    flag
    CMAKE_C_FLAGS_DEBUG_INIT
    CMAKE_C_FLAGS_MINSIZEREL_INIT
    CMAKE_C_FLAGS_RELEASE_INIT
    CMAKE_C_FLAGS_RELWITHDEBINFO_INIT
    CMAKE_CXX_FLAGS_DEBUG_INIT
    CMAKE_CXX_FLAGS_MINSIZEREL_INIT
    CMAKE_CXX_FLAGS_RELEASE_INIT
    CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT)
    if(${flag} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
    endif()
    if(${flag} MATCHES "/MDd")
      string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
    endif()
  endforeach()
  foreach(flag CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
               CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    if(${flag} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
    endif()
    if(${flag} MATCHES "/MDd")
      string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
    endif()
  endforeach()
endif()
