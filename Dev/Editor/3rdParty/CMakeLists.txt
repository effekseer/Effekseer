
enable_language(CSharp)
cmake_minimum_required(VERSION 3.23)

add_library(FlatBuffers
  SHARED
  FlatBuffers/ByteBuffer.cs
  FlatBuffers/ByteBufferUtil.cs
  FlatBuffers/FlatBufferBuilder.cs
  FlatBuffers/FlatBufferConstants.cs
  FlatBuffers/IFlatbufferObject.cs
  FlatBuffers/Offset.cs
  FlatBuffers/Struct.cs
  FlatBuffers/Table.cs
)

set_target_properties(FlatBuffers PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net5.0")

set_property(TARGET FlatBuffers PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET FlatBuffers PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")

set_target_properties (FlatBuffers PROPERTIES FOLDER Thirdparty)


add_library(PartsTreeSystem
  SHARED
  PartsTreeSystem/PartsTreeSystem/AccessKey.cs
  PartsTreeSystem/PartsTreeSystem/Asset.cs
  PartsTreeSystem/PartsTreeSystem/CommandManager.cs
  PartsTreeSystem/PartsTreeSystem/Consts.cs
  PartsTreeSystem/PartsTreeSystem/Difference.cs
  PartsTreeSystem/PartsTreeSystem/EditorUtility.cs
  PartsTreeSystem/PartsTreeSystem/Environment.cs
  PartsTreeSystem/PartsTreeSystem/FieldState.cs
  PartsTreeSystem/PartsTreeSystem/Instance.cs
  PartsTreeSystem/PartsTreeSystem/JsonSerializer.cs
  PartsTreeSystem/PartsTreeSystem/Node.cs
  PartsTreeSystem/PartsTreeSystem/NodeTree.cs
  PartsTreeSystem/PartsTreeSystem/NodeTreeAsset.cs
  PartsTreeSystem/PartsTreeSystem/NodeTreeAssetEditorProperty.cs
  PartsTreeSystem/PartsTreeSystem/SerializeField.cs
  PartsTreeSystem/PartsTreeSystem/Utility.cs
)


set_target_properties(PartsTreeSystem PROPERTIES
    DOTNET_SDK "Microsoft.NET.Sdk"
    DOTNET_TARGET_FRAMEWORK "net5.0")

set_property(TARGET PartsTreeSystem PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_13.0.1")

set_target_properties(PartsTreeSystem PROPERTIES
  VS_DOTNET_REFERENCES "System;System.Xml;System.Runtime")

set_property(TARGET PartsTreeSystem PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")
set_property(TARGET PartsTreeSystem PROPERTY VS_GLOBAL_DefineConstants "SCRIPT_ENABLED;")
set_property(TARGET PartsTreeSystem PROPERTY VS_GLOBAL_AppendTargetFrameworkToOutputPath "false")

set_target_properties (PartsTreeSystem PROPERTIES FOLDER Thirdparty)
