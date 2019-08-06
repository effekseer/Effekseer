How to build
==========

Requirements
----------

### Windows

- git
- git-lfs [Download](https://git-lfs.github.com/)
- Visual Studio 2015 or later
- cmake (3.15 or later)

### macOS

- git
- git-lfs
- XCode or g++
- mono
- Visual Studio for Mac (msbuild)
- cmake

### Linux

- git
- git-lfs
- g++
- mono-devel
- libx11-dev libgl1-mesa-dev libxrandr-dev libxi-dev x11proto-xf86vidmode-dev xorg-dev libglu1-mesa-dev libpulse-dev libgtk-3-dev

Get code and resources
----------

Clone and update submodule.

### Terminal

```
git clone https://github.com/effekseer/Effekseer
cd Effekseer
git submodule update --init
```

### TortoiseGit (Windows)

```
Clone -> input https://github.com/effekseer/Effekseer
Submodule Update -> check init
```

Build Runtime, Viewer and Editor
----------

Execure following command at the repository root.

`Windows: Use [Visual Studio Developer Command Prompt]`

```
python build.py
```

To develop ...

- Runtime, Viewer : see `build` folder.
- Editor : see `Dev/Editor/Effekseer.sln`

If you want to build individually, see the following section.


Build Runtime and Viewer
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
- macOS: "Xcode"
- Linux: `TODO`

### options

- `-DBUILD_VIEWER=ON` : Build the library (Viewer) needed to run the editor. If omitted, only the runtime will be built.


Editor
----------

### Windows

#### VisualStudio

- `-DBUILD_VIEWER=ON -DBUILD_EDITOR=ON` : Build the editor with CMake.

#### msbuild

Execure following command at the repository root.

`Windows: Use [Visual Studio Developer Command Prompt]`

```
msbuild Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64
msbuild Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64
```

Execute following command to start the editor.

```
Dev\release\Effekseer.exe
```

### maxOS

```
msbuild Dev/Editor/EffekseerCore/EffekseerCore.csproj /t:build /p:Configuration=Release /p:Platform=x64
msbuild Dev/Editor/Effekseer/Effekseer.csproj /t:build /p:Configuration=Release /p:Platform=x64
```

Execute following command to start the editor.

```
mono Dev/release/Effekseer.exe
```

Attention
----------

- fbxToEffekseerModelConverter is downloaded from https://github.com/effekseer/Effekseer/releases/tag/Prebuild
