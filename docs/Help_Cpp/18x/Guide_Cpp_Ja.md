# Effekseer For C++ 導入ガイド（DirectX 11/12）

Effekseer For C++を、DirectX 11/12を使用したWindowsゲーム/アプリケーションに組み込むためのチュートリアルです。  

このチュートリアルは以下の環境を対象にしています。  
環境の違いによる差異は、適宜読み替えてください。  

- Windows 10/11
- Microsoft Visual Studio 2022
- DirectX 11/12
- x86/x64

> **Note**  
> **ライブラリ導入に不慣れな方は、事前にプロジェクトのバックアップを取ることを推奨します。**  
> 導入時の操作を間違えると、アプリケーションのプロジェクト設定に異常が発生する可能性があります。  

> **Note**  
> 本ドキュメントでは、導入先のゲーム/アプリケーションのことを「アプリケーション」と呼称します。  

> **Note**  
> 本ドキュメントのサンプルソースは、アプリケーション内での算術ライブラリをDirextX ToolKit(`DirectX::SimpleMath`)として記述しています。必要に応じて読み替えてください。    

## **重要：導入先のアプリケーションの環境を確認する**

導入先となる、あなたのアプリケーションのプロジェクト設定を確認します。  

以下の４つを確認してください。  
以降のビルド・導入で必要になります。  

- VisualStudioのバージョン（例：Visual Studio 2022）
- DirectXのバージョン（DirectX 11 or 12）
- プラットフォーム（x86 or x64）
- プロジェクト設定の「ランタイムライブラリ」設定（マルチスレッドデバッグ or マルチスレッドデバッグDLL）

> **Note**  
> DXライブラリを使用している場合は、Effekseer For C++ではなく、Effekseer For DXライブラリを利用してください。

### プラットフォームとランタイムライブラリ設定の確認方法

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
> 「アクティブ」と書かれているプラットフォームが、あなたが最後にビルド・実行時に使用したプラットフォーム設定です。確認してください。  

> **Note**  
> 多くの場合、`Win32` `x86` `x64`などが設定されています。  
 
4. ソリューションエクスプローラーの、 構成プロパティ > C/C++ > コード生成を選択します。

5. **画面左上の「構成(C)」を`Debug`に設定してから**、「ランタイムライブラリ」を確認してください。  
6. つづいて、**画面左上の「構成(C)」を`Release`に設定してから**、「ランタイムライブラリ」を確認してください。  

> **Note**  
> 多くの場合、構成ごとに異なる設定がされているため、「Debug」「Release」それぞれの設定を確認しておきます。  

> **Note**  
> `マルチスレッド (/MT)` `マルチスレッド デバッグ (/MTd)` `マルチスレッド DLL (/MD)` `マルチスレッド デバッグ DLL (/MDd)` のいずれかに設定されています。  

## CMakeのインストール

Effekseer For C++には、ビルド済みのバイナリは含まれていません。  
そのため、導入前にビルドをする必要があります。  

ビルドをするためには、CMakeというソフトウェアを使います。  

