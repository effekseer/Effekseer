
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
    EffekseerCore/IO/*.cs	
    EffekseerCore/Utils/*.cs
	)

list(APPEND files_core
    EffekseerCore/IO/EfkEfc.cs
    EffekseerCore/Utils/ResourceCache.cs
)

add_library(EffekseerCore
  SHARED
  ${files_core}
)

set_target_properties(EffekseerCore PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq")

#set_target_properties(EffekseerCore PROPERTIES
#    VS_DOTNET_REFERENCE_IronPython ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.dll
#    VS_DOTNET_REFERENCE_IronPython_Modules ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/IronPython.Modules.dll
#    VS_DOTNET_REFERENCE_Microsoft_Dynamic ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Dynamic.dll
#    VS_DOTNET_REFERENCE_Microsoft_Scripting ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.dll
#    VS_DOTNET_REFERENCE_Microsoft_Scripting_Metadata ${CMAKE_CURRENT_SOURCE_DIR}/EffekseerCore/Microsoft.Scripting.Metadata.dll
#	)

set_property(TARGET EffekseerCore PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_12.0.1;DynamicLanguageRuntime_1.2.2;IronPython_2.7.9") 

target_compile_options(EffekseerCore PRIVATE "/unsafe")
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
	Effekseer/Process/*.cs
	)

list(APPEND files
    Effekseer/swig/IO.cs
    Effekseer/swig/IOCallback.cs
    Effekseer/swig/SystemLanguage.cs
    Effekseer/swig/MainWindow.cs
    Effekseer/swig/MainWindowState.cs
    Effekseer/GUI/Dock/Environment.cs
    Effekseer/Configs/WindowConfig.cs
    Effekseer/GUI/ThumbnailManager.cs
)

add_executable(EffekseerEditor
    ${files}
)

set_target_properties(EffekseerEditor PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

add_dependencies(EffekseerEditor EffekseerCore Viewer)
target_link_libraries(EffekseerEditor EffekseerCore Viewer)

target_compile_options(EffekseerEditor PRIVATE "/unsafe")
target_compile_options(EffekseerEditor PRIVATE "/langversion:6")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")

target_compile_definitions(EffekseerCore PRIVATE SCRIPT_ENABLED)

# specify a startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT EffekseerEditor)

if (MSVC)
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_DEBUG "Effekseer" )
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_RELEASE "Effekseer" )
endif()