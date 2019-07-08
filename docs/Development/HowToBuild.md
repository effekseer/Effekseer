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
- MacOS: `TODO`
- Linux: `TODO`

### options

- `BUILD_VIEWER=ON` : Build the library (Viewer) needed to run the editor. If omitted, only the runtime will be built.


Attention
----------

- fbxToEffekseerModelConverter is downloaded from https://github.com/effekseer/Effekseer/releases/tag/Prebuild
