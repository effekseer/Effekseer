
# Tool

## FBXSDK

Download here

https://aps.autodesk.com/developer/overview/fbx-sdk

Plese put fbxsdk on 

```
fbxsdk/{os}/include/
fbxsdk/{os}/lib/

```

Current Effekseer uses FBX SDK 2020.2.1.0

## Build

### Windows

```
 cmake -B build -S . -G "Visual Studio 16 2019" -A Win32
```