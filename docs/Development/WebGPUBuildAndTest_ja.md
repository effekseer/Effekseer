# WebGPU のビルドとテスト

このドキュメントでは、Effekseer の WebGPU バックエンドをビルドしてテストする方法を説明します。

## ネイティブ WebGPU ランタイムテスト

ネイティブ WebGPU ランタイムテスト用のビルドを構成します。

```powershell
cmake -S . -B build_webgpu_runtime_test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

`TestCpp` をビルドします。

```powershell
cmake --build build_webgpu_runtime_test --target TestCpp --config Debug
```

小さな WebGPU スモークテストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUUpdateSimpleTest
```

基本的な WebGPU 描画テストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.BasicRuntimeTestWebGPU
```

個別の WebGPU テストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUModelColorTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUScreenshotSmokeTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUMaterialUVTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUCompiledMaterialTest
```

## ブラウザ WebGPU テスト

ブラウザテストには Emscripten が必要です。

emsdk を有効化します。

```powershell
& D:\ProgramFiles\emsdk\emsdk.ps1 activate latest
& D:\ProgramFiles\emsdk\emsdk_env.ps1
```

ブラウザ WebGPU テスト用のビルドを構成します。

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

ブラウザテストターゲットをビルドします。

```powershell
cmake --build build_effekseer_webgpu_browser --target TestCpp
```

ブラウザ WebGPU のエフェクト表示テストを実行します。

```powershell
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

このテストは Chrome または Edge をヘッドレスモードで起動し、ブラウザ WebGPU を初期化します。
その後、`TestData/Effects/10/SimpleLaser.efk` を描画し、表示されたフレームを readback して、
背景のみのフレームとの差分があることを確認します。

## ブラウザエフェクトプレイヤー

ブラウザプレイヤーをビルドします。

```powershell
cmake --build build_effekseer_webgpu_browser --target EffekseerWebGPUBrowserPlayer
```

デフォルトのエフェクトをブラウザで開きます。

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/SimpleLaser.efk --width=640 --height=360 --loopFrame=180
```

別の既存エフェクトを開く場合は、エフェクトパスを変更します。

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/14/Model_Parameters1.efk --width=640 --height=360 --loopFrame=180
```

プレイヤースクリプトは生成されたファイルを localhost で配信し、WebGPU に対応した Chrome または Edge を開きます。
ブラウザが通常とは異なる場所にインストールされている場合は、実行前に次の環境変数のどちらかを設定してください。

```powershell
$env:CHROME_PATH = "C:\Program Files\Google\Chrome\Application\chrome.exe"
$env:EDGE_PATH = "C:\Program Files\Microsoft\Edge\Application\msedge.exe"
```

## LLGI WebGPU テスト

LLGI の WebGPU テスト用ビルドを構成します。

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DWEBGPU_DAWN_SOURCE_DIR=E:/Dev/_Effekseer/Effekseer/Dev/Cpp/3rdParty/LLGI/thirdparty/dawn
```

`LLGI_Test` をビルドします。

```powershell
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-test --target LLGI_Test --config Debug
```

LLGI の WebGPU テストを実行します。

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu
```

## シェーダー再生成

LLGI の ShaderTranspiler をビルドします。

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-tool -DBUILD_WEBGPU=ON -DBUILD_TOOL=ON -DBUILD_TEST=OFF -DBUILD_EXAMPLE=OFF
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-tool --target ShaderTranspiler --config Release
```

Effekseer の WebGPU シェーダーを再生成します。

```powershell
$env:SHADER_TRANSPILER = "E:\Dev\_Effekseer\Effekseer\Dev\Cpp\3rdParty\LLGI\build-webgpu-tool\tools\ShaderTranspiler\Release\ShaderTranspiler.exe"
python Dev\Cpp\EffekseerRendererWebGPU\EffekseerRendererWebGPU\compile.py
```

生成された WebGPU シェーダーファイルは次の場所に出力されます。

- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/Shader`
- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/ShaderHeader`

## 注意点

- ブラウザ WebGPU テストは localhost または HTTPS から配信する必要があります。
- ブラウザ向けの処理では、Emscripten が提供する事前初期化済み WebGPU デバイスを使用します。
- `BUILD_WEBGPU_BROWSER_TEST=ON` では、GLFW、OpenGL、OpenAL、スクリーンショットツールなどのネイティブ専用テスト依存を意図的に除外します。
- ブラウザ再生は、事前コンパイル済みの固定 WebGPU シェーダーと既存エフェクトデータを使用します。このブラウザテストターゲットでは、ランタイム WebGPU マテリアルコンパイルは除外されています。
