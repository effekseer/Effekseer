How to build
==========

Requirements
----------

### Windows

- Visual Studio 2015+

### macOS

- Visual Studio for Mac (msbuild)

### Linux

`TODO`


Runtime and Viewer
----------

Execure following command at the repository root.

```
mkdir build
cd build
cmake .. -G <generator> [options]
cmake --build . --config Release
```

### generator

`<generator>` is tested with the following values: 
- Windows: "Visual Studio 15 2017"
- macOS: `TODO`cmake .. -G "Xcode" -DBUILD_VIEWER=ON
- Linux: `TODO`cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON CMAKE_BUILD_TYPE=Release
-D"CMAKE_OSX_ARCHITECTURES=x86_64"

### options

- `-DBUILD_VIEWER=ON` : Build the library (Viewer) needed to run the editor. If omitted, only the runtime will be built.


Editor
----------

Execure following command at the repository root.

`Windows: Use [Visual Studio Developer Command Prompt]`

```
msbuild Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86
msbuild Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86
```

Execute following command to start editor.

Windows:
```
Dev\release\Effekseer.exe
```

macOS:
```
mono Effekseer.exe
```
s

Attention
----------

- fbxToEffekseerModelConverter is downloaded from https://github.com/effekseer/Effekseer/releases/tag/Prebuild
