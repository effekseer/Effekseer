project(EffekseerResourceConverter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE LOCAL_HEADERS source/*.h)
file(GLOB_RECURSE LOCAL_SOURCES source/*.cpp)

file(GLOB_RECURSE EXTERNAL_HEADERS external/*.h external/*.hpp)
file(GLOB_RECURSE EXTERNAL_SOURCES external/*.c external/*.cpp)

set(LOCAL_FILES
    ${LOCAL_HEADERS} ${LOCAL_SOURCES}
    ${EXTERNAL_HEADERS} ${EXTERNAL_SOURCES})

foreach(FILE ${LOCAL_FILES}) 
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    set(GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_executable(${PROJECT_NAME} ${LOCAL_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE external/ufbx external/tinygltf external/mqoio external/nlohmann)

set_target_properties( EffekseerResourceConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Dev/release/tools )
set_target_properties( EffekseerResourceConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR}/Dev/release/tools )
set_target_properties( EffekseerResourceConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/Dev/release/tools )
