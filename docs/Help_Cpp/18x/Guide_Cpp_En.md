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
> **If you are unfamiliar with library installation, we recommend backing up your project beforehand.**  
> If you make a mistake when installing the library, it may cause anomalies in the project settings of your application.

> [!NOTE]
> In this guide, the game/application to be installed is referred to as the "Your Application" or "Application".


- [1. Check the settings of Your Application](#1-check-the-settings-of-your-application)
  - [1.1. How to check Platform and "Runtime Library" settings](#11-how-to-check-platform-and-runtime-library-settings)
- [2. Install CMake](#2-install-cmake)
- [3. Build Effekseer For C++](#3-build-effekseer-for-c)
  - [3.1. Download](#31-download)
  - [3.2. Build](#32-build)
- [4. Copy built files](#4-copy-built-files)
  - [4.1. Copy header files (.h)](#41-copy-header-files-h)
  - [4.2. Copy library files (.lib)](#42-copy-library-files-lib)
  - [4.3. Check the file placement again](#43-check-the-file-placement-again)
- [5.  Setting up Visual Studio Project of Your Application](#5--setting-up-visual-studio-project-of-your-application)
  - [5.1. Include Directory Settings](#51-include-directory-settings)
  - [5.2. Configure the Library Directory](#52-configure-the-library-directory)
  - [5.3. Apply](#53-apply)
- [6. Play the effect in Your Application](#6-play-the-effect-in-your-application)
  - [6.1. Prepare the effect file to be played back](#61-prepare-the-effect-file-to-be-played-back)
  - [6.2. Include and pragma statements](#62-include-and-pragma-statements)
  - [6.3. Creating a Manager](#63-creating-a-manager)
  - [6.4. Create a drawing module](#64-create-a-drawing-module)
  - [6.5. Configuration of the created drawing module](#65-configuration-of-the-created-drawing-module)
  - [6.6. Setting the Coordinate System](#66-setting-the-coordinate-system)
  - [6.7. Create the projection and camera matrices](#67-create-the-projection-and-camera-matrices)
  - [6.8. Loading Effects](#68-loading-effects)
  - [6.9. Playback and control of effects](#69-playback-and-control-of-effects)
  - [6.10. \[DirectX12 only, every frame\] Processing at start of frame](#610-directx12-only-every-frame-processing-at-start-of-frame)
  - [6.11. \[Every frame\] Manager update](#611-every-frame-manager-update)
  - [6.12. \[Every frame\] Renderer update and drawing](#612-every-frame-renderer-update-and-drawing)
  - [6.13. \[DirectX12 only, every frame\] Finish command list](#613-directx12-only-every-frame-finish-command-list)
  - [6.14. \[DirectX12 only\] Release](#614-directx12-only-release)
  - [6.15. Checking operation](#615-checking-operation)
- [7. Match camera and coordinate system with application](#7-match-camera-and-coordinate-system-with-application)
  - [7.1. Synchronizing the camera](#71-synchronizing-the-camera)
  - [7.2. Match the coordinate system with the application](#72-match-the-coordinate-system-with-the-application)
  - [7.3. Layer Parameter Settings](#73-layer-parameter-settings)
- [8. \[Appendix\] Basic Control of Effects](#8-appendix-basic-control-of-effects)
  - [8.1. Playing back effects](#81-playing-back-effects)
  - [8.2. Moving effects](#82-moving-effects)
  - [8.3. Rotate Effects](#83-rotate-effects)
  - [8.4. Scale the effect](#84-scale-the-effect)
  - [8.5. Stop the effect](#85-stop-the-effect)
  - [8.6. Loop playback of the effect](#86-loop-playback-of-the-effect)
- [9. Troubleshooting](#9-troubleshooting)
  - [9.1. Dialog message: "Windows protected your PC"](#91-dialog-message-windows-protected-your-pc)
  - [9.2. CMake build-related problems](#92-cmake-build-related-problems)
  - [9.3. Build Error in Deployed Application](#93-build-error-in-deployed-application)
    - [9.3.1. Failure to open library/include file](#931-failure-to-open-libraryinclude-file)
    - [9.3.2. Failure to link library file](#932-failure-to-link-library-file)


## 1. Check the settings of Your Application

To install Effekseer with the correct configuration, first check the project settings for Your Application you are installing.

**Check the following items.  
These are needed later.**  

- **VisualStudio version** (e.g. Visual Studio 2022)
- **DirectX Version** (DirectX 11 or 12)
- **Platform** (x86/Win32 or x64)
- **"Runtime Library" setting** in project settings(Multi-threaded (Debug) or Multi-threaded (Debug) DLL)

### 1.1. How to check Platform and "Runtime Library" settings

> [!NOTE]
> If you know how to check, you may skip reading this section.

(The following is an example of how to check when there are two configurations, "Debug" and "Release".)

You can check them on the Project Properties Pages of Visual Studio.

1. Open Your Project. 
2. In the Solution Explorer, right-click on the project you wish to deploy to.  
(If there are multiple projects, open the properties of the project for which you want to use Effekseer.)
3. Select "Properties" from the menu that appears to open the Properties page.  
(The names and configurations of the solutions/projects in the figure are examples.)  
![VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_En.png)

 
4. The Platform can be found in the "Platform" at the top of the screen.  
In most cases, `Win32` `x86` `x64`, etc. are set.  
(The Platform marked "active" is the platform setting you are currently using.)

![VisualStudio_Platform](images/VisualStudio_Platform_En.png)  

5. Select Configuration Properties > C/C++ > Code Generation.

6. **Set "Configuration" in the upper left corner of the screen to either `Debug` or `Release`**, then check "Runtime Library".  
`Multi-threaded (/MT)`, `Multi-threaded Debug (/MTd)`, `Multi-threaded DLL (/MD)` `Multi-threaded Debug DLL (/MDd)` are set to one of them.  

> [!NOTE]
> If "Runtime Library" appears as `<different options>`,  try setting "Configuration" (in the upper left corner of the screen) to either `Debug` or `Release`.

## 2. Install CMake

Effekseer For C++ does not include pre-built binaries.
Therefore, it is necessary to build it before installation.

To build, use the software CMake.

Download and install CMake from the official website.

**CMake download page: <a href="https://cmake.org/download/" target="_blank" rel="noopener noreferrer">https://cmake.org/download/</a>**

> [!IMPORTANT]
> Download the Installer, not the Source.
> For example, if you are using a 64-bit version of Windows, download the Windows x64 Installer (`cmake-x.xx.0-xxx-windows-x86_64.msi`).

**Be sure to check either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user" to pass PATH.**  
(If you can set the environment variable/PATH by yourself, you can do so.)

![CMake_InstallOptions](images/CMake_InstallOptions.png)

> [!NOTE]
> Security software, such as Windows Defender, may stop the startup.
> Related: [9.1. Dialog message: "Windows protected your PC"](#91-dialog-message-windows-protected-your-pc)

## 3. Build Effekseer For C++

### 3.1. Download

First, download and unzip Effekseer for C++ (`EffekseerForCppXXX.zip`).  

**Effekseer for C++ download page: <a href="https://github.com/effekseer/Effekseer/releases" target="_blank" rel="noopener noreferrer">https://github.com/effekseer/Effekseer/releases</a>**

### 3.2. Build

A batch file is available to run the build.

Run `build_msvc.bat`.

> [!NOTE]
> Security software, such as Windows Defender, may prevents the startup.
> Related: [9.1. Dialog message: "Windows protected your PC"](#91-dialog-message-windows-protected-your-pc)

When the bat script is executed, the console will be displayed.  
Below is an example of what will be displayed. 

```
Preset numbers for building Effekseer for cpp:
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
Enter preset number:
```

From the presets displayed, find the preset that matches your `VisualStudio version` and `Platform` that you have checked in [1. Check the settings of Your Application](#1-check-the-settings-of-your-application).  
For example, if the "Visual Studio version" is `2022` and the "Platform" is `x64`, the preset to use is "`Visual Studio 2022(x64)`".  
And if the "Visual Studio version" is `2019` and the "Platform" is `Win32`, the preset to The preset to use is "`Visual Studio 2019(x86)`".  
(If "Platform" is `Win32`, select the `x86` preset.)  

> [!WARNING]
> If you are not using vulkan, **do not use presets marked with "`with Vulkan**".
> Vulkan is a different graphics API than DirectX.  

**When you find the appropriate preset, type the corresponding number and press Enter.**

Next, a message will appear asking if you want to enable the Runtime Library DLL setting.  

```
Enable runtime library DLL option?
```

Type either `y` or `n` according to the "Runtime Library" setting in the project settings,  which you checked in [1. Check the settings of Your Application](#1-check-the-settings-of-your-application).  
Then, press the Enter key.

- `Multithreaded DLL (/MD)`or`Multithreaded Debug DLL (/MDd)`: `y`
- `Multithreading (/MT)`or`Multithreaded Debugging (/MTd)`: `n`

The build will begin.  
Then, after a while, the build will complete.  
(Regardless of success or failure, a message will be displayed after the build process is complete, prompting you to 'Press any key to continue.')  

If the build is completed, the Install Folder will be created.  
For example, in the case of x86 with Visual Studio 2022, the generated Install Folder is `install_msvc2022_x86`.  
**From now on, we will this folder as the "Install Folder."**

The Install Folder has the following directory structure.  
**Please check that the Install Folder and its contents are generated correctly.**  

````
📁install_msvcXXXX_XXX/ <- Install Folder
 ├ 📁include/
 │ │ (Each folder contains header files (.h), etc.)
 │ ├ 📁Effekseer/
 │ ├ 📁EffekseerRendererCommon/
 │ ├ 📁EffekseerRendererDX9/
 │ ├ 📁EffekseerRendererDX11/
 │ ├ 📁EffekseerRendererDX12/
 │ ├ 📁EffekseerRendererLLGI/
 │ ├ 📁EffekseerSoundDSound/
 │ ├ 📁GLFW/
 │ └ 📁LLGI/
 └ 📁lib/
 　 │ (Contains library files (.lib)
 　 └   e.g. Effekseer.lib, Effekseerd.lib, EffekseerRendererCommon.lib, EffekseerRendererDX12.lib)
````

> [!NOTE]
> If build fails, check Troubleshooting.
> [9.2. CMake build-related problems](#92-cmake-build-related-problems)

## 4. Copy built files

From the Install Folder created in [3. Build Effekseer For C++](#3-build-effekseer-for-c), copy the necessary items into Your Project.

> [!NOTE]
> The arrangement of files and folders on this document is an example.
> If you know what you are looking for, please feel free to set it up.

> [!NOTE]
> **In this guide, The folder where Your Application's Project (.proj file) is located is called the "Your Project Folder".**

### 4.1. Copy header files (.h)

Copy the header file (.h) for inclusion.

From the Install Folder (e.g. `install_msvc2022_x86/`) created in [3. Build Effekseer For C++](#3-build-effekseer-for-c), copy what you need for your environment.

**Copy the following to `[Your Project Folder]/Libraries/Effekseer/Include/`.**   
**Copy the entire folder, including all files and folders inside.**   
(Create the folder `[Your Project Folder]/Libraries/Effekseer/Include/` manually.)  

**\<Must be copied>**
- `[Install Folder]/include/Effekseer`
- `[Install Folder]/include/EffekseerRendererCommon`

**<For DirectX 11, also copy the following>**
- `[Install Folder]/include/EffekseerRendererDX11`

**<For DirectX 12, also copy the following>**
- `[Install Folder]/include/EffekseerRendererDX12`


![IncludeFiles](images/IncludeFiles_En.png)


### 4.2. Copy library files (.lib)

Next, copy the library file (.lib).

From the Install Folder where Effekseer for Cpp was built(e.g. `install_msvc2022_x86/lib`),  
**copy the following files to `[Your Project Folder]/Libraries/Effekseer/Lib`.**    
(Create the folder `[Your Project Folder]/Libraries/Effekseer/Lib/` manually.)


**\<Must be copied>**
- `Effekseer.lib`
- `Effekseerd.lib`
- `EffekseerRendererCommon.lib`
- `EffekseerRendererCommond.lib`

**<For DirectX 11, also copy the following>**
- `EffekseerRendererDX11.lib`
- `EffekseerRendererDX11d.lib`

**<For DirectX 12, also copy the following>**
- `EffekseerRendererDX12.lib`
- `EffekseerRendererDX12d.lib`

![LibraryFiles](images/LibraryFiles_En.png)

> [!NOTE]
> Library files ending with `d`, such as `xxxxxxxd.lib`, are library files for the Debug build configuration settings.


### 4.3. Check the file placement again
Check again that the file/folder structure is as follows.

**DirectX 11**  
````
📁[Your Project folder] (location of Your Project (.proj file))
└ 📁Libraries/
　└ 📁Effekseer/
　 　├ 📁Include/
　 　│ ├ 📁Effekseer/
　 　│ │ └ ...
　 　│ ├ 📁EffekseerRendererCommon/
　 　│ │ └ ...
　 　│ └ 📁EffekseerRendererDX11/
　 　│   └ ...
　 　└ 📁Lib/
　 　　├ 📄Effekseer.lib
　 　　├ 📄Effekseerd.lib
　 　　├ 📄EffekseerRendererCommon.lib
　 　　├ 📄EffekseerRendererCommond.lib
　 　　├ 📄EffekseerRendererDX11.lib
　 　　└ 📄EffekseerRendererDX11d.lib
````

**DirectX 12**  
````
📁[Your Project folder] (location of Your Project (.proj file))
└ 📁Libraries/
　└ 📁Effekseer/
　 　├ 📁Include/
　 　│ ├ 📁Effekseer/
　 　│ │ └ ...
　 　│ ├ 📁EffekseerRendererCommon/
　 　│ │ └ ...
　 　│ └ 📁EffekseerRendererDX12/
　 　│   └ ...
　 　└ 📁Lib/
　 　　├ 📄Effekseer.lib
　 　　├ 📄Effekseerd.lib
　 　　├ 📄EffekseerRendererCommon.lib
　 　　├ 📄EffekseerRendererCommond.lib
　 　　├ 📄EffekseerRendererDX12.lib
　 　　└ 📄EffekseerRendererDX12d.lib
````

## 5.  Setting up Visual Studio Project of Your Application

Configure the project settings so that Your Application can handle the Effekseer for C++ library.
Set up the "Include Directory" and "Library Directory".

> [!NOTE]
> **The following steps are performed in Your Application Project.**  
> **Open the Visual Studio solution (project) of Your Application.**

**First, open Your Project's Properties Page**.  

1. Open Your Project.
2. from the Solution Explorer, right-click on the project of the application you wish to deploy. 
3. Select "Properties" from the menu that appears to open the Properties page.  
(The solution/project names and configurations in the figure are just examples.)  
![VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_En.png)

### 5.1. Include Directory Settings

Set the include directory to register the location where the include files are placed.

1. **Make sure to set the Configuration to "All Configurations" and the Platform to the one checked in [1. Check the settings of Your Application](#1-check-the-settings-of-your-application).**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_En.png)
2. Click Properties > C/C++ > General > Additional Include Directories > Edit.
![VisualStudio_IncludeDirectory_En](images/VisualStudio_IncludeDir_En.png)
3. Add the following entries.

**<Must describe>**  
```
.\Libraries\Effekseer\Include\Effekseer
.\Libraries\Effekseer\Include\EffekseerRendererCommon
```
**<For DirectX 11, also add>**
```
.\Libraries\Effekseer\Include\EffekseerRendererDX11
```
**<For DirectX 11, also add>**
```
.\Libraries\Effekseer\Include\EffekseerRendererDX12
```
![VisualStudio_IncludeDirAppend](images/VisualStudio_IncludeDirAppend_En.png)

4. Click "OK."

### 5.2. Configure the Library Directory

Similarly, set the location where the library files are placed.

1. **Be sure to set the configuration at the top of the screen to "All configurations" and the platform to the platform you confirmed in [1. Check the settings of Your Application](#1-check-the-settings-of-your-application).**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_En.png)
2. Select Configuration Properties > Linker > General > Additional Library Directories > Edit.    
![VisualStudio_LibraryDir](images/VisualStudio_LibraryDir_En.png)
3. Add the following.  
```
.\Libraries\Effekseer\Lib
```

![VisualStudio_IncludeDirAppend](images/VisualStudio_LibraryDirAppend_En.png)

4. Click "OK".

### 5.3. Apply

When you have completed setting up both the Include and Library directories,
Click "Apply" in the lower right corner of the Property Page to reflect the changes.  
Finally, click "OK" to close the Property Page.

## 6. Play the effect in Your Application

Call the Effekseer API from the source code to initialize and update Effekseer, play and draw effects, etc.

Please refer to the sample source code for implementation.

- **DirectX11 sample sources: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11)**
- **DirectX12 sample sources: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12)**


Also, please refer to the sample source code below for an explanation.

> [!NOTE]
> The `device` (`DeviceDX11` `DeviceDX12`) in the sample is a class for managing graphics API devices for the sample.  
> In the actual application, they are managed by the application side (or framework, etc.).  
> The `device` In the sample sources, please replace it with the environment on the application side and implement accordingly.  

### 6.1. Prepare the effect file to be played back

First, prepare the effect to be played.  
Originally, this should be created in the Effekseer editor, but this time we will use the sample effect.  
Copy the following files from the downloaded EffekseerForCpp to the project folder (.proj file folder).

- `Examples/Resources/Laser01.efkefc`
- `Examples/Resources/Texture` (entire folder, including all files and folders inside)

After copying, the folder structure should look like the following.

```
📁[Your Project folder] (location of your project (.proj file))
 ├ 📄Laser01.efkefc
 └ 📁Texture/
    └ ...
```

### 6.2. Include and pragma statements

**<Must describe>**  
```cpp
#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#else
#pragma comment(lib, "Effekseerd.lib")
#endif
#include <Effekseer.h>
```

**<For DirectX 11, also write>**
```cpp
#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX11.lib")
#else
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#endif
#include <EffekseerRendererDX11.h>
```

**<For DirectX 12, also write>**
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

### 6.3. Creating a Manager 

```cpp
// Create a manager of effects
::Effekseer::ManagerRef efkManager = ::Effekseer::Manager::Create(8000);
```

Create and initialize `Effekseer::Manager`.

> [!TIP]
> Effekseer types with `Ref`, such as `Effekseer::ManagerRef` and `Effekseer::EffectRef`, are managed by smart pointers.  
> These are automatically released when there are no more variables using the object.

### 6.4. Create a drawing module

**<For DirectX 11, also write>**
```cpp
// Create a graphics device
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(device.GetID3D11Device(), device.GetID3D11DeviceContext());

// Create a renderer of effects
::EffekseerRenderer::RendererRef efkRenderer;
efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);
```

**<For DirectX 12, also write>**
```cpp
// Create a graphics device
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(device.GetID3D12Device(), device.GetCommandQueue(), 3);

// Create a renderer of effects
::EffekseerRenderer::RendererRef efkRenderer;
DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

// Create a memory pool
::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

// Create a command list
::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;
efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
```

In DirectX11, in `::EffekseerRendererDX11::CreateGraphicsDevice()`, the DirextX11 Device (`ID3D11Device*`) and DeviceContext (`ID3D11 DeviceContext*`) are passed as arguments.

In DirectX12, at `::EffekseerRendererDX12::CreateGraphicsDevice()`, the DirectX12 Device (`ID3D12Device*`) and CommandQueue (`ID3D12CommandQueue *`) are passed as arguments.
The second and fourth arguments of `::EffekseerRendererDX12::Create` pass the render target and depth buffer format `DXGI_FORMAT`.  

**On the sample source, Device etc. are obtained from the device management class (`device`, `DeviceDX11`/`DeviceDX12`) for the sample and set.**  
**When actually incorporating the device, please replace it with an implementation suitable for Your Application/Framework.**  

### 6.5. Configuration of the created drawing module

```cpp
// Specify rendering modules
efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

// Specify a texture, model, curve and material loader
// It can be extended by yourself. It is loaded from a file on now.
efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
```


### 6.6. Setting the Coordinate System

```cpp
// Setup the coordinate system. This must be matched with your application.
efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
```

Set the coordinate system.

In practice, it is necessary to match the coordinate system of the application to which it is introduced.   
But, first implement it in the right-hand coordinate system(`RH`) for the purpose of checking its operation.   
(The coordinate system is described later. (Related: [7. Match camera and coordinate system with application](#7-match-camera-and-coordinate-system-with-application))

### 6.7. Create the projection and camera matrices

```cpp
// Specify a position of view
::Effekseer::Vector3D viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

// Specify a projection matrix
::Effekseer::Matrix44 projectionMatrix;.
PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

// Specify a camera matrix
::Effekseer::Matrix44 cameraMatrix;
cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
```

Creates the projection (projection) and camera matrices.

In practice, this also needs to be synchronized with Your Application.  
But, first create and use a fixed projection/camera matrix in the right-hand coordinate system(RH) for the purpose of checking the operation.

### 6.8. Loading Effects

```cpp
// Load an effect
Effekseer::EffectRef effect;.
effect = Effekseer::Effect::Create(efkManager, u"Laser01.efkefc");
```

Load an effect from the effect file (`.efkefc`) copied in [6.1. Prepare the effect file to be played back](#61-prepare-the-effect-file-to-be-played-back).  
The loaded effect data (`EffectRef`) will be returned.  

`EffectRef` (`effect`) is the data needed to play back the effect.   
It can be used to play back multiple effects from the same `EffectRef`.  
Also, since this is a loading process, the processing time is relatively long.  

So basically, `Effekseer::Effect::Create` only needs to be run once for one pattern of effect file.  

> [!WARNING].
> The macro `EFK_EXAMPLE_ASSETS_DIR_U16` in the sample source is for sample use only.  
> You cannot use it in Your Application.  

> [!NOTE]
> The second argument of the `Effekseer::Effect::Create()` function, `path`(.efkefc file path), is of type `char16_t*`.  
> You cannot use normal string literals (`"ABCDE"`) or normal strings (`char*`,`std::string`) as they are.  
> In the above source, a string literal is made into a string literal of type `char16_t*` by prefixing it with `u`.  

### 6.9. Playback and control of effects


```cpp
// Elapsed frames
int32_t time = 0;

// Used to control the played effect.
Effekseer::Handle efkHandle = 0;

while (device.NewFrame())
{
    // Stop and play effects every 120 frames(for sample)
    if (time % 120 == 0)
    {
        // Play an effect
        efkHandle = efkManager->Play(effect, 0, 0, 0); }
    }

    if (time % 120 == 119)
    {
        // Stop effects
        efkManager->StopEffect(efkHandle); }
    }

    // Move the effect
    efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

    ...

    time++;
    }
}
```

The effect is stopped and played back every 120 frames while moving every frame.

> [!NOTE]
> `while (device.NewFrame()) {}` is the main loop.  
> The main loop is implemented during `while (device.NewFrame() {}`, which is the main loop.  
> (The implementation of the main loop is also for the sample. The main loop should be implemented by replacing it with Your Application/Framework.)

The number of elapsed frames is counted by incrementing `time` every frame.

By using `Effekseer::Handle` (`efkHandle`), we can control the effects that are playing, such as moving or stopping.

### 6.10. [DirectX12 only, every frame] Processing at start of frame


```cpp
while (device.NewFrame())
{
  // <<DirectX 12 only: Add the following>>

  // Call on starting of a frame
  efkMemoryPool->NewFrame();

  // Begin a command list
  EffekseerRendererDX12::BeginCommandList(efkCommandList, device.GetCommandList());
  efkRenderer->SetCommandList(efkCommandList);

  ...
````

**Only in DirectX 12, Need to be done every frame.**  
At the beginning of the main loop, start recording the CommandList used for drawing.  
Add this to the beginning of the main loop in [6.9. Playback and control of effects](#69-playback-and-control-of-effects).

Again, GetCommandList is obtained from the device management class (`device`) provided for the sample and set, but this too can be replaced and implemented according to the application to be introduced.  

### 6.11. [Every frame] Manager update

```cpp
// Set layer parameters
Effekseer::Manager::LayerParameter layerParameter;
layerParameter.ViewerPosition = viewerPosition;
efkManager->SetLayerParameter(0, layerParameter);

// Update the manager
Effekseer::Manager::UpdateParameter updateParameter;
efkManager->Update(updateParameter);
```

**This process must be executed every frame.**  

Update LayerParameter (`efkManager->SetLayerParameter`) and Manager (`efkManager->Update`).

When updating the Manager, you can pass `Effekseer::Manager::UpdateParameter` as an argument.  
This allows control over elapsed frames, number of updates, asynchronous processing, etc.  

### 6.12. [Every frame] Renderer update and drawing

```cpp
// Execute functions about DirectX
device.ClearScreen();

// Update a time
efkRenderer->SetTime(time / 60.0f);

// Specify a projection matrix
efkRenderer->SetProjectionMatrix(projectionMatrix);

// Specify a camera matrix
efkRenderer->SetCameraMatrix(cameraMatrix);

// Begin to rendering effects
efkRenderer->BeginRendering();

// Render effects
Effekseer::Manager::DrawParameter drawParameter;
drawParameter.ZNear = 0.0f;
drawParameter.ZFar = 1.0f;
drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
efkManager->Draw(drawParameter);

// Finish to rendering effects
efkRenderer->EndRendering();

// Execute functions about DirectX
device.PresentDevice();
````

**This process must be executed every frame.**

**This should be executed between the start of the drawing process in DirectX and the end of the drawing process.**  
(In the sample, this is executed between `device.ClearScreen();` and `device.PresentDevice();`)

The elapsed time is updated and the projection/camera matrix is set before the drawing process.  

In `efkRenderer->SetTime(time / 60.0f);` the elapsed time is set using `time`, which was created in [6.9. Playback and control of effects](#69-playback-and-control-of-effects).  
In this sample, assuming a frame rate of 60 fps, the `time` is divided by 60.  

### 6.13. [DirectX12 only, every frame] Finish command list

```cpp
// <<DirectX 12 only: Add the following>>
// Implemented immediately after efkRenderer->EndRendering();.

// Finish a command list
efkRenderer->SetCommandList(nullptr);
EffekseerRendererDX12::EndCommandList(efkCommandList);
```

**Need to be done every frame only in DirectX 12.**  
Implement it right after `efkRenderer->EndRendering();` at the end of the main loop in the section [6.12. [Every frame] Renderer update and drawing](#612-every-frame-renderer-update-and-drawing).

### 6.14. [DirectX12 only] Release

```cpp
// time++; // }
// }
// DirectX12 only, implement after the main loop ends

efkCommandList.Reset();
efkMemoryPool.Reset();
efkRenderer.Reset();
```

DirectX12 only, implement the release process.  
It is implemented immediately after the main loop ends.  

### 6.15. Checking operation

Up to this point, the minimum implementation for playing back an effect.

Please try to build and run.

If it has been installed correctly, the effect should play as shown in the figure below.

If it does not play correctly or you get a compile/link error, etc., there may be an error somewhere.   Please check and correct it.  
Please also refer to [9. Troubleshooting](#9-troubleshooting).  

![LaserEffect](images/LaserEffect.png)

## 7. Match camera and coordinate system with application

For now, moving the camera or changing the viewing angle on the application side does not change the position or visibility of the effect.

Add/change the coordinate system to match the camera of the Effekseer and the application so that the effect is drawn in the correct position.

### 7.1. Synchronizing the camera

The projection and camera matrices must be set on the Effekseer side with the camera information for each frame.

The following is an example implementation of synchronizing the projection (projection) matrix and the camera matrix.

**Add the following to [6.12. [Every frame] Renderer update and drawing](#612-every-frame-renderer-update-and-drawing), before the `efkRenderer->SetProjectionMatrix` and `efkRenderer->SetCameraMatrix` calls. SetCameraMatrix`.**  

```cpp
// Add before calling efkRenderer->SetCameraMatrix, efkRenderer->SetProjectionMatrix 

// Effekseer's CameraMatrix refers to the inverse of the camera attitude matrix. 
// If the camera matrix is the camera attitude matrix, it should be inverted.
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

> [!NOTE]
> `appProjectionMatrix` and `invAppCameraMatrix` are the projection (projection) matrix and camera matrix on the application side.
> Also, this is an example implementation when it is a `DirectX::SimpleMath::Matrix` type of DirectXTK.
> Please replace and implement it according to the application you are introducing.

> [!IMPORTANT]
> Effekseer's CameraMatrix refers to **the inverse of the camera's attitude matrix**.
> On the other hand, many frameworks refer to the camera's attitude matrix as CameraMatrix and its inverse as ViewMatrix.
> Therefore, in the example implementation above, the inverse of CameraMatrix is set.

### 7.2. Match the coordinate system with the application

There are two types of coordinate systems for representing 3D space: right-handed (RH) and left-handed (LH).

This coordinate system must match the application.  

**If Your Application is left-handed, it is necessary to set Effekseer to a left-handed coordinate system during initialization.**  
**By modifying the call to `m_efkManager->SetCoordinateSystem` implemented in [6.6. Setting the Coordinate Systemn](#66-setting-the-coordinate-system) as follows, you can switch to the left-handed (LH) coordinate system.**  

(If Your Application is a right-handed coordinate system, there is no problem leaving the sample as it is (RH)).  

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
> ![CoordinateSystem](images/HelpCpp_CoordinateSystem_En.png)

### 7.3. Layer Parameter Settings

In the implementation as shown in the sample, the `layerParameter.ViewerPosition` is set to a fixed coordinate (`viewerPosition`).

This also needs to be adjusted to the actual camera coordinates.

The following is an implementation example of setting `layerParameter.ViewerPosition` according to the actual camera coordinates.  
**Add and change the corresponding code in [6.11. [Every frame] Manager update](#611-every-frame-manager-update)**.

```cpp
// Set layer parameters
Effekseer::Manager::LayerParameter layerParameter;
::Effekseer::Matrix44 invEfkCameraMatrix;
::Effekseer::Matrix44::Inverse(invEfkCameraMatrix, cameraMatrix);
layerParameter.ViewerPosition = ::Effekseer::Vector3D(invEfkCameraMatrix.Values[3][0], invEfkCameraMatrix.Values[3][1],. invEfkCameraMatrix.Values[3][2]);.
```

The camera coordinates are taken from the Effekseer camera matrix `cameraMatrix` that was synchronized with the application earlier in [7.1. Synchronizing the camera](#71-synchronizing-the-camera)  


## 8. [Appendix] Basic Control of Effects

This section explains the fundamentals of controlling effects.

> [!NOTE]
> - `Effekseer::EffectRef` is a reference to the data of an effect loaded with `Effekseer::Effect::Create`. It is used to start playing the effect.  
> - `Effekseer::EffectHandle` is the identification number of each and every instance of the played effect. It is used to perform operations on the played effects. It can be retrieved, for example, by the return value of a playback with `efkManager->Play`.  
>
> ![EffectRefHandle_Explain](images/EffectRefHandle_Explain_En.png)

### 8.1. Playing back effects

Play the effect with `efkManager->Play`.  
You can also specify the position of the effect as an argument.

**The return value `Effekseer::Handle` can be used to control the played back effect instance**.

```cpp
Effekseer::Handle efkHandle = 0;
// set the coordinates where you want the effect to appear
efkHandle = efkManager->Play(effect, 0.0f, 0.0f, 0.0f);
```

### 8.2. Moving effects

With `efkManager->SetLocation`, move the effect instance to the desired coordinates.  

```cpp
efkManager->SetLocation(efkHandle, 1.0f, 1.0f, 1.0f);
```

You can also use ``efkManager->AddLocation`` to move the effect by the specified coordinates.  
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

### 8.4. Scale the effect

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

### 8.6. Loop playback of the effect 

It can be looped by if the effect has finished playing, playing it again anew.

```cpp
// Check every frame to see if the effect has finished playing.
if (efkManager->Exists(efkHandle) == false)
{
  // Play the effect again. Set the coordinates to the location where you want to display the effect.
  // (position, rotation, and scaling must also be re-set)
  efkHandle = efkManager->Play(efkManager, 0.0f, 0.0f, 0.0f);
}
```

## 9. Troubleshooting

### 9.1. Dialog message: "Windows protected your PC"

Running is prevented by Widnows Defender.

You can run by clicking on `More Information` near the center of the screen and then clicking on `Run anyway` that appears.

![WindowsDefender_protection](images/WindowsDefender_protection_En.png)

### 9.2. CMake build-related problems

Please check or try the following.

- If you clone from the master branch on GitHub, try downloading the for C++ from the Releases page ([https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/Effekseer/releases)).
- Make sure you have the latest version of CMake installed.
- Make sure that the path to CMake is working (if not, make sure that you have installed the latest version of CMake)  
(If not, pass the path by yourself or reinstall CMake. When reinstalling, be sure to check either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user")
- Make sure you have not accidentally built with vulkan presets.

### 9.3. Build Error in Deployed Application

> [!NOTE]
> The error statement shown below is an example.  
> Please read file names such as `Effekseer.h` `Effekseer.lib` and so on as you see fit.  

#### 9.3.1. Failure to open library/include file

Examples of error messages:

- `LNK1104 Cannot open file 'Effekseer.lib'.`
- `E1696 Cannot open source file 'Effekseer.h'`
- `C1083 Cannot open include file '. Effekseer.h':No such file or directory`

If any of these occur, the corresponding file cannot be opened because it cannot be found or for other reasons.  
Please check the following.

- Are the relevant files placed in the correct directories?
- The `library directory` and `include directory` in the project properties are set correctly
- Are the `library directory` and `include directory` in the project properties set for the appropriate project, configuration, and platform?
- Preprocessor statements such as `#include <Effekseer.h>` `#pragma comment(lib, "Effekseer.lib")` are correct

> [!NOTE]
> Project properties apply only to the configuration/platform at the top of the properties page.
>
> Even if you think you have set them correctly, the `library directory` or `include directory` may not be set correctly due to an error in the configuration or > platform you have selected.  
>
> When setting up the `library directory` or `include directory`, be sure to set the proper ` library directory` or `include directory`.  
> ![VisualStudio_ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_En.png)

#### 9.3.2. Failure to link library file

Examples of error messages:

- `LNK2001 unresolved external symbol "public: __thiscall Effekseer::Matrix44::Matrix44(void)"(??0Matrix44@Effekseer@@QAE@XZ)`
- `LNK2019 unresolved external symbol __imp__rand referenced in function "private: static int __cdecl Effekseer::ManagerImplemented::Rand(void)" (?Rand@ManagerImplemented@Effekseer@@CAHXZ)`
- `LNK4272 library machine type 'x86' conflicts with target machine type 'x64'`
- `LNK1112 module machine type 'x64' conflicts with target machine type 'x86'` 
- `LNK2038 mismatch detected for 'RuntimeLibrary': value 'MDd_DynamicDebug' doesn't match value 'MTd_StaticDebug' in GraphEditor.obj`

If either of these occur, the build preset or settings you selected when [3. Build Effekseer For C++](#3-build-effekseer-for-c) is considered incorrect.

Please try follow the steps below to build Effekseer For C++ again with the correct presets and settings.

1. delete the build folder (`build_msvcXXXX_XXXX`) and the installation folder (`install_msvcXXXXXX_XXXX`) of Effekseer For C++
2. build Effekseer For C++ with the presets and settings suitable for your project ([3. Build Effekseer For C++](#3-build-effekseer-for-c))
3. copy the newly generated library files in the installation folder back to Your Application's project ([4. Copy built files](#4-copy-built-files))
