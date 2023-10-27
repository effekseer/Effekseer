# Effekseer For C++ 導入ガイド（DirectX 11/12）

Effekseer For C++を、C++言語とDirectX 11/12を使用したWindowsゲーム/アプリケーションに組み込んで再生するためのガイドです。  

このガイドは以下の環境を対象にしています。  
環境の違いによる差異は、適宜読み替えてください。  

**また、Effekseer for C++バージョン1.8での説明になります。**  

- Windows 10/11
- Microsoft Visual Studio 2022
- DirectX 11/12
- XAudio2(無くても良い)
- x86/x64

> **Note**  
> **ライブラリ導入に不慣れな方は、事前にプロジェクトのバックアップを取ることを推奨します。**  
> 導入時の操作を間違えると、アプリケーションのプロジェクト設定に異常が発生する可能性があります。  

> **Note**  
> 本ドキュメントでは、導入先のゲーム/アプリケーションのことを「アプリケーション」と呼称します。  

> **Note**  
> 本ドキュメントのサンプルソースは、アプリケーション内での算術ライブラリをDirextX ToolKit(`DirectX::SimpleMath`)として記述しています。必要に応じて読み替えてください。    

## 1. **導入先のアプリケーションの環境を確認**

導入先となる、あなたのアプリケーションのプロジェクト設定を確認します。  

**以下の４つを確認してください。  
以降のビルド・導入で必要になります。**  

- **VisualStudioのバージョン**  
（例：Visual Studio 2022）
- **DirectXのバージョン**  
（DirectX 11 or 12）
- **プラットフォーム**  
（x86 or x64）
- **プロジェクト設定の「ランタイムライブラリ」設定**  
（マルチスレッド(デバッグ) or マルチスレッド(デバッグ)DLL）

> **Note**  
> DXライブラリを使用している場合は、Effekseer For C++ではなく、Effekseer For DXライブラリを利用してください。

### 1.1. プラットフォームとランタイムライブラリ設定の確認方法

> **Note**  
> 確認方法が分かる方は、本項を読み飛ばしても問題ありません。

Visual Studioのプロジェクトのプロパティ画面にて確認することができます。  

以下は、「Debug」「Release」の２つの構成があるときの確認方法の例です。  

1. ソリューションエクスプローラーで、アプリケーションのプロジェクトを右クリックします。

2. 表示されるメニューから、「プロパティ」を選択して、プロパティページを開きます。  
（図のソリューション/プロジェクトの名称や構成は一例です）  
![VisualStudio_OpenSolution](images/VisualStudio_OpenSolution_Ja.png)  

> **Note**  
> プロジェクトが複数ある場合は、Effekseerを使いたいプロジェクトのプロパティを開いてください。 
 
3. プラットフォームは、画面上部の「プラットフォーム(P)」から確認することができます。  
![VisualStudio_Platform](images/VisualStudio_Platform_Ja.png)  
> **Note**  
> 「アクティブ」と書かれているプラットフォームが、あなたが現在使用中のプラットフォーム設定です。  

> **Note**  
> 多くの場合、`Win32` `x86` `x64`などが設定されています。  
 
1. ソリューションエクスプローラーの、 構成プロパティ > C/C++ > コード生成を選択します。

2. **画面左上の「構成(C)」を`Debug`に設定してから**、「ランタイムライブラリ」を確認してください。  
3. つづいて、**画面左上の「構成(C)」を`Release`に設定してから**、「ランタイムライブラリ」を確認してください。  

> **Note**  
> 多くの場合、構成ごとに異なる設定がされているため、「Debug」「Release」それぞれの設定を確認しておきます。  

> **Note**  
> `マルチスレッド (/MT)` `マルチスレッド デバッグ (/MTd)` `マルチスレッド DLL (/MD)` `マルチスレッド デバッグ DLL (/MDd)` のいずれかに設定されています。  

## 2. CMakeのインストール

Effekseer For C++には、ビルド済みのバイナリは含まれていません。  
そのため、導入前にビルドをする必要があります。  

ビルドをするためには、CMakeというソフトウェアを使います。  

