
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../../release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../release")

# specify C# framework
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION v4.6.1)

file(GLOB files 
    ${CMAKE_CURRENT_LIST_DIR}/*.cs
    GUI/*.cs
    GUI/Dock/*.cs
)

add_executable(EffekseerEditorLit
    ${files}
)

set_target_properties(EffekseerEditorLit PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

add_dependencies(EffekseerEditorLit EffekseerCore Viewer EffekseerCoreGUI)
target_link_libraries(EffekseerEditorLit PRIVATE EffekseerCore Viewer EffekseerCoreGUI)

target_compile_options(EffekseerEditorLit PRIVATE "/unsafe")
#set_property(TARGET EffekseerEditorLit PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")

if (MSVC)
    set_target_properties(EffekseerEditorLit PROPERTIES OUTPUT_NAME_DEBUG "EffekseerLit" )
    set_target_properties(EffekseerEditorLit PROPERTIES OUTPUT_NAME_RELEASE "EffekseerLit" )
endif()
