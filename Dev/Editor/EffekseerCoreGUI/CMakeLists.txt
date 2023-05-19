
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

file(GLOB files
    *.cs
    swig/*.cs
    Configs/*.cs
    GUI/*.cs
    GUI/BindableComponent/*.cs
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
    GUI/Inspector/Inspector.cs
    GUI/Inspector/InspectorGUI.cs
    GUI/Widgets/Color.cs
    GUI/Widgets/Gradient.cs
    GUI/Widgets/Texture.cs
    GUI/Widgets/WidgetHelper.cs
    GUI/Widgets/Widgets.cs
    GUI/Widgets/DynamicSelector.cs
)

set_target_properties(EffekseerCoreGUI PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net6.0")

set_target_properties(EffekseerCoreGUI PROPERTIES VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.IO.Compression;Microsoft.CSharp;System.IO.Compression.FileSystem")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_13.0.1;DynamicLanguageRuntime_1.2.2")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_GenerateDependencyFile "false")

# Avoid Cmake bug
# set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_Platforms "x64")

target_link_libraries(EffekseerCoreGUI PRIVATE EffekseerCore Viewer)
