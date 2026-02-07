
# C++

> NOTE: See also ARCHITECTURE.md

## Development Docs

- Release procedure: [ReleaseGuide.md](./ReleaseGuide.md)
- Effekseer-specific imgui changes: [ImGuiCustomization.md](./ImGuiCustomization.md)

## Effekseer

Core module without a graphics device.

```
Dev/Cpp/Effekseer
```

## EffekseerRendererCommon

Common module between graphic devices.
This module render particles with graphic devices.

```
Dev/Cpp/EffekseerRendererCommon
```

## EffekseerRendererDX9,DX11,GL

Renderers for DirectX9, DirectX11, OpenGL

```
Dev/Cpp/EffekseerRendererDX9
Dev/Cpp/EffekseerRendererDX11
Dev/Cpp/EffekseerRendererGL
```

## EffekseerRendererLLGI

Common module of renderers for DirectX12, Vulkan and metal (planned)
This module renders particles with LLGI(in 3rdParty)

```
Dev/Cpp/EffekseerRendererLLGI
```

## EffekseerSoundAL,DSound,XAudio2,OSMixer

Sound module for OpenAL, DirectSound, XAudio, OpenSoundMixter(in Downloads)

```
Dev/Cpp/EffekseerSoundAL
Dev/Cpp/EffekseerSoundDSound
Dev/Cpp/EffekseerSoundOSMixer
Dev/Cpp/EffekseerSoundXAudio2
```

## Viewer

A dll for the editor

```
Dev/Cpp/Viewer
```

## MaterialEditor

A material editor

- Model

```
Dev/Cpp/EffekseerMaterial
```

- View

```
Dev/Cpp/EffekseerMaterialEditor
```

## MaterialCompiler

Material compilers for editor and runtime

```
Dev/Cpp/EffekseerMaterialCompiler
```


# C#

## Core

The editor without GUI

```
Dev/Editor/EffekseerCore
```

## Editor

The editor

```
Dev/Editor/Effekseer
```

## EffekseerOld

Not used. It will be removed.

```
Dev/Editor/EffekseerOld
```
