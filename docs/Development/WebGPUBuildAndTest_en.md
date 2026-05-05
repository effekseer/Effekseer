# WebGPU Build and Test

This document describes how to build and test Effekseer's WebGPU backend.

## Native WebGPU Runtime Test

Configure a native WebGPU runtime test build:

```powershell
cmake -S . -B build_webgpu_runtime_test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

Build `TestCpp`:

```powershell
cmake --build build_webgpu_runtime_test --target TestCpp --config Debug
```

Run a small WebGPU smoke test:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUUpdateSimpleTest
```

Run the basic WebGPU rendering test:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.BasicRuntimeTestWebGPU
```

Run focused WebGPU tests:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUModelColorTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUScreenshotSmokeTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUMaterialUVTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUCompiledMaterialTest
```

## Browser WebGPU Test

The browser test requires Emscripten.

Activate emsdk:

```powershell
& D:\ProgramFiles\emsdk\emsdk.ps1 activate latest
& D:\ProgramFiles\emsdk\emsdk_env.ps1
```

Configure the browser WebGPU test build:

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

Build the browser test target:

```powershell
cmake --build build_effekseer_webgpu_browser --target TestCpp
```

Run the browser WebGPU effect presentation test:

```powershell
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

This test launches Chrome or Edge in headless mode, initializes browser WebGPU,
renders `TestData/Effects/10/SimpleLaser.efk`, reads back the presented frame,
and verifies that the rendered frame differs from the background-only frame.

## Browser Effect Player

Build the browser player:

```powershell
cmake --build build_effekseer_webgpu_browser --target EffekseerWebGPUBrowserPlayer
```

Open the default effect in a browser:

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/SimpleLaser.efk --width=640 --height=360 --loopFrame=180
```

Open another existing effect by changing the effect path:

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/14/Model_Parameters1.efk --width=640 --height=360 --loopFrame=180
```

The player script serves the generated files over localhost and opens a
WebGPU-capable Chrome or Edge instance. If the browser is installed in a custom
location, set one of these environment variables before running the script:

```powershell
$env:CHROME_PATH = "C:\Program Files\Google\Chrome\Application\chrome.exe"
$env:EDGE_PATH = "C:\Program Files\Microsoft\Edge\Application\msedge.exe"
```

## LLGI WebGPU Test

Configure LLGI's WebGPU test build:

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DWEBGPU_DAWN_SOURCE_DIR=E:/Dev/_Effekseer/Effekseer/Dev/Cpp/3rdParty/LLGI/thirdparty/dawn
```

Build `LLGI_Test`:

```powershell
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-test --target LLGI_Test --config Debug
```

Run LLGI WebGPU tests:

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu
```

## Shader Regeneration

Build LLGI's shader transpiler:

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-tool -DBUILD_WEBGPU=ON -DBUILD_TOOL=ON -DBUILD_TEST=OFF -DBUILD_EXAMPLE=OFF
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-tool --target ShaderTranspiler --config Release
```

Regenerate Effekseer WebGPU shaders:

```powershell
$env:SHADER_TRANSPILER = "E:\Dev\_Effekseer\Effekseer\Dev\Cpp\3rdParty\LLGI\build-webgpu-tool\tools\ShaderTranspiler\Release\ShaderTranspiler.exe"
python Dev\Cpp\EffekseerRendererWebGPU\EffekseerRendererWebGPU\compile.py
```

Generated WebGPU shader files are stored under:

- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/Shader`
- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/ShaderHeader`

## Notes

- Browser WebGPU tests must be served from localhost or HTTPS.
- The browser flow uses the preinitialized WebGPU device provided by Emscripten.
- `BUILD_WEBGPU_BROWSER_TEST=ON` intentionally skips native-only test dependencies such as GLFW, OpenGL, OpenAL, and screenshot tools.
- Browser playback currently uses precompiled fixed WebGPU shaders and existing effect data; runtime WebGPU material compilation is excluded from this browser test target.
