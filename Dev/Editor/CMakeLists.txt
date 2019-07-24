
enable_language(CSharp)
cmake_minimum_required(VERSION 3.8)

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

target_link_libraries(EffekseerCore
	IronPython
	IronPython.Modules
	Microsoft.Dynamic
	Microsoft.Scripting
	Microsoft.Scripting.Metadata)
target_compile_options(EffekseerCore PRIVATE "/langversion:6")


file(GLOB files 
    Effekseer/*.cs) 

add_executable(Effekseer
  ${files}
)

target_link_libraries(Effekseer EffekseerCore Viewer)
target_compile_options(Effekseer PRIVATE "/langversion:6")


# specify a startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Effekseer)

