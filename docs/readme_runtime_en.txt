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

Execute bat or sh which depends on your environment.

An install folder is created, and include files and library files are created in it.

For example, for Visual Studio 2019, the batch file is build_msvc2019.bat and the install folder is install_msvc2019.

You can use Effekseer by linking them.

## Examples

Examples are compiled along with libs.

Examples are containted in Examples directory.

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

### Others

There are various other samples as well.