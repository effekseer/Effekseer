
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION v4.6.1)

add_library(PartsTreeSystem
  SHARED
  PartsTreeSystem/PartsTreeSystem/AccessKey.cs
  PartsTreeSystem/PartsTreeSystem/Asset.cs
  PartsTreeSystem/PartsTreeSystem/CommandManager.cs
  PartsTreeSystem/PartsTreeSystem/Consts.cs
  PartsTreeSystem/PartsTreeSystem/Difference.cs
  PartsTreeSystem/PartsTreeSystem/Environment.cs
  PartsTreeSystem/PartsTreeSystem/FieldState.cs
  PartsTreeSystem/PartsTreeSystem/JsonSerializer.cs
  PartsTreeSystem/PartsTreeSystem/Node.cs
  PartsTreeSystem/PartsTreeSystem/NodeTreeGroup.cs
  PartsTreeSystem/PartsTreeSystem/NodeTreeGroupEditorProperty.cs
  PartsTreeSystem/PartsTreeSystem/SerializeField.cs
  PartsTreeSystem/PartsTreeSystem/Utility.cs
)

set_property(TARGET PartsTreeSystem PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_12.0.1")

set_target_properties(PartsTreeSystem PROPERTIES
  VS_DOTNET_REFERENCES "System;System.Xml;System.Runtime")

set_target_properties (PartsTreeSystem PROPERTIES FOLDER Thirdparty)
