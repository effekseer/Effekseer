How to build
==========

Requirements
----------

### Common

- git
- [.Net9.0 SDK](https://dotnet.microsoft.com/ja-jp/download/dotnet/9.0)
- cmake (3.23 or later)
- python3 (3.12 or later)

### Windows

- git-lfs [Download](https://git-lfs.github.com/)
- Visual Studio 2022 or later

### macOS

- Xcode
- Visual Studio for Mac (msbuild)

[How to install git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)

### Linux

- g++

- Required packages:
  - Ubuntu: `libx11-dev libgl1-mesa-dev libxrandr-dev libxi-dev x11proto-xf86vidmode-dev libx11-xcb-dev xorg-dev libglu1-mesa-dev libpulse-dev libgtk-3-dev libopenal-dev python3-distutils zenity`
  - Fedora 40: `gtk3-devel pulseaudio-libs-devel openal-soft-devel mesa-libGLU-devel libX11-devel libxcb-devel libXi-devel libXrandr-devel mesa-libGL-devel python3 dotnet zenity ninja-build`

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

Build to develop the Editor
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

Execute the following command at the repository root.

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

Run the tests
-------------

After building you can run the included unit test suite from the repository root with:

```
python3 -m unittest Script/test_screenshot.py -v
```

This command checks generated screenshots against the reference data and reports
the results.

## How to release

* Windows

Visual Studio 2022 is required.

Please read ``` .github/workflows/build.yml ```

* Mac

Please read ``` .github/workflows/build.yml ```

* Linux

Please read ``` .github/workflows/build.yml ```

The C++ runtime package can be created on any platform using:

```
python3 release_cpp.py
```
