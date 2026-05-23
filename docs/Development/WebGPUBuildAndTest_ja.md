# WebGPU のビルドとテスト

このドキュメントでは、Effekseer の WebGPU バックエンドをローカルでビルドしてテストする最新手順を説明します。
特に記載がない限り、コマンドは Effekseer リポジトリのルートから実行します。

## ネイティブランタイム

ネイティブ WebGPU ランタイムテスト用のビルドを構成します。

```powershell
cmake -S . -B build_webgpu_runtime_test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

`TestCpp` をビルドします。

```powershell
cmake --build build_webgpu_runtime_test --target TestCpp --config Debug
```

最小の WebGPU 更新スモークテストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUUpdateSimpleTest
```

重点的なネイティブ WebGPU テストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUModelColorTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUScreenshotSmokeTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUTexturelessDistortionVisibilityTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUMaterialUVTest
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.WebGPUCompiledMaterialTest
```

`Runtime.WebGPUTexturelessDistortionVisibilityTest` は非周期グラデーション背景を使い、`Distortions1.efk` の上半分にあるテクスチャなし歪みが WebGPU の描画結果を変化させることを確認します。通常のチェック柄背景では見えにくい回帰を検出するためのテストです。

より広いネイティブ WebGPU ランタイムテストを実行します。

```powershell
build_webgpu_runtime_test\Dev\Cpp\Test\Debug\TestCpp.exe --filter=Runtime.BasicRuntimeTestWebGPU
```

## ブラウザランタイム

ブラウザ WebGPU テストには Emscripten が必要です。

emsdk を有効化します。

```powershell
# <path-to-emsdk> はローカルの emsdk チェックアウトに置き換えます。
Push-Location "<path-to-emsdk>"
.\emsdk.ps1 activate latest
.\emsdk_env.ps1
Pop-Location
```

ブラウザ WebGPU テスト用のビルドを構成します。

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

WebAssembly SIMD 版をテストする場合は、`-DBUILD_WEBGPU_BROWSER_SIMD=ON` を追加します。

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser_simd -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_WEBGPU_BROWSER_SIMD=ON -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

WebAssembly SIMD と pthreads を併用する場合は、ブラウザスレッドも有効にします。

```powershell
emcmake cmake -S . -B build_effekseer_webgpu_browser_threads -DBUILD_TEST=ON -DBUILD_WEBGPU=ON -DBUILD_WEBGPU_BROWSER_TEST=ON -DBUILD_WEBGPU_BROWSER_SIMD=ON -DBUILD_WEBGPU_BROWSER_THREADS=ON -DBUILD_WEBGPU_BROWSER_PTHREAD_POOL_SIZE=4 -DBUILD_GL=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TOOLS=OFF -DUSE_OPENAL=OFF
```

`BUILD_WEBGPU_BROWSER_THREADS=ON` の場合、CTest のブラウザランナーは生成されたファイルを COOP/COEP ヘッダー付きで配信し、`SharedArrayBuffer` を利用できるようにします。

ブラウザテストターゲットをビルドします。

```powershell
cmake --build build_effekseer_webgpu_browser --target TestCpp
```

ブラウザ WebGPU テストを実行します。

```powershell
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

このテストは Emscripten 経由で実ブラウザの WebGPU デバイスを初期化し、エフェクトを描画して、表示されたフレームを readback します。その後、背景のみのフレームとの差分があることを確認します。現在のブラウザテストは次を含みます。

- `Runtime.WebGPUBrowserSimpleEffectPresentation`
- `Runtime.WebGPUBrowserDistortionPresentation`

ヘッドレス Chrome または Edge が WebGPU テスト実行前に終了する場合は、非ヘッドレス Edge で再実行します。

```powershell
$env:LLGI_WEBGPU_HEADLESS = "0"
ctest --test-dir build_effekseer_webgpu_browser -R Effekseer_WebGPU_Browser --output-on-failure
```

runner は Windows の一般的な Chrome / Edge インストール先を自動検出します。
ブラウザが通常とは異なる場所にインストールされている場合は、実行前に `CHROME_PATH` または `EDGE_PATH` を設定してください。

```powershell
$env:CHROME_PATH = "C:\Program Files\Google\Chrome\Application\chrome.exe"
# Microsoft Edge を使う場合:
$env:EDGE_PATH = "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
```