CMakeを公式サイトからダウンロード、インストールしてください。  
[https://cmake.org/download/](https://cmake.org/download/)  

**このとき、必ず「Add CMake to the system PATH for all users」か「Add CMake to the system PATH for the current user」のどちらかにチェックを入れて、PATHを通してください。**  
(環境変数/PATHを自力で設定できる場合は、自力で設定しても問題ありません)  
![CMake_InstallOptions](images/CMake_InstallOptions.png)  

> **Note**  
> Windows Defenderなどのセキュリティソフトによって、起動が停止されることがあります。  
> 関連: [「WindowsによってPCが保護されました」と表示される](#101-windowsによってpcが保護されましたと表示される)

## 3. Effekseer For C++のビルド

### 3.1. ダウンロード

まず、Effekseer for C++(`EffekseerForCppXXX.zip`)をダウンロード、解凍してください。  

[https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/Effekseer/releases)  

### 3.2. ビルド

ビルドを実行するバッチファイルを用意しています。  

`build_msvc.bat`を実行してください。  

> **Note**  
> Windows Defenderなどのセキュリティソフトによって、起動が停止されることがあります。  
> 関連: [「WindowsによってPCが保護されました」と表示される](#101-windowsによってpcが保護されましたと表示される)

実行すると、コンソールが表示されます。  
以下は、表示される内容の例です。(表示されるプリセットは、Effekseer for C++のバージョンによって異なる場合があります。)  

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

表示されているプリセットから、  
 [導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認した、`VisualStudioのバージョン`や`プラットフォーム`に合ったプリセットを見つけてください。  

例えば、Visual Studioバージョンが`2022`、プラットフォームが`x64`ならば、使用するプリセットは`Visual Studio 2022(x64)`です。  

> **Note**  
> プラットフォームが`Win32`である場合は、`(x86)`のプリセットを選択してください。   

> **Warining**  
> vulkanを使っていない場合、**`with Vulkan`が付いたプリセットは使用しないでください**。  
> vulkanは、DirectXやOpenGLとは異なるグラフィックスAPIです。  

適切なプリセットを見つけたら、対応する番号を入力し、Enterキーを押してください。  

つづいて、ランタイムライブラリDLL設定を有効化するかの確認が表示されます。  

```
Enable runtime library DLL option?(y/n):
```

 [導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)**で確認した、`プロジェクト設定の「ランタイムライブラリ」設定`に合わせて、以下のように入力し、Enterキーを押してください。**  
 ビルドが開始します。  

- `マルチスレッド (/MT)`or`マルチスレッド デバッグ (/MTd)`: `n`
- `マルチスレッド DLL (/MD)`or`マルチスレッド デバッグ DLL (/MDd)`: `y`

しばらく待つと、ビルドが完了します。  
インストールフォルダが作成され、その中にインクルードファイルとライブラリファイルが生成されます。  

例えば、Visual Studio 2022でx86の場合、  
生成されるインストールフォルダは `install_msvc2022_x86` です。  
**以降、このフォルダのことを「インストールフォルダ」と呼びます。**   

> **Note**  
> ビルドに失敗した場合は、トラブルシューティングの解決法を確認してください。  
> [トラブルシューティング：cmakeビルド関連でうまく行かない時](#102-cmakeビルド関連でうまく行かない時)

## 4. ライブラリファイルの配置

さきほどEffekseer for Cppをビルドしたときに出力されたEffekseerのライブラリファイルを、導入先のアプリケーションのプロジェクトで使えるように、プロジェクトフォルダ内に配置していきます。  

> **Note**  
> ファイルやフォルダの配置は、分かる方は自由に設定してください。  
> 本ドキュメントでは、一例を紹介します。  

> **Note**  
> **ここでは、アプリケーションのプロジェクト（.projファイル）があるフォルダのことを、「プロジェクトフォルダ」と呼びます。**

### 4.1. ヘッダファイル（.h）の配置

インクルードするための、ヘッダファイル(.h)をコピーします。

Effekseer for Cppをビルドしたときに出力された、**インストールフォルダ(例：`install_msvc2022_x86/`)内のファイル/フォルダーを使います。**  

**`[プロジェクトフォルダ]/Libraries/Effekseer]`へコピーしてください。**  
**フォルダごと、中身のファイルやフォルダを含めてコピーしてください。**  

- `[インストールフォルダ]/include/Effekseer`  
- `[インストールフォルダ]/include/EffekseerRendererCommon`  
- `[インストールフォルダ]/include/EffekseerSoundXAudio2`

**DirectX 11の場合のみ**  
- `[インストールフォルダ]/include/EffekseerRendererDX11`  

**DirectX 12の場合のみ**  
- `[インストールフォルダ]/include/EffekseerRendererDX12`  

### 4.2. ライブラリファイル（.lib）の配置

つづいて、ライブラリファイル(.lib)をコピーします。  

さきほどEffekseer for Cppをビルドしたときに出力されたインストールフォルダ(例：`install_msvc2022_x86/`)から、  
**`[プロジェクトフォルダ]/Libraries/Effekseer]`へ、以下のファイルをコピーしてください。**  

- `Effekseer.lib`
- `Effekseerd.lib`
- `EffekseerRendererCommon.lib`
- `EffekseerRendererCommond.lib`

**DirectX 11の場合のみ**  
- `EffekseerRendererDX11.lib`
- `EffekseerRendererDX11d.lib`

**DirectX 12の場合のみ**  
- `EffekseerRendererDX12.lib`
- `EffekseerRendererDX12d.lib`

> **Note**  
> `xxxxxxd.lib`のように、`d`が末尾についているライブラリファイルは、Debugビルド構成設定のライブラリファイルです。  

### 4.3. ファイルの配置を再度チェック
以下のようなファイル/フォルダ構成になっているか、再度確認しておきます。  

DirectX 11の場合
```
[プロジェクトディレクトリ] (導入先プロジェクト(.projファイル)の場所)
└ Libraries/
　└ Effekseer/
　 　├ Include/
　 　│ ├ Effekseer/
　 　│ │ └ ...
　 　│ ├ EffekseerSoundXAudio2/
　 　│ │ └ ...
　 　│ ├ EffekseerRendererCommon/
　 　│ │ └ ...
　 　│ └ EffekseerRendererDX11/
　 　│   └ ...
　 　└ Lib/
　 　　├ Effekseer.lib
　 　　├ Effekseerd.lib
　 　　├ EffekseerSoundXAudio2.lib
　 　　├ EffekseerSoundXAudio2d.lib
　 　　├ EffekseerRendererCommon.lib
　 　　├ EffekseerRendererCommond.lib
　 　　├ EffekseerRendererDX11.lib
　 　　└ EffekseerRendererDX11d.lib
```

DirectX 12の場合
```
TODO: DirectX12の場合も書く
```

## 5. 導入先アプリケーションのVisualStudioプロジェクト設定

導入先アプリケーションのアプリケーションが、Effekseer for C++ライブラリを扱えるようにプロジェクト設定をします。  
「インクルードディレクトリ」と「ライブラリディレクトリ」の２つを設定します。  

**ここからは、導入先のアプリケーションでの作業です。**  
**導入先のアプリケーションの、VisualStudioのソリューション（プロジェクト）を開いてください。**   

VisualStudioで、導入先のプロジェクトのプロパティを開きます。  

1. ソリューションエクスプローラーから、導入先のアプリケーションのプロジェクトを右クリックします。  
2. 表示されるメニューから、「プロパティ」を選択して、プロパティページを開きます。

### 5.1. インクルードディレクトリの設定

インクルードファイルを配置した場所を登録するため、インクルードディレクトリを設定します。  

1. **画面上部の構成を「すべての構成」、プラットフォームを[導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認したプラットフォームに設定しておきます。**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
1. 構成プロパティ > C/C++ > 全般 > 追加のインクルードディレクトリ > 編集 を選択します。  
![VisualStudio_IncludeDirectory_Ja](images/VisualStudio_IncludeDir_Ja.png)
1. 以下を追記します。  
```
.\Libraries\Effekseer\Include\Effekseer
.\Libraries\Effekseer\Include\EffekseerSoundXAudio2
.\Libraries\Effekseer\Include\EffekseerRendererCommon
```
**DirectX 11の場合のみ**  
```
.\Libraries\Effekseer\Include\EffekseerRendererDX11
```
**DirectX 12の場合のみ**   
```
.\Libraries\Effekseer\Include\EffekseerRendererDX12
```
![VisualStudio_IncludeDirAppend](images/VisualStudio_IncludeDirAppend_Ja.png)   
1. 「OK」を押します。

### 5.2. ライブラリディレクトリの設定

同じように、ライブラリファイルも配置した場所を設定します。  

1. **画面上部の構成を「すべての構成」、プラットフォームを[導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認したプラットフォームに設定しておきます。**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
1. 構成プロパティ > リンカー > 全般 > 追加のライブラリディレクトリ > 編集 を選択します。  
![VisualStudio_LibraryDir](images/VisualStudio_LibraryDir_Ja.png)
1. 以下を追記します。  
```
.\Libraries\Effekseer\Lib
```

![VisualStudio_IncludeDirAppend](images/VisualStudio_LibraryDirAppend_Ja.png)   
4. 「OK」を押します。
   
### 5.3. 適用

インクルードディレクトリとライブラリディレクトリの両方の設定が完了したら、  
プロパティページの右下の「適用(A)」をクリックして、変更を反映させます。  
最後に、「OK」をクリックしてプロパティページを閉じてください。  

## 6. アプリケーションでエフェクトを再生する

EffekseerのAPIをソースコード上から呼び出して、Effekseerの初期化や更新、エフェクトの再生や描画などをします。  

サンプルソースを参考にしながら実装してください。  

DirectX11: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11)  
DirectX12: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12)  


また、以下でサンプルソースの解説をしますので、併せてご参照ください。  

### 6.1. 再生するエフェクトファイルを用意する

TODO: サンプルエフェクトファイル(Laser)を、導入先プロジェクトへコピーしてきてもらう。

### 6.2. サンプルソースの解説

サンプルソースの流れは以下の通りです。  


> **Note**  
> サンプル内の`device`(`DeviceDX11` `DeviceDX12`)は、サンプル用の、グラフィックスやオーディオのデバイスを管理するクラスです。  
> 実際のアプリケーションでは、アプリケーション側(またはフレームワークなど)で管理されます。  
> サンプルソースで、`device`が使用されている箇所は、適宜アプリケーション側の環境に置き換えて実装をしてください。  

#### 6.2.1. includeとpragma文の記述


**<<必ず記述>>**  
```cpp
#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#include <Effekseer.h>
```

**XAudio2使用時（Effekseerを使って音を再生する時）のみ**  
```cpp
#pragma comment(lib, "EffekseerSoundXAudio2.lib")
#include <EffekseerSoundXAudio2.h>
```

**DirectX11使用時のみ**  
```cpp
#pragma comment(lib, "EffekseerRendererDX11.lib")
#include <EffekseerRendererDX11.h>
```
**DirectX12使用時のみ**  
```cpp
#pragma comment(lib, "EffekseerRendererDX12.lib")
#include <EffekseerRendererDX12.h>
//TODO: あとで調べながら書く
```

#### 6.2.2. エフェクトのマネージャーの作成

```cpp
// Create a manager of effects
// エフェクトのマネージャーの作成
::Effekseer::ManagerRef efkManager = ::Effekseer::Manager::Create(8000);
```

`Effekseer::Manager`を作成、初期化します。  

> **Note**  
> `Effekseer::ManagerRef`や`Effekseer::EffectRef`など、Effekseerの型で`Ref`が付くものは、スマートポインタで管理されています。  
> これらは、オブジェクトを使用する変数が無くなると自動で解放されます。  

#### 6.2.3. 描画モジュールの作成・設定

```cpp
// Create a  graphics device
// 描画デバイスの作成
auto graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(device.GetID3D11Device(), device.GetID3D11DeviceContext());

// Create a renderer of effects
// エフェクトのレンダラーの作成
auto efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);

// Sprcify rendering modules
// 描画モジュールの設定
efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

// Specify a texture, model, curve and material loader
// It can be extended by yourself. It is loaded from a file on now.
// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
```

TODO: DirectX12の場合も書く

`::EffekseerRendererDX11::CreateGraphicsDevice()`では、DirextX11のDevice(`ID3D11Device*`)とDeviceContext(`ID3D11DeviceContext*`)を引数として渡します。  

**ここでは、サンプル用のデバイス管理クラス(`device`, `DeviceDX11`/`DeviceDX12`)からDevice等を取得し、セットしています。**  
**アプリケーション/フレームワークに合わせて、置き換えながら実装してください。**

#### 6.2.4. サウンドモジュールの作成(XAudio2使用時のみ)

```cpp
// Specify sound modules
// サウンドモジュールの設定
auto efkSound = ::EffekseerSound::Sound::Create(device.GetIXAudio2(), 16, 16);

// Specify a metho to play sound from an instance of efkSound
// 音再生用インスタンスから再生機能を指定
efkManager->SetSoundPlayer(efkSound->CreateSoundPlayer());

// Specify a sound data loader
// It can be extended by yourself. It is loaded from a file on now.
// サウンドデータの読込機能を設定する。
// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
efkManager->SetSoundLoader(efkSound->CreateSoundLoader());
```

**XAudio2使用時(Effekseerを使って音を再生する時)のみ、実装してください。**  

エフェクトに設定した音を再生するための、サウンドモジュールの作成と設定をします。  

**ここでも、サンプル用に用意されているデバイス管理クラス(`device`, `DeviceDX11`/`DeviceDX12`)から`IXAudio2*`を取得し、セットしています。**  
**アプリケーション/フレームワークに合わせて、置き換えながら実装してください。**

#### 6.2.5. 座標系の設定

```cpp
// Setup the coordinate system. This must be matched with your application.
// 座標系を設定する。アプリケーションと一致させる必要がある。
efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
```

座標系を設定します。  
導入先のアプリケーションの座標系と一致させる必要がありますが、まずは動作確認のため、右手座標系(`LH`)で実装してみてください。  
(座標系については後述します。(関連: [カメラと座標系をアプリケーションと一致させる](#7-カメラと座標系をアプリケーションと一致させる)))

#### 6.2.6. 投影行列とカメラ行列の作成

```cpp
// Specify a position of view
// 視点位置を確定
auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

// Specify a projection matrix
// 投影行列を設定
::Effekseer::Matrix44 projectionMatrix;
projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

// Specify a camera matrix
// カメラ行列を設定
::Effekseer::Matrix44 cameraMatrix;
cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
```

投影(射影)行列とカメラ行列を作成します。  

ここでも、アプリケーションのものと一致(同期)させる必要がありますが、まずは動作確認のため、右手座標系(RH)の固定の投影行列/カメラ行列を作成して使用します。  

#### 6.2.7. エフェクトの読み込み

```cpp
// Load an effect
// エフェクトの読込
auto effect = Effekseer::Effect::Create(efkManager, u"Laser01.efkefc");
```

エフェクトをファイル(`.efkefc`)から読み込みます。  
戻り値として、読み込まれたエフェクトデータ(`EffectRef`)が返ってきます。  

`EffectRef`(`effect`)は、再生中のエフェクトではなく、「エフェクトを再生するために必要なデータ」です。  
同じエフェクトを複数再生するときは、使いまわすことができます。  

また、読み込み処理なので、比較的処理時間が長くなります。  

そのため、 基本的には１種類のエフェクトファイルに対して、１度のみ実行するだけで良いです。  

> **Warning**  
> サンプルソース中の`EFK_EXAMPLE_ASSETS_DIR_U16`は、サンプル専用のマクロです。  
> 導入先のアプリケーションで使うことができません。  

> **Note**  
> `Effekseer::Effect::Create()`関数の第２引数の`path`(.efkefcファイルのパス)は、`char16_t*`型です。  
> 通常の文字列リテラル(`"ABCDE"`)や、通常の文字列(`char*`,`std::string`)をそのまま使うことはできません。  
> 上記のソースでは、文字列リテラルの前に`u`を付けることによって、`char16_t*`型の文字列リテラルにしています。  

#### 6.2.8. エフェクトの再生と制御


```cpp
int32_t time = 0;
Effekseer::Handle efkHandle = 0;

while (device.NewFrame())
{
    // Stop and play effects every 120 frames(for sample)
    // 120フレームごとに、エフェクトを停止、再生する(サンプル用)
    if (time % 120 == 0)
    {
        // Play an effect
        // エフェクトの再生
        efkHandle = efkManager->Play(effect, 0, 0, 0);
    }

    if (time % 120 == 119)
    {
        // Stop effects
        // エフェクトの停止
        efkManager->StopEffect(efkHandle);
    }

    // Move the effect
    // エフェクトの移動
    efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

    ...(中略)

    time++;
}
```

エフェクトを毎フレームを移動させつつ、120フレームごとに停止、再生します。  

> **Note**  
> `while (device.NewFrame()) {}`がゲームループにあたります。  
> この中の処理が、毎フレーム呼び出されるので、毎フレーム実行したい処理はここに実装されています。  
> (ゲームループの実装もサンプル用です。導入先のアプリケーションやフレームワークの実装と置き換えて捉えて実装してください)  

毎フレーム`time`をインクリメントさせることで、経過フレーム数をカウントしています。  

`Effekseer::Handle`(`efkHandle`)を使うことで、再生中のエフェクトの、移動や停止などの制御を行っています。  


#### 6.2.9. [毎フレーム実行]Managerの更新処理

```cpp
// Set layer parameters
// レイヤーパラメータの設定
Effekseer::Manager::LayerParameter layerParameter;
layerParameter.ViewerPosition = viewerPosition;
efkManager->SetLayerParameter(0, layerParameter);

// Update the manager
// マネージャーの更新
Effekseer::Manager::UpdateParameter updateParameter;
efkManager->Update(updateParameter);
```
  
LayerParameterの更新(`efkManager->SetLayerParameter`)と、Managerの更新(`efkManager->Update`)を行います。  

Managerの更新の際には、引数に`Effekseer::Manager::UpdateParameter`を渡すことが出来ます。  
これを使うことで、経過フレームや更新回数、非同期処理などに関する制御が可能です。

#### 6.2.10. [毎フレーム実行]Rendererの更新と描画処理

```cpp
// Execute functions about DirectX
// DirectXの処理
device.ClearScreen();

// Update a time
// 時間を更新する
efkRenderer->SetTime(time / 60.0f);

// Specify a projection matrix
// 投影行列を設定
efkRenderer->SetProjectionMatrix(projectionMatrix);

// Specify a camera matrix
// カメラ行列を設定
efkRenderer->SetCameraMatrix(cameraMatrix);

// Begin to rendering effects
// エフェクトの描画開始処理を行う。
efkRenderer->BeginRendering();

// Render effects
// エフェクトの描画を行う。
Effekseer::Manager::DrawParameter drawParameter;
drawParameter.ZNear = 0.0f;
drawParameter.ZFar = 1.0f;
drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
efkManager->Draw(drawParameter);

// Finish to rendering effects
// エフェクトの描画終了処理を行う。
efkRenderer->EndRendering();

// Execute functions about DirectX
// DirectXの処理
device.PresentDevice();
```

TODO: DirectX12についても書く

**DirectXでの描画処理の開始から、描画処理の終了までの間に行うようにしてください。**  
(サンプルでは`device.ClearScreen();`と`device.PresentDevice();`の間に行っています)  

経過時間の更新や、投影/カメラ行列のセットをしてから、描画処理を行います。  

#### 6.2.11. 動作を確認する

この状態で、ビルド・実行してみてください。  

正しく導入できていれば、下図のようなエフェクトが再生されているはずです。  

もし、正しく再生されなかったり、コンパイル/リンクエラーなどが出た場合には、トラブルシューティングをご確認ください。  

![LaserEffect](images/LaserEffect.png)  

## 7. カメラと座標系をアプリケーションと一致させる

今の状態では、アプリケーション側のカメラを動かしたり視野角を変更しても、エフェクトの位置や見え方が変わりません。  

Effekseerとアプリケーションのカメラと座標系を一致させることで、正しい位置にエフェクトが描画されるようにします。  

### 7.1. カメラを同期させる

投影(射影)行列とカメラ行列は、毎フレームカメラの情報をEffekseer側にセットする必要があります。  

以下は、ProjectionMatrixとCameraMatrixを同期させるサンプルです。  
(アプリケーション側の算術ライブラリは、DirectXTKのSimpleMathとして書いています)  

```cpp
// アプリケーションとEffekseerのカメラ行列を同期
for (int i = 0; i < 4; ++i)
{
    for (int j = 0; j < 4; ++j)
    {
        cameraMatrix.Values[i][j] = appCameraMatrix.m[i][j];
        projectionMatrix.Values[i][j] = appProjectionMatrix.m[i][j];
    }
}
```


> **Warning**  
> EffekseerのCameraMatrixは、**カメラの姿勢行列の逆行列**を指します。  
> 一方で、カメラの姿勢行列のことをCameraMatrix、その逆行列のことをViewMatrixとする場合もあります。  
> その場合、EffekseerのCameraMatrixにはViewMatrixをセットしてください。  
> ```cpp
> // "カメラの姿勢行列"のことを「CameraMatrix」と呼ぶ場合のみ、逆行列化しておく
> auto invAppCameraMatrix = appCameraMatrix.Invert();
> // アプリケーションとEffekseerのカメラ行列を同期
> for (int i = 0; i < 4; ++i)
> {
>     for (int j = 0; j < 4; ++j)
>     {
>         cameraMatrix.Values[i][j] = invAppCameraMatrix.m[i][j];
>     }
> }
> ```
 


### 7.2. 座標系をアプリケーションと一致させる

3D空間を表現するための座標系には、右手系（RH）と左手系（LH）の２種類があります。  

この座標系を、アプリケーションと一致させる必要があります。  

Effekseerの座標系はデフォルトでは右手系（RH）です。  
**あなたのアプリケーションが左手系であれば、Effekseerの初期化時に左手系座標系に設定する必要があります。**  

```cpp
...
m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
...
```

## 8. 付録 エフェクトの基本的な制御

> **Note**   
> `Effekseer::EffectRef`と`Effekseer::EffectHandle`について  
> TODO: 図解する
> ![Alt text](images/EffectRefHandle_Explain.png)

### 8.1. エフェクトを再生する

### 8.2. エフェクトを移動させる

### 8.3. エフェクトを停止させる



## 9. トラブルシューティング（うまく動かないとき）

### 9.1. 「WindowsによってPCが保護されました」と表示される

Widnows Defenderによる表示です。  

画面中央付近の `詳細情報` をクリックしてから、表示される`実行する` ボタンをクリックすることで、実行することができます。  

![WindowsDefender_protection](images/WindowsDefender_protection_Ja.png)  

### 9.2. CMakeビルド関連でうまく行かない時

以下をお試しください。  

- GitHubのmasterブランチからcloneしている場合、Releasesページ([https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/Effekseer/releases))からfor C++をダウンロードしてお試しください
- CMakeの最新バージョンがインストールされているか
- 誤って`build_vulkan_xxxxxxxxx.bat`を実行していないか

### 9.3. 導入先アプリケーションでのビルドエラー

> **Note**  
> `Effekseer.lib`などのファイル名は、あくまでも一例です。適宜読み替えてください。  

#### 9.3.1. 「エラー	LNK1104	ファイル 'Effekseer.lib' を開くことができません。」


libファイルが見つからない等の理由で、開くことができない状態です。  
以下を確認してください。

- libファイルが正しいディレクトリに置かれているか
- ライブラリディレクトリが適切に設定されているか
- `#pragma comment(lib, "Effekseer.lib")`などのプリプロセッサ文にタイプミスなどが無いか

#### 9.3.2. 「エラー	E1696	ソース ファイルを開けません "Effekseer.h"」「エラー	C1083	include ファイルを開けません。'Effekseer.h':No such file or directory」

ソースファイルが見つからない等の理由で、開くことができない状態です。  
以下が正しいか確認してください。

- ソースファイルのディレクトリ
- Project設定の`Includeディレクトリ`
- `#include <Effekseer.h>`などのプリプロセッサ文

#### 9.3.3. TODO: プロジェクト設定の差異に起因するエラーについても書いておく


