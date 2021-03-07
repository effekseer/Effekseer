
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

file(GLOB files
    *.cs
    swig/*.cs
    Configs/*.cs
    GUI/*.cs
    GUI/Component/*.cs
    GUI/Dialog/*.cs
    GUI/Dock/*.cs
    GUI/Menu/*.cs
    IO/*.cs
    Plugin/*.cs
    Process/*.cs
    Utils/*.cs
    IO/mqoToEffekseerModelConverter/*.cs
    IO/mqoToEffekseerModelConverter/mqoIO/*.cs
    IO/mqoToEffekseerModelConverter/mqoIO/Realtime/*.cs
)

add_library(EffekseerCoreGUI
  SHARED
  ${files}
)

set_target_properties(EffekseerCoreGUI PROPERTIES VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.IO.Compression;Microsoft.CSharp;System.IO.Compression.FileSystem")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_12.0.1;DynamicLanguageRuntime_1.2.2") 
target_compile_options(EffekseerCoreGUI PRIVATE "/unsafe")
target_link_libraries(EffekseerCoreGUI PRIVATE EffekseerCore Viewer)