## ブラウザプレイヤー

ブラウザプレイヤーをビルドします。

```powershell
cmake --build build_effekseer_webgpu_browser --target EffekseerWebGPUBrowserPlayer
```

ブラウザプレイヤーでエフェクトを開きます。

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/SimpleLaser.efk --width=640 --height=360 --loopFrame=180
```

pthreads 版では、プレイヤーランナーのコマンドに `--cross-origin-isolated` を追加してください。

歪みエフェクトを開く例です。

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/10/Distortions1.efk --width=640 --height=360 --loopFrame=180
```

モデルエフェクトを開く例です。

```powershell
node Dev\Cpp\Test\Runtime\browser\run_effekseer_webgpu_player.mjs build_effekseer_webgpu_browser\Dev\Cpp\Test\EffekseerWebGPUBrowserPlayer.html /TestData/Effects/14/Model_Parameters1.efk --width=640 --height=360 --loopFrame=180
```

プレイヤースクリプトは生成されたファイルを localhost で配信し、WebGPU に対応した Chrome または Edge を開きます。

## LLGI WebGPU

LLGI の WebGPU テスト用ビルドを構成します。

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-test -DBUILD_TEST=ON -DBUILD_WEBGPU=ON
```

既定では、存在する場合に `Dev/Cpp/3rdParty/LLGI/thirdparty/dawn` が使われます。
リポジトリ外の Dawn チェックアウトを使う場合は `-DWEBGPU_DAWN_SOURCE_DIR=<path-to-dawn>` を追加してください。

`LLGI_Test` をビルドします。

```powershell
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-test --target LLGI_Test --config Debug
```

LLGI の WebGPU テストをすべて実行します。

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu
```

三角形の重点スモークテストを実行します。

```powershell
Dev\Cpp\3rdParty\LLGI\build-webgpu-test\src_test\Debug\LLGI_Test.exe --webgpu --filter=SimpleRender.BasicTriangle
```

## シェーダー再生成

LLGI の ShaderTranspiler をビルドします。

```powershell
cmake -S Dev/Cpp/3rdParty/LLGI -B Dev/Cpp/3rdParty/LLGI/build-webgpu-tool -DBUILD_WEBGPU=ON -DBUILD_TOOL=ON -DBUILD_TEST=OFF -DBUILD_EXAMPLE=OFF
cmake --build Dev/Cpp/3rdParty/LLGI/build-webgpu-tool --target ShaderTranspiler --config Release
```

Effekseer の WebGPU シェーダーを再生成します。

```powershell
$env:SHADER_TRANSPILER = ".\Dev\Cpp\3rdParty\LLGI\build-webgpu-tool\tools\ShaderTranspiler\Release\ShaderTranspiler.exe"
python Dev\Cpp\EffekseerRendererWebGPU\EffekseerRendererWebGPU\compile.py
```

生成された WebGPU シェーダーファイルは次の場所に出力されます。

- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/Shader`
- `Dev/Cpp/EffekseerRendererWebGPU/EffekseerRendererWebGPU/ShaderHeader`

## 確認項目

引き渡し前に次を実行します。

```powershell
git diff --check
git -C Dev/Cpp/3rdParty/LLGI diff --check
```

既存のブラウザヘルパーファイルでは行末警告が出る場合があります。空白エラーは修正対象ですが、行末警告だけであれば WebGPU 実装の失敗とは限りません。

## 注意点

- ブラウザ WebGPU テストは localhost または HTTPS から配信する必要があります。
- ブラウザ向けの処理では、Emscripten が提供する事前初期化済み WebGPU デバイスを使用します。
- `BUILD_WEBGPU_BROWSER_TEST=ON` では、GLFW、OpenGL、OpenAL、Vulkan/glslang compiler projects、スクリーンショットツールなどのネイティブ専用テスト依存を意図的に除外します。
- ブラウザ再生は、事前コンパイル済みの固定 WebGPU シェーダーと既存エフェクトデータを使用します。このブラウザテストターゲットでは、ランタイム WebGPU マテリアルコンパイルは除外されています。
- `Smoke_*.png`、`Model_Parameters1_*_ColorCheck.png`、`TexturelessDistortion_NonPeriodicGradient_WebGPU.png` などの生成スクリーンショットは、明示的にビジュアルリファレンスを更新する場合を除き、ローカル検証用の成果物です。
