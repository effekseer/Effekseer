
enable_language(CSharp)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../release")

# specify C# framework
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION v4.6.1)

file(GLOB files_core
    EffekseerCore/*.cs
    EffekseerCore/Binary/*.cs
    EffekseerCore/Command/*.cs
    EffekseerCore/Data/*.cs
    EffekseerCore/Data/Value/*.cs
    EffekseerCore/Exporter/*.cs
    EffekseerCore/InternalScript/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Script/*.cs
    EffekseerCore/Utl/*.cs	
	)

add_library(EffekseerCore
  SHARED
  ${files_core}
)

set_target_properties(EffekseerCore PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq")

set_target_properties(EffekseerCore PROPERTIES
    VS_DOTNET_REFERENCE_IronPython ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.dll
    VS_DOTNET_REFERENCE_IronPython_Modules ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.Modules.dll
    VS_DOTNET_REFERENCE_Microsoft_Dynamic ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Dynamic.dll
    VS_DOTNET_REFERENCE_Microsoft_Scripting ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.dll
    VS_DOTNET_REFERENCE_Microsoft_Scripting_Metadata ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.Metadata.dll
	)

target_compile_definitions(EffekseerCore PRIVATE SCRIPT_ENABLED)

set_property(TARGET EffekseerCore PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_12.0.1") 

target_compile_options(EffekseerCore PRIVATE "/langversion:6")


file(GLOB files 
    Effekseer/*.cs
    Effekseer/Data/*.cs
    Effekseer/GUI/*.cs
    Effekseer/GUI/Component/*.cs
    Effekseer/GUI/Dialog/*.cs
    Effekseer/GUI/Dock/*.cs
    Effekseer/GUI/Menu/*.cs
    Effekseer/Plugin/*.cs
    Effekseer/Properties/*.cs
    Effekseer/swig/*.cs
    Effekseer/Utils/*.cs	
	) 

add_executable(Effekseer
  ${files}
)

set_target_properties(Effekseer PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp")

target_link_libraries(Effekseer EffekseerCore Viewer)
target_compile_options(Effekseer PRIVATE "/unsafe")
target_compile_options(Effekseer PRIVATE "/langversion:6")
set_property(TARGET Effekseer PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")


# specify a startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Effekseer)

