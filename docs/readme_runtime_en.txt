# Effekseer Runtime

## Overview

Thank you for downloading Effekseer.
This package contains a runtime for showing visual effects on applications using DirectX or OpenGL.
The package does not contain tools or samples to create visual effects.
If you want these, please download a package of tools.

## How to use

### Install CMake

You need to install CMake. CMake is a tool to generate a project for VisualStudio, Xcode and so on.
Please add a path where CMake application exists into an environment variable.

https://cmake.org/

### When integrate an application which is compiled with Cmake

You can integrate Effekseer as a subdirectory

### When integrate an application which is not compiled with Cmake

#### Generate a project

Execute bat of sh which depends on your environment.

#### Compile

Open a generated project and compile it.

If you use Visual Studio, library files(.lib) are generated on xxx.
If you use XCode, archive files(.a) are generated on xxx.

You can use Effekseer by including header files and linking .lib or .a files.

## Examples

Examples are compiled along with libs.

Examples are containted in Examples directory.

OldExampls are old examples.

Its are left because some examples are not migrated into new examples. These cannot be compiled. 

### CustomLoader

A sample to customize how to load files.

### DirectX9

A sample to use Effekseer with DirectX9 on your application.

### DirectX11

A sample to use Effekseer with DirectX11 on your application.

### DirectX12

A sample to use Effekseer with DirectX12 on your application.

### OpenGL

A sample to use Effekseer with OpenGL on your application.

### OpenAL

A sample to use Effekseer with OpenAL on your application.
