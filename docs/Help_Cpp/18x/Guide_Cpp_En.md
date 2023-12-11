# Effekseer For C++ Introduction Guide (DirectX 11/12)

A guide to incorporating and playing Effekseer For C++ in Windows games/applications using the C++ language and DirectX 11/12.

This guide is intended for the following environments.
Please read the differences due to differences in environment accordingly.

- Windows 10/11
- Microsoft Visual Studio 2019/2022
- DirectX 11/12
- x86/x64
- Effekseer for C++ 1.8

> [!NOTE]
> **If you are unfamiliar with library installation, we recommend backing up your project beforehand. **
> If you make a mistake when installing the library, it may cause anomalies in the project settings of your application.

> [!NOTE]
> In this document, the game/application to be installed is referred to as the "application".


- [1. Check the environment of the application to be installed](#1-Check the environment of the application to be installed)
  - [1.1. how to check platform and runtime library settings](#11-How to check platform and runtime library settings)
- [2. install CMake](#2-cmake installation)
- [3. Build Effekseer For C++](#3-build-effekseer-for-c)
  - [3.1. download](#31-download)
  - [3.2. Build](#32-Build)
- [4. Deployment of built library files](#4-Deployment of built library files)
  - [4.1. header file (.h) placement](#41-Placement of header file h)
  - [4.2. library file (.lib) placement](#42-Library file lib placement)
  - [4.3. check file placement again](#43-Check file placement again) [5.
- VisualStudio project settings for the application to be installed](#5-Visualstudio project settings for the application to be installed) [5.1.
  - [5.1. include directory settings](#51-Include Directory Settings)
  - [5.2. library directory settings](#52-Library directory settings)
  - [5.3. apply](#53-Apply)
- Playing back effects in the application](#6-Play back effects in the application) [6.1.
  - [6.1. prepare effect file to play](#61-Prepare effect file to play)
  - [6.2. include and pragma statement](#62-include and pragma statement)
  - [6.3. create effect manager](#63-create effect manager)
  - [6.4. Create drawing module](#64-Create drawing module)
  - [6.5. create drawing module settings](#65-Create drawing module settings)
  - [6.6. Coordinate System Settings](#66-Coordinate System Settings)
  - [6.7. Create projection and camera matrices](#67-Create projection and camera matrices)
  - [6.8. Load Effects](#68 - Load Effects)
  - [6.9. Playback and Control of Effects](#69-Playback and Control of Effects)
  - [6.10. Processing at the beginning of every frame execution](#610-Processing at the beginning of every frame execution for directX12 only)
  - [6.11. Update process of \[毎回フレーム実行\]Manager](#611-update process of "毎フレース実行\]Manager)
  - [6.12.\[毎回フレーム実行\]Renderer update and drawing process](#612-update renderer for every frame execution and drawing process)
  - [6.13. \[DirectX12 only, every frame execution\]Command list exit processing](#613-Every frame execution command list exit processing for directx12 only)
  - [6.14. release process](#614-release process only for directx12)
  - [6.15. check behavior](#615-check behavior)
- [7. align camera and coordinate system with application](#7-Align camera and coordinate system with application)
  - [7.1. synchronize the camera](#71-Synchronize the camera)
  - [7.2. align coordinate system with application](#72 - align coordinate system with application)
  - [7.3. set layer parameters](#73-Set Layer Parameters)
- [8.2. Basic control of effects](#8-Appendix-Basic control of effects)
  - [8.1. play an effect](#81-Play an effect)
  - [8.2. moving an effect] (#82 - Moving an effect)
  - [8.3. Rotate Effect](#83-Rotate Effect)
  - [8.4. Scale Effect](#84-Scale Effect)
  - [8.5. Stop Effect] (#85-Stop Effect)
  - 8.6. looping an effect](#86-playing an effect in a loop)
- [9. Troubleshooting (When it doesn't work)](#9-Troubleshooting when it doesn't work)
  - [9.1. "Your PC has been protected by Windows" appears](#91-Windows has protected my PC)
  - [9.2. CMake build-related problems](#92-When CMake build-related problems occur)
  - [9.3. build errors in the destination application](#93-Build errors in the destination application)
    - [9.3.1. "Unable to open LNK1104 file 'Effekseer.lib'. 'E1696 Cannot open source file "Effekseer.h"' 'C1083 Cannot open include file '. Effekseer.h':No such file or directory"](#931-lnk1104 file -effekseerlib- cannot open e1696 source file -effekseerhc1083include- cannot open file cannot-effekseerhno-such-file-or-directory)
    - [9.3.2. "LNK2001 external symbol "public: \_thiscall Effekseer::Matrix44::Matrix44(void)" (? ?0Matrix44@Effekseer@@QAE@XZ) is unresolved" "LNK4272 Library computer type 'x64' conflicts with target computer type 'x86'" "LNK2038 'RuntimeLibrary' mismatch detected. LNK2038 'RuntimeLibrary' mismatch detected. The value 'MDd\_DynamicDebug' does not match the value 'GraphEditor.obj' in MTd\_StaticDebug."] (#932-lnk2001-external-symbol-public-__thiscall-effekseermatrix44matrix44void-0matrix44effekseerqaexz-is-unresolved-lnk4272-library-computer-type-x64-is lnk2038runtimelibrary- mismatch detected value-mdd_dynamicdebug- does not match -mtd_staticdebug- value-grapheditorobj- conflicting with target computer type-x86-)


## 1. Check the environment of the application to be installed

To install Effekseer with the correct configuration, first check the project settings for the application you are installing.

**Check the following four items.
They will be required for subsequent builds and installations. **

- **VisualStudio version**.
(e.g. Visual Studio 2022)
- **DirectX Version** (DirectX 11 or 12)
(DirectX 11 or 12)
- **Platform** (x86/Win32 or x64)
(x86/Win32 or x64)
- **"Runtime Library" setting** in project settings
(multithreaded (debug) or multithreaded (debug) DLL)

> [!NOTE]
> If you are using the DX library, use the Effekseer For DX library, not Effekseer For C++.

### 1.1. how to check platform and runtime library settings

> [!NOTE]
> If you know how to check, you may skip this section.

You can check it on the project properties screen of Visual Studio.

The following is an example of how to check when there are two configurations, "Debug" and "Release".

In the Solution Explorer, right-click on the project you wish to deploy to.

Select "Properties" from the menu that appears to open the Properties page.
(The names and configurations of the solutions/projects in the figure are examples.)
! [VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_Ja.png)

> [!NOTE]
> If you have more than one project, open the properties of the project for which you want to use Effekseer.


 
The platform can be found under "Platform (P)" at the top of the screen.
! [VisualStudio_Platform](images/VisualStudio_Platform_Ja.png)
> [!NOTE]
> The platform marked "active" is the platform setting you are currently using.

> [!NOTE]
> In many cases, `Win32` `x86` `x64`, etc. are set.


 
In the Solution Explorer, select Configuration Properties > C/C++ > Code Generation.

5. **Set "Configuration (C)" in the upper left corner of the screen to either `Debug` or `Release` **, then check "Runtime Library".
> [!NOTE]
> `Multithreading (/MT)` ` `Multithreaded Debug (/MTd)` `Multithreaded DLL (/MD)` `Multithreaded Debug DLL (/MDd)` > [!NOTE].

## 2. Install CMake

Effekseer For C++ does not include pre-built binaries.
Therefore, it is necessary to build it before installation.

To build, use the software CMake.

Download and install CMake from the official website.

**CMake download page: <a href="https://cmake.org/download/" target="_blank" rel="noopener noreferrer">https://cmake.org/download/</a>**

> [!IMPORTANT]
> Download the Installer, not the Source.
> For example, if you are using a 64-bit version of Windows, download the Windows x64 Installer (`cmake-x.xx.0-xxx-windows-x86_64.msi`).

**Be sure to check either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user" to pass PATH. **
(If you can set the environment variable/PATH by yourself, you can do so.)
! [CMake_InstallOptions](images/CMake_InstallOptions.png)

> [!NOTE]
> > > Security software, such as Windows Defender, may stop the startup.
> Related: [PC protected by Windows](#91-windows protected my pc)

## 3. Build Effekseer For C++

### 3.1. download

First, download and unzip Effekseer for C++ (`EffekseerForCppXXX.zip`).

**Effekseer for C++ download page: <a href="https://github.com/effekseer/Effekseer/releases" target="_blank" rel="noopener noreferrer">https ://github.com/effekseer/Effekseer/releases</a>**

### 3.2. Build

A batch file is available to run the build.

Run `build_msvc.bat`.

> [!NOTE]
> > > Security software, such as Windows Defender, may stop the startup.
> Related: [9.1. "Your PC has been protected by Windows"](#91-"Your PC has been protected by windows")

When executed, the console will be displayed.
Below is an example of what will be displayed. (The presets displayed may vary depending on the version of Effekseer for C++.)

````
Preset numbers for building Effekseer for cpp:.
[1] Visual Studio 2017(x86)
[2] Visual Studio 2017(x64)
[3] Visual Studio 2019(x86)
[4] Visual Studio 2019(x64)
[5] Visual Studio 2022(x86)
[6] Visual Studio 2022(x64)
[7] Visual Studio 2019(x86) with Vulkan
[8] Visual Studio 2019(x64) with Vulkan
[9] Visual Studio 2022(x86) with Vulkan
[10] Visual Studio 2022(x64) with Vulkan
------------------------------------

```

From the presets displayed,
 Find the preset that matches your `VisualStudio version` and `platform` that you have checked in [1.

For example, if the "Visual Studio version is `2022` and the platform is `x64`, the preset to use is "`Visual Studio 2022(x64)`", and if the "Visual Studio version is `2019` and the platform is `Win32`, the preset to The preset to use is "`Visual Studio 2019(x86)`" if "Visual Studio version is `2019` and platform is `Win32`".

> [!NOTE]
> If your platform is `Win32`, select the `(x86)` preset.

> [!WARNING]
> > If you are not using vulkan, do not use presets marked with **`with Vulkan**.
> vulkan is a different graphics API than DirectX or OpenGL.

**When you find the appropriate preset, enter the corresponding number and press Enter. **

Next, you will be asked if you want to activate the runtime library DLL setting.

````
Enable runtime library DLL option?
```

 [Check the environment of the application you are going to install](#1-Check the environment of the application you are going to install)**, enter the following and press the Enter key. **




 
  

- `Multithreading (/MT)`or`Multithreaded Debugging (/MTd)`: `n`
- `Multithreaded DLL (/MD)`or`Multithreaded Debug DLL (/MDd)`: `y`

The build will begin.
After a short wait, the build will complete.
(Regardless of whether it succeeded or not, you will be prompted to `press any key to continue' when the build process is finished.)

An installation folder is created.

For example, Visual Studio 2022 for x86,
The generated installation folder is `install_msvc2022_x86`.
**From now on, this folder will be referred to as the "install folder". **

The installation folder consists of the following folder structure, which generates include files and library files.
**Check that the installation folder and the files and folders it contains are generated correctly. **

````
📁install_msvcXXXX_XXX/ <- install folder
 ├ 📁include/
 │ │ (Each folder contains header files (.h), etc.)
 │ ├ 📁Effekseer/
 │ ├ 📁EffekseerRendererCommon/
 │ ├ 📁EffekseerRendererDX9/
 │ ├📁EffekseerRendererDX11/
 │ ├📁EffekseerRendererDX12/
 │ ├📁EffekseerRendererLLGI/
 │ ├📁EffekseerSoundDSound/
 │ ├📁GLFW/
 │ └ 📁LLGI/
 └ 📁lib/
 　 │ (library files (.lib) with a number of
 　 └ Example: Effekseer.lib, Effekseerd.lib, EffekseerRendererCommon.lib, EffekseerRendererDX12.lib)
````



> [!NOTE]
> If build fails, check Troubleshooting.
> [9.2. CMake build-related problems](#92-cmake build-related problems)

## 4. Placement of built library files

From the installation folder created in [3. Build Effekseer For C++](#3-build-effekseer-for-c), copy the necessary items for your environment to the project of the application you are installing.

> [!NOTE]
> The arrangement of files and folders on this document is an example.
> If you know what you are looking for, please feel free to set it up.

> [!NOTE]
> ** Here, the folder where the application project (.proj file) is located is called the "Destination Project Folder". **

### 4.1. Placement of header files (.h)

Copy the header file (.h) for inclusion.

From the installation folder (e.g. `install_msvc2022_x86/`) created in [3. Build Effekseer For C++](#3-effekseer-for-c build), copy what you need for your environment.

**Copy the following to `[destination project folder]/Libraries/Effekseer/Include/]`. **
**Copy the entire folder, including all files and folders inside. **

**<must copy***.
- `[installation folder]/include/Effekseer`.
- `[install folder]/include/EffekseerRendererCommon`.

**<Copy the following for DirectX 11 as well>**
- `[installation folder]/include/EffekseerRendererDX11`.

**<Copy the following for DirectX 12 as well>**
- `[installation folder]/include/EffekseerRendererDX12`.

> [!NOTE]
> Create your own `[Destination Project Folder]/Libraries/Effekseer/Include/` folder.

! [IncludeFiles](images/IncludeFiles_Ja.png)


### 4.2. Placement of library files (.lib)

Next, copy the library file (.lib).

From the installation folder (e.g. `install_msvc2022_x86/`) that was output when you built Effekseer for Cpp earlier,
**Copy the following files from the installation folder (e.g. `install_msvc2022_x86/`) to `*[destination project folder]/Libraries/Effekseer/Lib]`. **


**<must be copied***.
- `Effekseer.lib`.
- `Effekseerd.lib`
- `EffekseerRendererCommon.lib`
- `EffekseerRendererCommon.lib`

**<Copy the following for DirectX 11 as well>**
- `EffekseerRendererDX11.lib`.
- `EffekseerRendererDX11d.lib` **<DirectX 11, copy the following

**<DirectX 12, copy the following as well>**
- `EffekseerRendererDX12.lib`.
- `EffekseerRendererDX12d.lib`.

> [!NOTE]
> Create your own `[Destination Project Folder]/Libraries/Effekseer/Lib/` folder.

! [LibraryFiles](images/LibraryFiles_Ja.png)

> [!NOTE]
> Library files ending with `d`, such as `xxxxxxxd.lib`, are library files for the Debug build configuration settings.


### 4.3. Check the file arrangement again
Check again that the file/folder structure is as follows.

**In DirectX 11**.
````
📁[Destination project folder] (location of the destination project (.proj file))
└ 📁Libraries/
　└ 📁Effekseer/
　 　├ 📁Include/
　 　│ ├📁Effekseer/
　 　│ │ └ ...
　 　│ ├ 📁EffekseerRendererCommon/
　 　│ │ └ ...
　 　│ └ 📁EffekseerRendererDX11/
　 　│ │ └ ...
　 　│ └ 📁Lib/
　 　　├ 📄Effekseer.lib
　 　　├ 📄Effekseerd.lib
　 　　├📄EffekseerRendererCommon.lib
　 　　├📄EffekseerRendererCommon.lib
　 　　├📄EffekseerRendererDX11.lib
　 　　└ 📄EffekseerRendererDX11d.lib
````

**In DirectX 12**.
````
📁[Destination project folder] (location of the destination project (.proj file))
└ 📁Libraries/
　└ 📁Effekseer/
　 　├ 📁Include/
　 　│ ├📁Effekseer/
　 　│ │ └ ...
　 　│ ├ 📁EffekseerRendererCommon/
　 　│ │ └ ...
　 　│ └ 📁EffekseerRendererDX12/
　 　│ │ └ ...
　 　│ └ 📁Lib/
　 　　├ 📄Effekseer.lib
　 　　├ 📄Effekseerd.lib
　 　　├📄EffekseerRendererCommon.lib
　 　　├📄EffekseerRendererCommon.lib
　 　　├📄EffekseerRendererDX12.lib
　 　　└ 📄EffekseerRendererDX12d.lib
````

## 5. VisualStudio project settings for the destination application

Configure the project settings so that the destination application can handle the Effekseer for C++ library.
Set up the "include directory" and "library directory".

> [!NOTE]
> **From here, work with the application you are installing. **
> **Open the VisualStudio solution (project) of the application you are installing. **

**In VisualStudio, open the properties of the destination project. **

1. from the Solution Explorer, right-click on the project of the application you wish to deploy. 2.
Select "Properties" from the menu that appears to open the Properties page.
(The solution/project names and configurations in the figure are examples.)
! [VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_Ja.png)

### 5.1. Include Directory Settings

Set the include directory to register the location where the include files are placed.

1. **Be sure to set the configuration at the top of the screen to "All configurations" and the platform to the platform you confirmed in [1. Confirm the environment of the application to be installed](#1-Confirm the environment of the application to be installed). **
! [ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
1. select Configuration Properties > C/C++ > General > Additional Include Directories > Edit.
! [VisualStudio_IncludeDirectory_Ja](images/VisualStudio_IncludeDir_Ja.png)
2. add the following

**<must describe***.
````
. \Libraries\Effekseer\Include\Effekseer
. \Libraries\Effekseer\Include\EffekseerRendererCommon
````
**<In case of DirectX 11, also include the following:**
```
. \```` .
```
**<DirectX 12 also includes:** ```` .
```
. \```` .
```
! [VisualStudio_IncludeDirAppend](images/VisualStudio_IncludeDirAppend_Ja.png)
Press "OK".

### Configure the library directory.

Similarly, set the location where the library files are placed.

1. **Be sure to set the configuration at the top of the screen to "All configurations" and the platform to the platform you confirmed in [1. Confirm the environment of the application to be installed](#1-Confirm the environment of the application to be installed). **
! [ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
Select Configuration Properties > Linker > General > Additional Library Directories > Edit.
! [VisualStudio_LibraryDir](images/VisualStudio_LibraryDir_Ja.png)
3. add the following.
```` .
. \Libraries\Effekseer\Lib
````

! [VisualStudio_IncludeDirAppend](images/VisualStudio_LibraryDirAppend_Ja.png)
Press "OK".


   
### 5.3. apply

When you have completed setting up both the include and library directories,
Click "Apply (A)" in the lower right corner of the properties page to reflect the changes.
Finally, click "OK" to close the properties page.

## 6. Play the effect in the application

Call the Effekseer API from the source code to initialize and update Effekseer, play and draw effects, etc.

Please refer to the sample source code for implementation.

- **DirectX11 sample sources: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11](https://github.com/effekseer/Effekseer /tree/master/Examples/DirectX11)**
- **DirectX12 sample sources: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12](https://github.com/effekseer/Effekseer /tree/master/Examples/DirectX12)**


Also, please refer to the sample source code below for an explanation.

> [!NOTE]
> The `device` (`DeviceDX11` `DeviceDX12`) in the sample is a class for managing graphics API devices for the sample.
> In the actual application, they are managed by the application side (or framework, etc.).
> In the sample sources, where `device` is used, please replace it with the environment on the application side and implement accordingly.

### Prepare the effect file to be played back.

First, prepare the effect to be played.
Originally, this should be created in the Effekseer editor, but this time we will use the sample effect.
Copy the following files from the downloaded EffekseerForCpp to the project folder (.proj file folder).

- `Examples/Resources/Laser01.efkefc`.
- `Examples/Resources/Texture` (by folder)

After copying, the folder structure should look like the following.

````
[Destination project folder] (location of the destination project (.proj file))
Laser01.efkefc
└ Texture/
  Texture/ ...
```

### 6.2. include and pragma statement

**<must describe***.
```cpp
#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#else
#pragma comment(lib, "Effekseerd.lib")
#endif
#include <Effekseer.h>
```

**<when using DirectX11, also write
```cpp
#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX11.lib")
#else
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#endif
#include <EffekseerRendererDX11.h>
```

**<when using DirectX12, also write
```cpp
#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX12.lib")
#else
#pragma comment(lib, "EffekseerRendererDX12d.lib")
#endif
#include <EffekseerRendererDX12.h>
```

Specify to include the required header files and link library files.
For debug configuration, use `XXXXXXXd.lib`.

Creating an Effects Manager ### 6.3.

```cpp
// Create a manager of effects
// create a manager of effects
::Effekseer::ManagerRef efkManager = ::Effekseer::Manager::Create(8000);
```

Create and initialize `Effekseer::Manager`.

> [!TIP]
> > > Effekseer types with `Ref`, such as `Effekseer::ManagerRef` and `Effekseer::EffectRef`, are managed by smart pointers.
> These are automatically released when there are no more variables using the object.

### 6.4. Create a drawing module

**<in DirectX11***.

```cpp
// Create a graphics device
// create a graphics device
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(device.GetID3D11Device(), device.GetID3D11DeviceContext());

// Create a renderer of effects
// Create a renderer of effects
::EffekseerRenderer::RendererRef efkRenderer;
efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);
```

**<in case of DirectX12***.
```cpp
// Create a graphics device
// create a graphics device
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(device.GetID3D12Device(), device.GetCommandQueue(), 3);

// Create a renderer of effects
// Create a renderer of effects
::EffekseerRenderer::RendererRef efkRenderer;
DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

// Create a memory pool
// create a memory pool
::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;.
efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

// Create a command list
// create a command list
::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;.
efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
```

In DirectX11, in `::EffekseerRendererDX11::CreateGraphicsDevice()`, the DirextX11 Device (`ID3D11Device*`) and DeviceContext (`ID3D11 DeviceContext*`) are passed as arguments.

In DirectX12, at `::EffekseerRendererDX12::CreateGraphicsDevice()`, the DirectX12 Device (`ID3D12Device*`) and CommandQueue (`ID3D12CommandQueue *`) are passed as arguments.
The second and fourth arguments of `**`::EffekseerRendererDX12::Create` pass the render target and depth buffer format `DXGI_FORMAT`.
Pass the appropriate one for the application you are installing. **

**On the sample source, Device etc. are obtained from the device management class (`device`, `DeviceDX11`/`DeviceDX12`) for the sample and set. **
** When actually incorporating the device, please replace it with an implementation suitable for your application/framework. **

### 6.5. Configuration of the created drawing module

```cpp
// Sprcify rendering modules
// Set rendering modules
efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

// Specify a texture, model, curve and material loader
// It can be extended by yourself. It is loaded from a file on now.
Specify a texture, model, curve and material loader // It can be extended by yourself.
// It can be extended by yourself. It is loaded from a file on now.
efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
```


### 6.6. Coordinate System Settings

```cpp
// Setup the coordinate system. This must be matched with your application.
// This must be matched with your application. This must be matched with your application.
efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
```

Sets the coordinate system.

In practice, it is necessary to match the coordinate system of the application to which it is introduced, but first we will implement it in the right-hand coordinate system (`RH`) for the purpose of checking its operation.
(The coordinate system is described later. (Related: [7. Match the camera and coordinate system with the application](#7-Match the camera and coordinate system with the application))

### 6.7. Create the projection and camera matrices

```cpp
// Specify a position of view
// Specify a position of view
::Effekseer::Vector3D viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

// Specify a projection matrix
// Specify a projection matrix
::Effekseer::Matrix44 projectionMatrix;.
PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

// Specify a camera matrix
// Specify a camera matrix
::Effekseer::Matrix44 cameraMatrix;
cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
```

Creates the projection (projection) and camera matrices.

This also needs to be matched (synchronized) with that of the application in practice, but first we will create and use a fixed projection matrix/camera matrix in the right-hand coordinate system (RH) for the purpose of checking the operation.

### 6.8. Loading Effects

```cpp
// Load an effect
// Load an effect
Effekseer::EffectRef effect;.
effect = Effekseer::Effect::Create(efkManager, u "Laser01.efkefc");
```

Loads an effect from the effect file (`.efkefc`) copied in [6.1. Prepare effect file to be played](#61-Prepare effect file to be played).
The loaded effect data (`EffectRef`) is returned as the return value.

`EffectRef` (`effect`) is not the effect being played back.
It can be used to play back multiple copies of the same effect.

Also, since this is a read process, the processing time is relatively long.

So basically, `Effekseer::Effect::Create` only needs to be run once for one type of effect file.

> [!WARNING].
> The macro `EFK_EXAMPLE_ASSETS_DIR_U16` in the sample source is for sample use only.
> > > You cannot use it in the application you are introducing.

> [!NOTE]
> The second argument of the `Effekseer::Effect::Create()` function, `path`(.efkefc file path), is of type `char16_t*`.
> You cannot use normal string literals (`"ABCDE"`) or normal strings (`char*`,`std::string`) as they are.
> In the above source, a string literal is made into a string literal of type `char16_t*` by prefixing it with `u`.

### 6.9. playback and control of effects


```cpp
// Elapsed frames
// counter for elapsed frames
int32_t time = 0;

// Used to control the played effect.
// Used to control the played effect. Used to control the played effect.
Effekseer::Handle efkHandle = 0;

while (device.NewFrame())
{
    // Stop and play effects every 120 frames(for sample)
    // Stop and play effects every 120 frames (for sample)
    if (time % 120 == 0)
    {
        // Play an effect
        // play an effect
        efkHandle = efkManager->Play(effect, 0, 0, 0); }
    }

    if (time % 120 == 119)
    {
        // Stop effects
        // Stop effects
        efkManager->StopEffect(efkHandle); }
    }

    // Move the effect
    // move the effect
    efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

    ... (omitted)

    time++; }
}
```

The effect is stopped and played back every 120 frames while moving every frame.

> [!NOTE]
> `while (device.NewFrame()) {}` is the main loop.
> The main loop is implemented in `while (device.NewFrame() {}`, which is the main loop.
> (The implementation of the main loop is also for the sample. The main loop should be implemented by replacing it with the implementation of the application or framework in which it will be implemented.)

The number of elapsed frames is counted by incrementing `time` every frame.

By using `Effekseer::Handle` (`efkHandle`), we can control the effects that are playing, such as moving or stopping.

### 6.10. [DirectX12 only, every frame] Processing at start of frame


```cpp
NewFrame())
{
  // <<Add the following for DirectX 12 only>>

  // Call on starting of a frame
  // Call on starting of a frame
  efkMemoryPool->NewFrame();

  // Begin a command list
  // Begin a command list.
  EffekseerRendererDX12::BeginCommandList(efkCommandList, device.GetCommandList());
  efkRenderer->SetCommandList(efkCommandList);

  ... (omitted)
````

**Need to be done every frame only in DirectX 12. **
At the beginning of the main loop, start recording the list of commands used for drawing.
Add this to the beginning of the main loop in [6.9. Playback and Control of Effects](#69 - Playback and Control of Effects).


Again, GetCommandList is obtained from the device management class (`device`) provided for the sample and set, but this too can be replaced and implemented according to the application to be introduced.

### 6.11. [Execute every frame] Manager update process

```cpp
// Set layer parameters
// Set layer parameters
Effekseer::Manager::LayerParameter layerParameter;
layerParameter.ViewerPosition = viewerPosition;
efkManager->SetLayerParameter(0, layerParameter);

// Update the manager
// Update the manager
Effekseer::Manager::UpdateParameter updateParameter;
efkManager->Update(updateParameter);
```


  
**This process must be executed every frame (written in the main loop). **

Update LayerParameter (`efkManager->SetLayerParameter`) and Manager (`efkManager->Update`).

When updating the Manager, you can pass `Effekseer::Manager::UpdateParameter` as an argument.
This allows control over elapsed frames, number of updates, asynchronous processing, etc.

### 6.12. [Every frame] Renderer update and drawing process

```cpp
// Execute functions about DirectX
// Execute functions about DirectX
ClearScreen(); device.

// Update a time
// Update a time
efkRenderer->SetTime(time / 60.0f);

// Specify a projection matrix
// Set the projection matrix
efkRenderer->SetProjectionMatrix(projectionMatrix);

// Specify a camera matrix
// Specify a camera matrix
efkRenderer->SetCameraMatrix(cameraMatrix);

// Begin to rendering effects
// Begin to rendering effects
efkRenderer->BeginRendering();

// Render effects
// render effects.
Effekseer::Manager::DrawParameter drawParameter;
ZNear = 0.0f; drawParameter.
ZFar = 1.0f; drawParameter.
drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
efkManager->Draw(drawParameter);

// Finish to rendering effects
// Finish to rendering effects
efkRenderer->EndRendering();

// Execute functions about DirectX
// Execute functions about DirectX
device.PresentDevice();
````

**This process must be executed every frame (written in the main loop). **

**This should be done between the start of the drawing process in DirectX and the end of the drawing process. **
(In the sample, this is done between `device.ClearScreen();` and `device.PresentDevice();`)

The elapsed time is updated and the projection/camera matrix is set before the drawing process.

In `efkRenderer->SetTime(time / 60.0f);` the elapsed time is set using `time`, which was created in [6.9. Effects Playback and Control](#69 - Effects Playback and Control) and incremented every frame.
In this sample, assuming a frame rate of 60 fps, the `time` is divided by 60.

### 6.13. [DirectX12 only, execute every frame] Command list termination process

```cpp
// <<Add the following only for DirectX 12>>
// *Implemented immediately after efkRenderer->EndRendering();.

// Finish a command list
// Finish a command list.
efkRenderer->SetCommandList(nullptr);
EffekseerRendererDX12::EndCommandList(efkCommandList);
```

**Need to be done every frame only in DirectX 12. **
Update and rendering of renderer](#612-Updating renderer and rendering every frame)

### 6.14. [DirectX12 only] Release Processing

```cpp
// time++; // }
// }
// DirectX12 only, written just after the main loop ends

efkCommandList.Reset();
Reset(); efkMemoryPool.
Reset(); efkRenderer.
```

Only DirectX12 performs the release process.
It is implemented immediately after the main loop ends.

### 6.15. Check the operation

Up to this point, the implementation is to play a minimum of effects.

Please try to build and run once.

If it has been installed correctly, the effect should play as shown in the figure below.

If it does not play correctly or you get a compile/link error, etc., there may be an error somewhere. Please check and correct it.
Please also refer to [9. Troubleshooting (If it doesn't work)](#9-Troubleshooting if it doesn't work).

! [LaserEffect](images/LaserEffect.png)

## 7. Match camera and coordinate system with application

In the current state, moving the camera or changing the viewing angle on the application side does not change the position or visibility of the effect.

Add/change the coordinate system to match the camera of the Effekseer and the application so that the effect is drawn in the correct position.

### 7.1. synchronizing the camera

The projection and camera matrices must be set on the Effekseer side with the camera information for each frame.

The following is an example implementation of synchronizing the projection (projection) matrix and the camera matrix.

**Add the following to [6.12. [Every frame] Renderer update and drawing process](#612-Every frame renderer update and drawing process), before the `efkRenderer->SetProjectionMatrix` and `efkRenderer->SetCameraMatrix` calls. SetCameraMatrix`. **

```cpp
// efkRenderer->SetProjectionMatrix Add before calling efkRenderer->SetCameraMatrix

// Effekseer's CameraMatrix refers to the inverse of the camera attitude matrix. If the camera matrix is the camera attitude matrix, it should be inverted.
auto invAppCameraMatrix = appCameraMatrix.Invert();
// Synchronize the application and Effekseer's projection and camera matrices
for (int i = 0; i < 4; ++i)
{
    for (int j = 0; j < 4; ++j)
    {
        projectionMatrix.Values[i][j] = appProjectionMatrix.m[i][j];
        cameraMatrix.Values[i][j] = invAppCameraMatrix.m[i][j];
    }
}
```

> [!NOTE].
> `appProjectionMatrix` and `invAppCameraMatrix` are the projection (projection) matrix and camera matrix on the application side.
> Also, this is an example implementation when it is a `DirectX::SimpleMath::Matrix` type of DirectXTK.
> Please replace and implement it according to the application you are introducing.

> [!IMPORTANT]
> Effekseer's CameraMatrix refers to **the inverse of the camera's attitude matrix**.
> On the other hand, many frameworks refer to the camera's attitude matrix as CameraMatrix and its inverse as ViewMatrix.
> Therefore, in the example implementation above, the inverse of CameraMatrix is set.

### Match the coordinate system with the application

There are two types of coordinate systems for representing 3D space: right-handed (RH) and left-handed (LH).

This coordinate system must match the application.
(Effekseer's coordinate system is right-hand (RH) by default.)

**If your application is left-handed, you will need to set Effekseer to a left-handed coordinate system when you initialize it. **
**The part where `m_efkManager->SetCoordinateSystem` is called in [6.6. Setting coordinate system](#66-Setting coordinate system) can be changed to the left-hand (LH) coordinate system by rewriting as follows: **[6.6. **
(If the coordinate system is the right hand coordinate system, there is no problem with the sample as it is (RH))

```cpp
...
// If the application you are introducing uses a left-handed coordinate system, change to the following
m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
...
```

> [!NOTE]
> The difference between the left hand/right hand coordinate system is the **combination of the positive (plus) orientations of the XYZ axes**.
> As shown in the figure below, the direction of the thumb/forefinger/middle finger of the left/right hand corresponds to the positive (plus) orientation of the XYZ axes.
> This allows you to examine the coordinate system of the application you are introducing.
> ! [CoordinateSystem](images/HelpCpp_CoordinateSystem_Ja.png)

### 7.3. Layer Parameter Settings

In the implementation as shown in the sample, the `layerParameter.ViewerPosition` is set to a fixed coordinate (`viewerPosition`).

This also needs to be adjusted to the actual camera coordinates.

The following is an implementation example of setting `layerParameter.ViewerPosition` according to the actual camera coordinates.
**Add or change the corresponding section in [6.11. [Execute every frame] Manager update processing](#611 - Execute every frame manager update processing) as follows: **Add or change the following section in [6.11. [Execute every frame] Manager update processing](#611 - Execute every frame manager update processing). **

```cpp
// Set layer parameters
Effekseer::Manager::LayerParameter layerParameter;
::Effekseer::Matrix44 invEfkCameraMatrix;
::Effekseer::Matrix44::Inverse(invEfkCameraMatrix, cameraMatrix);
layerParameter.ViewerPosition = ::Effekseer::Vector3D(invEfkCameraMatrix.Values[3][0], invEfkCameraMatrix.Values[3][1],. invEfkCameraMatrix.Values[3][2]);.
```

The coordinates of the camera are taken from the Effekseer camera matrix `cameraMatrix`, which was synchronized with the application earlier in [7.1.


## 8. [Appendix] Basic Control of Effects

This section explains the fundamentals of controlling effects.

> [!NOTE]
> - `Effekseer::EffectRef` is a reference to the data of an effect loaded with `Effekseer::Effect::Create`. It is used to start playing the effect.
> - `Effekseer::EffectHandle` is the identification number of each and every instance of the played effect. It is used to perform operations on the played effects. It can be retrieved, for example, by the return value of a playback with `efkManager->Play`.
>
> The figure below is an illustration.
> ! [EffectRefHandle_Explain](images/EffectRefHandle_Explain_Ja.png)

### 8.1. playing back effects

Play the effect with `efkManager->Play`.
You can also specify the position of the effect as an argument.

**The return value `Effekseer::Handle` can be used to control the played back effect instance**.

```cpp
Effekseer::Handle efkHandle = 0;.
// set the coordinates where you want the effect to appear
efkHandle = efkManager->Play(effect, 0.0f, 0.0f, 0.0f);
```

### 8.2. moving effects

With `efkManager->SetLocation`, move the effect instance to the desired coordinates.

```cpp
efkManager->SetLocation(efkHandle, 1.0f, 1.0f, 1.0f);
```

You can also use ``efkManager->AddLocation`` to move it by the specified coordinates.
```cpp
efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));
```

### 8.3. Rotate Effects

Rotate the posture of the effect instance with `efkManager->SetRotation`.

Angles are specified in radians.

```cpp
// Rotation about the XYZ coordinate axes
efkManager->SetRotation(efkHandle, 3.141592653589f, 0.0f, 0.0f);
```

### 8.4. scale the effect

Set the scaling of the effect instance with `efkManager->SetScale`.

```cpp
// set the scale to 2x
efkManager->SetScale(efkHandle, 2.0f, 2.0f, 2.0f);
```

### 8.5. Stop the effect

Stop the effect instance with `efkManager->StopEffect`.

```cpp
efkManager->StopEffect(efkHandle);
```

You can also use `efkManager->SetPaused` to pause and resume.

```cpp
// pause
efkManager->SetPaused(efkHandle, true);

// Resume
efkManager->SetPaused(efkHandle, false);
```

Loop playback of effects ### 8.6.

If the effect has finished playing, it can be looped by playing it again anew.

```cpp
// Check every frame to see if the effect has finished playing.
if (efkManager->Exists(efkHandle))
{
  // Play the effect again. Set the coordinates to the location where you want to display the effect.
  // (position, rotation, and scaling must also be re-set)
  efkHandle = efkManager->Play(efkManager, 0.0f, 0.0f, 0.0f);
}
```

## 9. Troubleshooting (when it doesn't work)

### 9.1. "PC protected by Windows" is displayed

Displayed by Widnows Defender.

You can do this by clicking on `More Information` near the center of the screen and then clicking on the `Execute` button that appears.

! [WindowsDefender_protection](images/WindowsDefender_protection_Ja.png)

### 9.2. When CMake build-related problems occur

Please check or try the following

- If you clone from the master branch on GitHub, try downloading the for C++ from the Releases page ([https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/ Effekseer/releases)).
- Make sure you have the latest version of CMake installed.
- Make sure that the path to CMake is working (if not, make sure that you have installed the latest version of CMake)
(If not, pass the path by yourself or reinstall CMake. When reinstalling, be sure to check either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user" to pass the PATH)
- Make sure you have not accidentally built with vulkan presets.

### 9.3. Build Error in Deployed Application

> [!NOTE]
> The error statement shown below is an example.
> > > > Please read file names such as `Effekseer.h` `Effekseer.lib` and so on as you see fit.

#### 9.3.1. "Cannot open LNK1104 file 'Effekseer.lib'." E1696 Cannot open source file 'Effekseer.h'" "C1083 Cannot open include file '. Effekseer.h':No such file or directory"

If any of these occur, the corresponding file cannot be opened because it cannot be found or for other reasons.
Please check the following

- Are the relevant files placed in the correct directories?
- The `library directory` and `include directory` in the project properties are set correctly
- Are the `library directory` and `include directory` in the project properties set for the appropriate project, configuration, and platform?
- Preprocessor statements such as `#include <Effekseer.h>` `#pragma comment(lib, "Effekseer.lib")` are correct

> [!NOTE]
> Project properties apply only to the configuration/platform at the top of the properties page.
>
> Even if you think you have set them correctly, the `library directory` or `include directory` may not be set correctly due to an error in the configuration or > platform you have selected.
>
> > When setting up the `library directory` or `include directory`, be sure to set the proper ` library directory` or `include directory`.
> ! [VisualStudio_ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)

#### 9.3.2. "LNK2001 external symbol "public: __thiscall Effekseer::Matrix44::Matrix44(void)" (? ?0Matrix44@Effekseer@@QAE@XZ) is unresolved" "LNK4272 Library computer type 'x64' conflicts with target computer type 'x86'" "LNK2038 'RuntimeLibrary' mismatch detected. LNK2038 'RuntimeLibrary' mismatch detected. The value 'MDd_DynamicDebug' does not match the value 'GraphEditor.obj' in MTd_StaticDebug.

If either of these occur, there was an error in the preset or settings you selected when building with [3. Build Effekseer For C++] (#3-effekseer-for-c build).

Please follow the steps below to build Effekseer For C++ again with the correct presets and settings.

1. delete the build folder (`build_msvcXXXX_XXXX`) and the installation folder (`install_msvcXXXXXX_XXXX`) of Effekseer For C++
2. build Effekseer For C++ with the presets and settings suitable for your project ([3. Build Effekseer For C++](#3-effekseer-for-c build))
3. copy the newly generated library files in the installation folder back to the destination project ([4. Deployment of the built library files](#4-Deployment of the built library files))
