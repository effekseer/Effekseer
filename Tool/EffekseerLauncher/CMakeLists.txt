cmake_minimum_required(VERSION 3.15)

if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      "12.0"
      CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

project(EffekseerLauncher C CXX)

set(CMAKE_CXX_STANDARD 14)

set(EFFEKSEER_VERSION
    ""
    CACHE STRING "Effekseer version embedded in the launcher")
if(EFFEKSEER_VERSION STREQUAL "")
  set(EFFEKSEER_CORE_SOURCE
      "${CMAKE_CURRENT_SOURCE_DIR}/../../Dev/Editor/EffekseerCore/Core.cs")
  if(EXISTS "${EFFEKSEER_CORE_SOURCE}")
    file(READ "${EFFEKSEER_CORE_SOURCE}" EFFEKSEER_CORE_CONTENTS)
    string(
      REGEX MATCH
            "public const string Version = \"([0-9]+)\\.([0-9]+)\\.([0-9]+)\""
            EFFEKSEER_VERSION_MATCH "${EFFEKSEER_CORE_CONTENTS}")
    if(EFFEKSEER_VERSION_MATCH)
      set(EFFEKSEER_VERSION
          "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
    endif()
  endif()
endif()

if(EFFEKSEER_VERSION STREQUAL "")
  set(EFFEKSEER_VERSION "0.0.0")
endif()

string(REPLACE "." ";" EFFEKSEER_VERSION_COMPONENTS "${EFFEKSEER_VERSION}")
list(LENGTH EFFEKSEER_VERSION_COMPONENTS EFFEKSEER_VERSION_COMPONENT_COUNT)
if(NOT EFFEKSEER_VERSION_COMPONENT_COUNT EQUAL 3)
  message(FATAL_ERROR "EFFEKSEER_VERSION must have the form major.minor.patch")
endif()
list(GET EFFEKSEER_VERSION_COMPONENTS 0 EFFEKSEER_VERSION_MAJOR)
list(GET EFFEKSEER_VERSION_COMPONENTS 1 EFFEKSEER_VERSION_MINOR)
list(GET EFFEKSEER_VERSION_COMPONENTS 2 EFFEKSEER_VERSION_PATCH)

if(APPLE)
  add_executable(EffekseerLauncher src/main.cpp)
elseif(WIN32)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
      "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
      "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")

  configure_file(resources.rc.in resources.rc @ONLY)
  add_executable(EffekseerLauncher src/main.cpp
                                   "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")

  # Preserve the launcher's file reputation when its source and toolchain have
  # not changed. Without /Brepro, LINK writes the current time into two PE
  # fields, producing a different hash on every CI build.
  if(MSVC)
    target_link_options(EffekseerLauncher PRIVATE /Brepro)
  endif()
elseif(UNIX)
  add_executable(EffekseerLauncher src/main.cpp)

  # Keep source/build paths and wall-clock time out of Linux artifacts, including
  # debug information. A content-derived build ID remains useful to debuggers.
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      EffekseerLauncher PRIVATE
      "-ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=/usr/src/effekseer-launcher"
      "-ffile-prefix-map=${CMAKE_CURRENT_BINARY_DIR}=/usr/src/effekseer-launcher/build"
      -Werror=date-time)
    target_link_options(EffekseerLauncher PRIVATE "-Wl,--build-id=sha1")
  endif()
endif()
