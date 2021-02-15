
enable_language(CSharp)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../../release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../release")

# specify C# framework
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION v4.6.1)

set(EDITOR_ROOT ..)

file(GLOB files 
    ${CMAKE_CURRENT_LIST_DIR}/*.cs
    GUI/*.cs
    GUI/Dock/*.cs
    #${EDITOR_ROOT}/Effekseer/GUI/Component/*.cs
    #${EDITOR_ROOT}/Effekseer/Utils/*.cs
)



#list(APPEND files
#    ${EDITOR_ROOT}/Effekseer/GUI/RecentFiles.cs
#    ${EDITOR_ROOT}/Effekseer/GUI/Shortcuts.cs
#    ${EDITOR_ROOT}/Effekseer/GUI/Commands.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Manager.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Controls.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/ThumbnailManager.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Viewer.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Dock/DockManager.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Dock/EffectViwer.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Dock/DockPanel.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Dock/Environment.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Menu/Menu.cs
#     ${EDITOR_ROOT}/Effekseer/GUI/Menu/MenuItem.cs
#     ${EDITOR_ROOT}/Effekseer/Configs/WindowConfig.cs
# )

add_executable(EffekseerEditorLit
    ${files}
)

set_target_properties(EffekseerEditorLit PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Xml;System.Xml.Linq;System.Text.RegularExpressions;Microsoft.CSharp;System.IO.Compression")

target_link_libraries(EffekseerEditorLit PRIVATE EffekseerCore EffekseerCoreGUI)

target_compile_options(EffekseerEditorLit PRIVATE "/unsafe")
#set_property(TARGET EffekseerEditorLit PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Effekseer/icon.ico")

if (MSVC)
    set_target_properties(EffekseerEditorLit PROPERTIES OUTPUT_NAME_DEBUG "EffekseerLit" )
    set_target_properties(EffekseerEditorLit PROPERTIES OUTPUT_NAME_RELEASE "EffekseerLit" )
endif()