CMakeを公式サイトからダウンロード、インストールしてください。  
[https://cmake.org/download/](https://cmake.org/download/)  

> **Warning**  
> このとき、必ず「**Add CMake to the system PATH for all users**」か「**Add CMake to the system PATH for the current user**」のどちらかにチェックを入れて、PATHを通してください。  
> （環境変数/PATHを自力で設定できる場合は、自力で設定しても問題ありません）  
> ![CMake_InstallOptions](images/CMake_InstallOptions.png)  


## Effekseer For C++のビルド

ビルドを実行するバッチファイルを用意しています。  

環境に合ったバッチファイルを実行してください。  

例えば Visual Studio 2022で32bit（x86）の場合、実行されるバッチファイルは `build_msvc2022_x86.bat`です。


しばらく待つと、installフォルダが作成され、その中に「インクルードファイル」と「ライブラリファイル」が生成されます。  
例えば Visual Studio 2022で32bit（x86）の場合、生成されるinstallフォルダは `install_msvc2022_x86` です。  
**以降、このフォルダのことを「インストールフォルダ」と呼びます。**   

> **Warning**  
> vulkanについて  
> vulkanを使っていない場合、**`build_vulkan_xxxxxxxxx.bat`は使用しないでください**。  
> vulkanは、DirectXやOpenGLとは異なるグラフィックスAPIです。  

## ファイルの配置

Effekseerのファイルを、あなたのアプリケーションのプロジェクトで使えるように、プロジェクトディレクトリ内に配置していきます。  


> **Note**  
> ここでは、アプリケーションのプロジェクト（.projファイル）があるディレクトリのことを、「プロジェクトディレクトリ」と呼びます。

### includeファイル（.h）

Effekseer for Cppをビルドしたときに出力されたインストールフォルダ(例：`install_msvc2022_x86/`)から、  
以下のファイルを、`[プロジェクトディレクトリ]/Libraries]`へコピーしてください。  

`[インストールフォルダ]/include/Effekseer` 内
- `Effekseer.h`
- `Effekseer.Modules.h`
- `Effekseer.SIMD.h`

**【DirectX 11の場合のみ】**  
- `[インストールフォルダ]/include/EffekseerRendererDX11/`内のすべてのファイル  

**【DirectX 12の場合のみ】**  
- `[インストールフォルダ]/include/EffekseerRendererDX12/`内のすべてのファイル

### ライブラリファイル（.lib）

TODO: includeディレクトリと同様に書く

> **Note**  
> `xxxxxxd.lib`のように、が末尾についているライブラリファイルは、デバッグビルド構成設定のライブラリファイルです。  

### ファイルの配置を再度チェック
以下のようなファイル/フォルダ構成になっているか、再度確認しておきます。  

DirectX 11の場合
```
[プロジェクトディレクトリ]/Libraries/
└ Effekseer
　 ├ include
　 │ ├ Effekseer.h
　 │ ├ Effekseer.Modules.h
　 │ ├ Effekseer.SIMD.h
　 │ └ EffekseerRendererDX11.h
　 └ lib
　 　├ Effekseer.lib
　 　├ Effekseerd.lib
　 　├ EffekseerRendererDX11.lib
　 　└ EffekseerRendererDX11d.lib
```

DirectX 12の場合
```
TODO: DirectX12の場合も書く
```

## VisualStudioのプロジェクト設定

### includeディレクトリの設定

includeディレクトリを設定します。  

1. ソリューションエクスプローラーから、あなたのアプリケーションのプロジェクトを右クリックします。  
2. 表示されるメニューから、「プロパティ」を選択して、プロパティページを開きます。
3. **【重要】画面上部の構成とプラットフォームを、それぞれ「すべての構成」「アクティブ」に設定しておきます。**
4. 構成プロパティ > C/C++ > 全般 > 追加のインクルードディレクトリ > 編集 を選択します。
5. 画面右上のフォルダーのアイコンをクリックして、新しい行を挿入できる状態にします。  
6. `Libraries\Effekseer\include`を追記します。
7. 「OK」を押します。

TODO: 画像の内容を文章と一致させる（ライブラリのディレクトリが異なっている）
![VIsualStudio_IncludeDirectory_Ja](images/VIsualStudio_IncludeDirectory_Ja.png)

### ライブラリディレクトリの設定

TODO: includeディレクトリと同様に書く

## アプリケーションでエフェクトを再生できるようにする

使用しているフレームワークなどに合わせて、EffekseerのAPIを呼び出します。  
サンプルを参考にして、実装してください。  

TODO: サンプルはここにありますとか書く

難しいようであれば、まずはサンプルの処理内容を移植することから始めることをお勧めします。  

### サンプルソースの内容

サンプルソースは、以下の構成です。  

TODO: main.cpp以外にも、DeviceのSetupEffekseerModulesも見るように促す。
TODO: 軽く内容を説明する。表とか使う？

### サンプルソースの内容の移植の例


#### includeとpragma文の記述
```cpp
#pragma comment(lib, "Effekseer.lib")

// DirectX 11のときのみ以下をコメントアウトしてください
//#pragma comment(lib, "EffekseerRendererDX11.lib")
//#include <EffekseerRendererDX11.h>

// DirectX 12のときのみ以下をコメントアウトしてください
//#pragma comment(lib, "EffekseerRendererDX12.lib")
//TODO: あとで調べながら書く

#include <Effekseer.h>
```

#### Effekseerの初期化

`Effekseer::Manager`と`EffekseerRenderer`を作成、初期化します。  

#### カメラ行列の作成


#### エフェクトの読み込み

#### エフェクトの再生


#### [毎フレーム実行]Effekseerの更新処理




## カメラを同期させる


今の状態では、アプリケーション側のカメラを動かしたり視野角を変更しても、エフェクトの見え方が変わりません。

毎フレームカメラの情報をEffekseer側にセットする必要があります。  

以下は、ProjectionMatrixとCameraMatrixを同期させるサンプルソースです。  

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
> EffekseerでのCameraMatrixは、**カメラの姿勢行列の逆行列**を指します。  
> 一方で、カメラの姿勢行列のことをCameraMatrix、その逆行列のことをViewMatrixと呼ぶフレームワークもあります。  
> その場合、EffekseerのCameraMatrixにはViewMatrixをセットしてください。  

## 座標系の設定

3D空間を表現するための座標系には、右手系（RH）と左手系（LH）の２種類があります。  

この座標系を、アプリケーションと一致させる必要があります。  

Effekseerの座標系はデフォルトでは右手系（RH）です。  
**あなたのアプリケーションが左手系であれば、Effekseerの初期化時に左手系座標系に変更する必要があります。**  

```cpp
...
// エフェクトのマネージャーの作成
m_efkManager = ::Effekseer::Manager::Create(8000);
// 追加: 左手座標系に変更する
m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
...
```


## エフェクトの基本的な制御

> **Note** `Effekseer::EffectRef`と`Effekseer::EffectHandle`について  
> TODO: 図解する

### エフェクトを再生する

### エフェクトを移動させる

### エフェクトを停止させる



## トラブルシューティング（うまく動かないとき）

### CMakeエラー

以下をお試しください。  

- GitHubのmasterブランチからcloneしている場合、Releasesページ([https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/Effekseer/releases))からfor Cppをダウンロードしたものをお試しください
- CMakeの最新バージョンがインストールされているか
- 誤って`build_vulkan_xxxxxxxxx.bat`を実行していないか

### 導入先アプリケーションでのビルドエラー例

> **Note**  
> `Effekseer.lib`などのファイル名は、あくまでも一例です。適宜読み替えてください。  

#### 「エラー	LNK1104	ファイル 'Effekseer.lib' を開くことができません。」


libファイルが見つからない等の理由で、開くことができない状態です。  
以下を確認してください。

- libファイルが正しいディレクトリに置かれているか
- ライブラリディレクトリが適切に設定されているか
- `#pragma comment(lib, "Effekseer.lib")`などのプリプロセッサ文にタイプミスなどが無いか

#### 「エラー	E1696	ソース ファイルを開けません "Effekseer.h"」「エラー	C1083	include ファイルを開けません。'Effekseer.h':No such file or directory」

ソースファイルが見つからない等の理由で、開くことができない状態です。  
以下が正しいか確認してください。

- ソースファイルのディレクトリ
- Project設定の`Includeディレクトリ`
- `#include <Effekseer.h>`などのプリプロセッサ文

#### TODO: プロジェクト設定の差異に起因するエラーについても書いておく


