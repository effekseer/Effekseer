
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

add_executable(ConvertingCodeGenerator
    Program.cs
)

set_target_properties(ConvertingCodeGenerator PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net6.0")

set_target_properties(ConvertingCodeGenerator PROPERTIES VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.IO.Compression;Microsoft.CSharp;System.IO.Compression.FileSystem")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")
set_property(TARGET EffekseerCoreGUI PROPERTY VS_GLOBAL_GenerateDependencyFile "false")

target_link_libraries(ConvertingCodeGenerator PRIVATE EffekseerCore)
