# WebGPU Build and Test

This document describes the current local build and test flow for Effekseer's WebGPU backend.
Run the commands from the Effekseer repository root unless a step says otherwise.

## Native Runtime

Configure a native WebGPU runtime test build:

```powershell
cmake -S . -B build_webgpu_runtime_test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

Build `TestCpp`:

```powershell
cmake --build build_webgpu_runtime_test --target TestCpp --config Debug
```

Run a minimal WebGPU update smoke test:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUUpdateSimpleTest
```

Run the focused native WebGPU checks:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUModelColorTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUScreenshotSmokeTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUTexturelessDistortionVisibilityTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUMaterialUVTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUCompiledMaterialTest
```

`Runtime.WebGPUTexturelessDistortionVisibilityTest` uses a non-periodic gradient background and verifies that the textureless upper half of `Distortions1.efk` changes the rendered WebGPU frame. This catches regressions that can be hidden by the default checkered background.

Run the broader native WebGPU runtime test:

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.BasicRuntimeTestWebGPU
```

## Browser Runtime

The browser WebGPU test requires Emscripten.

Activate emsdk:

```powershell
# Replace <path-to-emsdk> with your local emsdk checkout.
Push-Location "<path-to-emsdk>"
.\emsdk.ps1 activate latest
.\emsdk_env.ps1
Pop-Location
```

Configure the browser WebGPU test build:

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

To test the WebAssembly SIMD build, add `-DBUILD_WEBGPU_BROWSER_SIMD=ON`:

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser_simd -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_WEBGPU_BROWSER_SIMD=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

To test WebAssembly SIMD plus pthreads, also enable browser threads:

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser_threads -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_WEBGPU_BROWSER_SIMD=ON -DBUILD_WEBGPU_BROWSER_THREADS=ON -DBUILD_WEBGPU_BROWSER_PTHREAD_POOL_SIZE=4 -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

When `BUILD_WEBGPU_BROWSER_THREADS=ON`, the CTest browser runner serves the generated files with COOP/COEP headers so `SharedArrayBuffer` is available.

Build the browser test target:

```powershell
cmake --build build_effekseer_webgpu_browser --target TestCpp
```

Run the browser WebGPU tests:

```powershell
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

The browser test initializes a real browser WebGPU device through Emscripten, renders effects, reads back the presented frame, and verifies that rendering changed the background-only frame. The suite currently covers:

- `Runtime.WebGPUBrowserSimpleEffectPresentation`
- `Runtime.WebGPUBrowserDistortionPresentation`

If headless Chrome or Edge exits before WebGPU test execution, rerun with non-headless Edge:

```powershell
$env:LLGI_WEBGPU_HEADLESS = "0"
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

The runner auto-detects common Chrome and Edge install locations on Windows.
If the browser is installed elsewhere, set `CHROME_PATH` or `EDGE_PATH` before running the test:

```powershell
$env:CHROME_PATH = "C:\Program Files\Google\Chrome\Application\chrome.exe"
# or, for Microsoft Edge:
$env:EDGE_PATH = "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
```

## Browser Player

Build the browser player:

```powershell
cmake --build build_effekseer_webgpu_browser --target EffekseerWebGPUBrowserPlayer
```

Open an effect in the browser player:

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/SimpleLaser.efk --width=640 --height=360 --loopFrame=180
```

For a pthreads build, add `--cross-origin-isolated` to the player runner command.

Try a distortion effect:

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/Distortions1.efk --width=640 --height=360 --loopFrame=180
```

Try a model effect:

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/14/Model_Parameters1.efk --width=640 --height=360 --loopFrame=180
```

The player script serves the generated files over localhost and opens a WebGPU-capable Chrome or Edge instance.

## LLGI WebGPU

Configure LLGI's WebGPU test build:

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

By default LLGI uses `Dev/Cpp/3rdParty/LLGI/thirdparty/dawn` when it exists.
To use a Dawn checkout outside the repository, add `-DWEBGPU_DAWN_SOURCE_DIR=<path-to-dawn>`.

Build `LLGI_Test`:

```powershell
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-test --target LLGI_Test --config Debug
```

Run all LLGI WebGPU tests:

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu
```

Run the focused triangle smoke test:

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu --filter=SimpleRender.BasicTriangle
```

## Shader Regeneration

Build LLGI's shader transpiler:

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-tool -DBUILD_WEBGPU=ON -DBUILD_TOOL=ON -DBUILD_TEST=OFF -DBUILD_EXAMPLE=OFF
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-tool --target ShaderTranspiler --config Release
```

Regenerate Effekseer WebGPU shaders:

```powershell
$env:SHADER_TRANSPILER = ".\Dev\Cpp\3rdParty\LLGI\build-webgpu-tool\tools\ShaderTranspiler\Release\ShaderTranspiler.exe"
python Dev\Cpp\EffekseerRendererWebGPU\EffekseerRendererWebGPU\compile.py
```

Generated WebGPU shader files are stored under:

- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/Shader`
- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/ShaderHeader`

## Sanity Checks

Before handoff, run:

```powershell
git diff --check
git -C Dev/Cpp/3rdParty/LLGI diff --check
```

Line-ending warnings can appear for existing browser helper files. Treat whitespace errors as blockers, but line-ending warnings alone are not necessarily WebGPU implementation failures.

## Notes

- Browser WebGPU tests must be served from localhost or HTTPS.
- The browser flow uses the preinitialized WebGPU device provided by Emscripten.
- `BUILD_WEBGPU_BROWSER_TEST=ON` intentionally skips native-only test dependencies such as GLFW, OpenGL, OpenAL, Vulkan/glslang compiler projects, and screenshot tools.
- Browser playback currently uses precompiled fixed WebGPU shaders and existing effect data. Runtime WebGPU material compilation is excluded from this browser test target.
- Generated screenshots such as `Smoke_*.png`, `Model_Parameters1_*_ColorCheck.png`, and `TexturelessDistortion_NonPeriodicGradient_WebGPU.png` are local verification artifacts unless a visual-reference update is explicitly intended.
