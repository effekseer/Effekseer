
enable_language(CSharp)
cmake_minimum_required(VERSION 3.23)

add_subdirectory(3rdParty)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../release")

file(GLOB files_core
    EffekseerCore/*.cs
    EffekseerCore/Binary/*.cs
    EffekseerCore/Binary/RenderData/*.cs
    EffekseerCore/Command/*.cs
    EffekseerCore/Data/*.cs
    EffekseerCore/Data/Group/*.cs
    EffekseerCore/Data/Value/*.cs
    EffekseerCore/Exporter/*.cs
    EffekseerCore/InternalScript/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Properties/*.cs
    EffekseerCore/Script/*.cs
    EffekseerCore/IO/*.cs	
    EffekseerCore/Utils/*.cs
    EffekseerCore/Data/ProceduralModelValues.cs
    EffekseerCore/Data/Value/Int2.cs
    EffekseerCore/Data/Value/ObjectCollection.cs
    EffekseerCore/Localization/Localization.cs
    EffekseerCore/EffectAsset/EffectAsset.cs
    EffekseerCore/Math/Vector2I.cs
    EffekseerCore/Math/Vector3I.cs
    EffekseerCore/Math/Vector2F.cs
    EffekseerCore/Math/Vector3F.cs
)

list(APPEND files_core
    EffekseerCore/IO/EfkEfc.cs
    EffekseerCore/Utils/ResourceCache.cs
    EffekseerCore/Utils/CsvReader.cs
    EffekseerCore/Utils/ProjectVersionUpdater.cs
)

add_library(EffekseerCore
  SHARED
  ${files_core}
)

set_target_properties(EffekseerCore PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net6.0")

set_property(TARGET EffekseerCore PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET EffekseerCore PROPERTY VS_GLOBAL_DefineConstants "SCRIPT_ENABLED;")
set_property(TARGET EffekseerCore PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")
set_property(TARGET EffekseerCore PROPERTY VS_GLOBAL_GenerateDependencyFile "false")

target_link_libraries(EffekseerCore PUBLIC PartsTreeSystem)

# Avoid Cmake bug
# set_property(TARGET EffekseerCore PROPERTY VS_GLOBAL_Platforms "x64")

set_target_properties(EffekseerCore PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.IO.Compression;System.IO.Compression.FileSystem")

set_property(TARGET EffekseerCore PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_13.0.1;DynamicLanguageRuntime_1.2.2;IronPython_2.7.9") 

set_target_properties(EffekseerCore PROPERTIES FOLDER Editor)
target_link_libraries(EffekseerCore PUBLIC PartsTreeSystem)


add_subdirectory(EffekseerCoreGUI)
set_target_properties (EffekseerCoreGUI PROPERTIES FOLDER Editor)



file(GLOB files 
    Effekseer/*.cs
    Effekseer/Data/*.cs
    Effekseer/GUI/*.cs
    Effekseer/GUI/Dock/*.cs
    Effekseer/GUI/Menu/*.cs
    Effekseer/Properties/*.cs
    Effekseer/Utils/*.cs
    Effekseer/GUI/Dock/ProceduralModel.cs
)

list(APPEND files
    Effekseer/GUI/Dock/Environment.cs
)

add_executable(EffekseerEditor
    ${files}
)

set_target_properties(EffekseerEditor PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net6.0")

set_target_properties(EffekseerEditor PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

add_dependencies(EffekseerEditor EffekseerCore Viewer EffekseerCoreGUI)
target_link_libraries(EffekseerEditor PRIVATE EffekseerCore Viewer EffekseerCoreGUI)

set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_OutputType "WinExe")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")
set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_GenerateDependencyFile "false")

# Avoid Cmake bug
# set_property(TARGET EffekseerEditor PROPERTY VS_GLOBAL_Platforms "x64")

# specify a startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT EffekseerEditor)

if (MSVC)
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_DEBUG "Effekseer" )
    set_target_properties(EffekseerEditor PROPERTIES OUTPUT_NAME_RELEASE "Effekseer" )
endif()

set_target_properties (EffekseerEditor PROPERTIES FOLDER Editor)

if (BUILD_TEST)
    file(GLOB files_test
        TestCSharp/*.cs
        TestCSharp/Misc/*.cs
        TestCSharp/Tests/*.cs
    )
    add_executable(TestCSharp ${files_test})

    set_target_properties(TestCSharp PROPERTIES
        DOTNET_SDK "Microsoft.NET.Sdk"
        DOTNET_TARGET_FRAMEWORK "net6.0")
    
    set_target_properties(TestCSharp PROPERTIES VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

    set_property(TARGET TestCSharp PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
    set_property(TARGET TestCSharp PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")
    set_property(TARGET TestCSharp PROPERTY VS_GLOBAL_GenerateDependencyFile "false")

    # Avoid Cmake bug
    # set_property(TARGET TestCSharp PROPERTY VS_GLOBAL_Platforms "x64")

    add_dependencies(TestCSharp EffekseerCore)
    target_link_libraries(TestCSharp PRIVATE EffekseerCore)

    if (MSVC)
        set_target_properties(TestCSharp PROPERTIES OUTPUT_NAME_DEBUG "Effekseer.TestCSharp" )
        set_target_properties(TestCSharp PROPERTIES OUTPUT_NAME_RELEASE "Effekseer.TestCSharp" )
    endif()
    set_target_properties (TestCSharp PROPERTIES FOLDER Tests)
endif()

add_subdirectory(EffekseerLit)
set_target_properties (EffekseerEditorLit PROPERTIES FOLDER Editor)
