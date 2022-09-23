How to build
==========

Requirements
----------

### Windows

- git
- git-lfs [Download](https://git-lfs.github.com/)
- Visual Studio 2017 or later
- cmake (3.15 or later)
- python3 (3.8 or later)

### macOS

- git
- git-lfs
- Xcode
- dotnet-core
- Visual Studio for Mac (msbuild)
- cmake

[How to install git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)

### Linux

- git
- git-lfs
- g++
- [.Net SDK](https://docs.microsoft.com/en-us/dotnet/core/install/linux-ubuntu)

- libx11-dev libgl1-mesa-dev libxrandr-dev libxi-dev x11proto-xf86vidmode-dev libx11-xcb-dev xorg-dev libglu1-mesa-dev libpulse-dev libgtk-3-dev libopenal-dev python3-distutils zenity

Get code and resources
----------

Clone and update submodule.

### Terminal

```
git clone https://github.com/effekseer/Effekseer
cd Effekseer
git submodule update --init
git lfs install
git lfs pull
```

Build to develop Editor
----------

### Windows

Click ``` Script/GenerateProjects.bat ```

Open ``` build/Effekseer.sln ```

#### CUI

If you'd like to generate a project with CUI, please execute following command at the repository root.

```
mkdir build
cd build
cmake -DBUILD_VIEWER=ON -DBUILD_EDITOR=ON .. 
```

### macOS

Execure following command at the repository root.

```
mkdir build
cd build
cmake -G Xcode -DBUILD_VIEWER=ON ..
```

Open ``` build/Effekseer.xcodeproj ``` (C++)

Open ``` Dev/Editor/Effekseer.sln ``` (C#)


Build with a script
----------

Execute following command at the repository root.

### Windows

```
python3 build.py
cd Dev/release
Effekseer.exe
```

### macOS

```
python3 build.py
cd Dev/release
mono Effekseer.exe
```

### Linux

```
python3 build.py
cd Dev/release
./Effekseer
```

## How to release

* Windows

Visual Studio 2017 are required.

Please read ``` .github/workflows/build.yml ```

* Mac

Please read ``` .github/workflows/build.yml ```

* Linux

Please read ``` .github/workflows/build.yml ```
