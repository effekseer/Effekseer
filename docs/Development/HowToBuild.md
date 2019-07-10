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
```

### generator

`<generator>` is tested with the following values: 
- Windows: "Visual Studio 15 2017"
- MacOS: `TODO`cmake .. -G "Xcode" -DBUILD_VIEWER=ON
- Linux: `TODO`cmake .. -G "Unix Makefiles" -DBUILD_VIEWER=ON CMAKE_BUILD_TYPE=Release
-D"CMAKE_OSX_ARCHITECTURES=x86_64"

msbuild Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x86
msbuild Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x86

mono Effekseer.exe

### options

- `BUILD_VIEWER=ON` : Build the library (Viewer) needed to run the editor. If omitted, only the runtime will be built.


Attention
----------

- fbxToEffekseerModelConverter is downloaded from https://github.com/effekseer/Effekseer/releases/tag/Prebuild
