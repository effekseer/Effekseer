# Effekseer For C++ 導入ガイド（DirectX 11/12）

Effekseer For C++を、C++言語とDirectX 11/12を使用したWindowsゲーム/アプリケーションに組み込んで再生するためのガイドです。  

このガイドは以下の環境を対象にしています。  
環境の違いによる差異は、適宜読み替えてください。  

- Windows 10/11
- Microsoft Visual Studio 2019/2022
- DirectX 11/12
- x86/x64
- Effekseer for C++ 1.8

> [!NOTE]  
> **ライブラリ導入に不慣れな方は、事前にプロジェクトのバックアップを取ることを推奨します。**  
> 導入時の操作を間違えると、アプリケーションのプロジェクト設定に異常が発生する可能性があります。  

> [!NOTE]  
> 本ガイドでは、導入先のゲーム/アプリケーションのことを「アプリケーション」と呼称します。  


- [1. 導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)
  - [1.1. プラットフォームとランタイムライブラリ設定の確認方法](#11-プラットフォームとランタイムライブラリ設定の確認方法)
- [2. CMakeのインストール](#2-cmakeのインストール)
- [3. Effekseer For C++のビルド](#3-effekseer-for-cのビルド)
  - [3.1. ダウンロード](#31-ダウンロード)
  - [3.2. ビルド](#32-ビルド)
- [4. ビルドしたライブラリファイルの配置](#4-ビルドしたライブラリファイルの配置)
  - [4.1. ヘッダファイル（.h）の配置](#41-ヘッダファイルhの配置)
  - [4.2. ライブラリファイル（.lib）の配置](#42-ライブラリファイルlibの配置)
  - [4.3. ファイルの配置を再度チェック](#43-ファイルの配置を再度チェック)
- [5. 導入先アプリケーションのVisualStudioプロジェクト設定](#5-導入先アプリケーションのvisualstudioプロジェクト設定)
  - [5.1. インクルードディレクトリの設定](#51-インクルードディレクトリの設定)
  - [5.2. ライブラリディレクトリの設定](#52-ライブラリディレクトリの設定)
  - [5.3. 適用](#53-適用)
- [6. アプリケーションでエフェクトを再生する](#6-アプリケーションでエフェクトを再生する)
  - [6.1. 再生するエフェクトファイルを用意する](#61-再生するエフェクトファイルを用意する)
  - [6.2. includeとpragma文の記述](#62-includeとpragma文の記述)
  - [6.3. エフェクトのマネージャーの作成](#63-エフェクトのマネージャーの作成)
  - [6.4. 描画モジュールの作成](#64-描画モジュールの作成)
  - [6.5. 作成した描画モジュールの設定](#65-作成した描画モジュールの設定)
  - [6.6. 座標系の設定](#66-座標系の設定)
  - [6.7. 投影行列とカメラ行列の作成](#67-投影行列とカメラ行列の作成)
  - [6.8. エフェクトの読み込み](#68-エフェクトの読み込み)
  - [6.9. エフェクトの再生と制御](#69-エフェクトの再生と制御)
  - [6.10. \[DirectX12のみ、毎フレーム実行\]フレーム開始時の処理](#610-directx12のみ毎フレーム実行フレーム開始時の処理)
  - [6.11. \[毎フレーム実行\]Managerの更新処理](#611-毎フレーム実行managerの更新処理)
  - [6.12. \[毎フレーム実行\]Rendererの更新と描画処理](#612-毎フレーム実行rendererの更新と描画処理)
  - [6.13. \[DirectX12のみ、毎フレーム実行\]コマンドリスト終了処理](#613-directx12のみ毎フレーム実行コマンドリスト終了処理)
  - [6.14. \[DirectX12のみ\]解放処理](#614-directx12のみ解放処理)
  - [6.15. 動作を確認する](#615-動作を確認する)
- [7. カメラと座標系をアプリケーションと一致させる](#7-カメラと座標系をアプリケーションと一致させる)
  - [7.1. カメラを同期させる](#71-カメラを同期させる)
  - [7.2. 座標系をアプリケーションと一致させる](#72-座標系をアプリケーションと一致させる)
  - [7.3. レイヤーパラメータの設定](#73-レイヤーパラメータの設定)
- [8. \[付録\] エフェクトの基本的な制御](#8-付録-エフェクトの基本的な制御)
  - [8.1. エフェクトを再生する](#81-エフェクトを再生する)
  - [8.2. エフェクトを移動させる](#82-エフェクトを移動させる)
  - [8.3. エフェクトを回転させる](#83-エフェクトを回転させる)
  - [8.4. エフェクトを拡大縮小させる](#84-エフェクトを拡大縮小させる)
  - [8.5. エフェクトを停止させる](#85-エフェクトを停止させる)
  - [8.6. エフェクトをループ再生する](#86-エフェクトをループ再生する)
- [9. トラブルシューティング（うまく動かないとき）](#9-トラブルシューティングうまく動かないとき)
  - [9.1. 「WindowsによってPCが保護されました」と表示される](#91-windowsによってpcが保護されましたと表示される)
  - [9.2. CMakeビルド関連でうまく行かない時](#92-cmakeビルド関連でうまく行かない時)
  - [9.3. 導入先アプリケーションでのビルドエラー](#93-導入先アプリケーションでのビルドエラー)
    - [9.3.1. ライブラリ/インクルードファイルが開けない](#931-ライブラリインクルードファイルが開けない)
    - [9.3.2. ライブラリのリンクに失敗する](#932-ライブラリのリンクに失敗する)


## 1. 導入先のアプリケーションの環境を確認

正しい構成のEffekseerを導入するため、まずは導入先のアプリケーションのプロジェクト設定を確認します。  

**以下の４つを確認してください。  
以降のビルド・導入で必要になります。**  

- **VisualStudioのバージョン**  
（例：Visual Studio 2022）
- **DirectXのバージョン**  
（DirectX 11 or 12）
- **プラットフォーム**  
（x86/Win32 or x64）
- **プロジェクト設定の「ランタイムライブラリ」設定**  
（マルチスレッド(デバッグ) or マルチスレッド(デバッグ)DLL）

> [!NOTE]  
> DXライブラリを使用している場合は、Effekseer For C++ではなく、Effekseer For DXライブラリを利用してください。  

### 1.1. プラットフォームとランタイムライブラリ設定の確認方法

> [!NOTE]  
> 確認方法が分かる方は、本項を読み飛ばしても問題ありません。

Visual Studioのプロジェクトのプロパティ画面にて確認することができます。  

以下は、「Debug」「Release」の２つの構成があるときの確認方法の例です。  

1. 導入先のアプリケーションのVisualStudioのソリューションを開きます。
2. ソリューションエクスプローラーで、導入先のプロジェクトを右クリックします。  
(プロジェクトが複数ある場合は、Effekseerを使いたいプロジェクトのプロパティを開いてください)   
3. 表示されるメニューから、「プロパティ」を選択して、プロパティページを開きます。  
(図のソリューション/プロジェクトの名称や構成は一例です)  
![VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_Ja.png)

4. プラットフォームは、画面上部の「プラットフォーム(P)」から確認することができます。  
多くの場合、`Win32` `x86` `x64`などが設定されています。  
(「アクティブ」と書かれているプラットフォームが、あなたが現在使用中のプラットフォーム設定です)  
![VisualStudio_Platform](images/VisualStudio_Platform_Ja.png)  
5. 構成プロパティ > C/C++ > コード生成を選択します。

6. **画面左上の「構成(C)」を`Debug`か`Release`のどちらかに設定してから**、「ランタイムライブラリ」を確認してください。  
`マルチスレッド (/MT)` `マルチスレッド デバッグ (/MTd)` `マルチスレッド DLL (/MD)` `マルチスレッド デバッグ DLL (/MDd)` のいずれかに設定されています。  

## 2. CMakeのインストール

Effekseer For C++には、ビルド済みのバイナリは含まれていません。  
そのため、導入前にビルドをする必要があります。  

ビルドをするためには、CMakeというソフトウェアを使います。  

CMakeを公式サイトからダウンロード、インストールしてください。   

**CMakeダウンロードページ: <a href="https://cmake.org/download/" target="_blank" rel="noopener noreferrer">https://cmake.org/download/</a>**  

> [!IMPORTANT]  
> Sourceではなく、Installerをダウンロードしてください。  
> 例えば、64bit版のWindowsを使っている場合は、Windows x64 Installer(`cmake-x.xx.0-xxx-windows-x86_64.msi`)をダウンロードします。

**このとき、必ず「Add CMake to the system PATH for all users」か「Add CMake to the system PATH for the current user」のどちらかにチェックを入れて、PATHを通してください。**  
(環境変数/PATHを自力で設定できる場合は、自力で設定しても問題ありません)  
![CMake_InstallOptions](images/CMake_InstallOptions.png)  

> [!NOTE]  
> Windows Defenderなどのセキュリティソフトによって、起動が停止されることがあります。  
> 関連: [「WindowsによってPCが保護されました」と表示される](#91-windowsによってpcが保護されましたと表示される)

## 3. Effekseer For C++のビルド

### 3.1. ダウンロード

まず、Effekseer for C++(`EffekseerForCppXXX.zip`)をダウンロード、解凍してください。  

**Effekseer for C++ダウンロードページ: <a href="https://github.com/effekseer/Effekseer/releases" target="_blank" rel="noopener noreferrer">https://github.com/effekseer/Effekseer/releases</a>**

### 3.2. ビルド

ビルドを実行するバッチファイルを用意しています。  

`build_msvc.bat`を実行してください。  

> [!NOTE]  
> Windows Defenderなどのセキュリティソフトによって、起動が停止されることがあります。  
> 関連: [9.1. 「WindowsによってPCが保護されました」と表示される](#91-windowsによってpcが保護されましたと表示される)

実行すると、コンソールが表示されます。  
以下は、表示される内容の例です。

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
 [1. 導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認した、`VisualStudioのバージョン`や`プラットフォーム`に合ったプリセットを見つけてください。  

例えば、「Visual Studioバージョンが`2022`、プラットフォームが`x64`」ならば使用するプリセットは「`Visual Studio 2022(x64)`」で、「Visual Studioバージョンが`2019`、プラットフォームが`Win32`」ならば使用するプリセットは「`Visual Studio 2019(x86)`」です。  

(プラットフォームが`Win32`である場合は、`(x86)`のプリセットを選択してください)   

> [!WARNING]  
> Vulkanを使っていない場合、**`with Vulkan`が付いたプリセットは使用しないでください**。  

**適切なプリセットを見つけたら、対応する番号を入力し、Enterキーを押してください。**  

つづいて、ランタイムライブラリDLL設定を有効化するかの確認が表示されます。  

```
Enable runtime library DLL option?(y/n):
```

 [1. 導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)**で確認した、`プロジェクト設定の「ランタイムライブラリ」設定`に合わせて、`y`か`n`のどちらかを入力して、Enterキーを押してください。**  
 
  
- `マルチスレッド DLL (/MD)`or`マルチスレッド デバッグ DLL (/MDd)`: `y`
- `マルチスレッド (/MT)`or`マルチスレッド デバッグ (/MTd)`: `n`

ビルドが開始します。  
しばらく待つと、ビルドが完了します。  
（成否にかかわらず、ビルドの処理が終了すると`続行するには何かキーを押してください`と表示されます。）

インストールフォルダが作成されます。  

例えば、Visual Studio 2022でx86の場合、  
生成されるインストールフォルダは `install_msvc2022_x86` です。  
**以降、このフォルダのことを「インストールフォルダ」と呼びます。**   

インストールフォルダは、以下のようなフォルダ構成です。  
**インストールフォルダと、中身のファイルやフォルダが正しく生成されているか、確認してください。**  

```
📁install_msvcXXXX_XXX/ <- インストールフォルダ
 ├ 📁include/
 │ │ (それぞれのフォルダに、ヘッダファイル(.h)などが入っている)
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
 　 │ (ライブラリファイル(.lib)が、いくつも入っている
 　 └ 例：Effekseer.lib、Effekseerd.lib、EffekseerRendererCommon.lib、EffekseerRendererDX12.lib)
```

> [!NOTE]  
> ビルドがうまく行かない場合は、トラブルシューティングを確認してください。  
> [9.2. CMakeビルド関連でうまく行かない時](#92-cmakeビルド関連でうまく行かない時)

## 4. ビルドしたライブラリファイルの配置

さきほど[3. Effekseer For C++のビルド](#3-effekseer-for-cのビルド)で出来上がったインストールフォルダから、環境に合わせて必要なものを導入先のアプリケーションのプロジェクトへコピーします。

> [!NOTE]  
> 本ドキュメント上でのファイルやフォルダの配置は、一例です。  
> 分かる方は自由に設定してください。  

> [!NOTE]  
> **ここでは、アプリケーションのプロジェクト（.projファイル）があるフォルダのことを、「導入先プロジェクトフォルダ」と呼びます。**

### 4.1. ヘッダファイル（.h）の配置

インクルードするための、ヘッダファイル(.h)をコピーします。

[3. Effekseer For C++のビルド](#3-effekseer-for-cのビルド)で出来上がったインストールフォルダ(例: `install_msvc2022_x86/`)から、環境に合わせて必要なものをコピーします。

**以下を`[導入先プロジェクトフォルダ]/Libraries/Effekseer/Include/`へコピーしてください。**  
**フォルダごと、中身のファイルやフォルダを含めてコピーしてください。**  

**<必ずコピー>**
- `[インストールフォルダ]/include/Effekseer`  
- `[インストールフォルダ]/include/EffekseerRendererCommon`  

**<DirectX 11の場合は以下もコピー>**  
- `[インストールフォルダ]/include/EffekseerRendererDX11`  

**<DirectX 12の場合は以下もコピー>**  
- `[インストールフォルダ]/include/EffekseerRendererDX12`  

> [!NOTE]  
> `[導入先プロジェクトフォルダ]/Libraries/Effekseer/Include/`フォルダは、自分で作成してください。

![IncludeFiles](images/IncludeFiles_Ja.png)


### 4.2. ライブラリファイル（.lib）の配置

つづいて、ライブラリファイル(.lib)をコピーします。  

さきほどEffekseer for Cppをビルドしたときに出力されたインストールフォルダ(例：`install_msvc2022_x86/lib`)から、  
**`[導入先プロジェクトフォルダ]/Libraries/Effekseer/Lib]`へ、以下のファイルをコピーしてください。**  


**<必ずコピー>**
- `Effekseer.lib`
- `Effekseerd.lib`
- `EffekseerRendererCommon.lib`
- `EffekseerRendererCommond.lib`

**<DirectX 11の場合は以下もコピー>**  
- `EffekseerRendererDX11.lib`
- `EffekseerRendererDX11d.lib`

**<DirectX 12の場合は以下もコピー>**  
- `EffekseerRendererDX12.lib`
- `EffekseerRendererDX12d.lib`

> [!NOTE]  
> `[導入先プロジェクトフォルダ]/Libraries/Effekseer/Lib/`フォルダは、自分で作成してください。

![LibraryFiles](images/LibraryFiles_Ja.png)

> [!NOTE]  
> `xxxxxxd.lib`のように、`d`が末尾についているライブラリファイルは、Debugビルド構成設定のライブラリファイルです。  


### 4.3. ファイルの配置を再度チェック
以下のようなファイル/フォルダ構成になっているか、再度確認しておきます。  

**DirectX 11の場合**
```
📁[導入先プロジェクトフォルダ] (導入先プロジェクト(.projファイル)の場所)
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
```

**DirectX 12の場合**
```
📁[導入先プロジェクトフォルダ] (導入先プロジェクト(.projファイル)の場所)
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
```

## 5. 導入先アプリケーションのVisualStudioプロジェクト設定

導入先アプリケーションのアプリケーションが、Effekseer for C++ライブラリを扱えるようにプロジェクト設定をします。  
「インクルードディレクトリ」と「ライブラリディレクトリ」の２つを設定します。  

> [!NOTE]  
> **ここからは、導入先のアプリケーションでの作業です。**  
> **導入先のアプリケーションの、VisualStudioのソリューション（プロジェクト）を開いてください。**   

**VisualStudioで、導入先のプロジェクトのプロパティを開きます。**  

1. ソリューションエクスプローラーから、導入先のアプリケーションのプロジェクトを右クリックします。  
2. 表示されるメニューから、「プロパティ」を選択して、プロパティページを開きます。  
（図のソリューション/プロジェクトの名称や構成は一例です）  
![VisualStudio_OpenProjectProperty](images/VisualStudio_OpenProjectProperty_Ja.png)

### 5.1. インクルードディレクトリの設定

インクルードファイルを配置した場所を登録するため、インクルードディレクトリを設定します。  

1. **必ず、画面上部の構成を「すべての構成」、プラットフォームを[1. 導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認したプラットフォームに設定しておきます。**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
1. 構成プロパティ > C/C++ > 全般 > 追加のインクルードディレクトリ > 編集 を選択します。  
![VisualStudio_IncludeDirectory_Ja](images/VisualStudio_IncludeDir_Ja.png)
2. 以下を追記します。  

**<必ず記述>**  
```
.\Libraries\Effekseer\Include\Effekseer
.\Libraries\Effekseer\Include\EffekseerRendererCommon
```
**<DirectX 11の場合は以下も記述>**  
```
.\Libraries\Effekseer\Include\EffekseerRendererDX11
```
**<DirectX 12の場合は以下も記述>**   
```
.\Libraries\Effekseer\Include\EffekseerRendererDX12
```
![VisualStudio_IncludeDirAppend](images/VisualStudio_IncludeDirAppend_Ja.png)   
1. 「OK」を押します。

### 5.2. ライブラリディレクトリの設定

同じように、ライブラリファイルも配置した場所を設定します。  

1. **必ず、画面上部の構成を「すべての構成」、プラットフォームを[1. 導入先のアプリケーションの環境を確認](#1-導入先のアプリケーションの環境を確認)で確認したプラットフォームに設定しておきます。**  
![ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)
2. 構成プロパティ > リンカー > 全般 > 追加のライブラリディレクトリ > 編集 を選択します。  
![VisualStudio_LibraryDir](images/VisualStudio_LibraryDir_Ja.png)
3. 以下を追記します。  
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

- **DirectX11サンプルソース: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX11)**  
- **DirectX12サンプルソース: [https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12](https://github.com/effekseer/Effekseer/tree/master/Examples/DirectX12)**  


また、以下でサンプルソースの解説をしますので、併せてご参照ください。  

> [!NOTE]  
> サンプル内の`device`(`DeviceDX11` `DeviceDX12`)は、サンプル用の、グラフィックスAPIのデバイスを管理するクラスです。  
> 実際のアプリケーションでは、アプリケーション側(またはフレームワークなど)で管理されます。  
> サンプルソースで、`device`が使用されている箇所は、適宜アプリケーション側の環境に置き換えて実装をしてください。  

### 6.1. 再生するエフェクトファイルを用意する

まず、再生するためのエフェクトを用意します。  
本来はEffekseerエディタで作成しますが、今回はサンプルエフェクトを使います。  
ダウンロードしたEffekseerForCppにある以下のファイルを、導入先プロジェクトフォルダ(.projファイルがあるフォルダ)へコピーします。

- `Examples/Resources/Laser01.efkefc`
- `Examples/Resources/Texture`(フォルダごと)

コピー後、以下のようなフォルダ構成になっていれば大丈夫です。  

```
📁[導入先プロジェクトフォルダ] (導入先プロジェクト(.projファイル)の場所)
 ├ 📄Laser01.efkefc
 └ 📁Texture/
    └ ...
```

### 6.2. includeとpragma文の記述

**<必ず記述>**  
```cpp
#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#else
#pragma comment(lib, "Effekseerd.lib")
#endif
#include <Effekseer.h>
```

**<DirectX11使用時は以下も記述>**  
```cpp
#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX11.lib")
#else
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#endif
#include <EffekseerRendererDX11.h>
```

**<DirectX12使用時は以下も記述>**  
```cpp
#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX12.lib")
#else
#pragma comment(lib, "EffekseerRendererDX12d.lib")
#endif
#include <EffekseerRendererDX12.h>
```

必要なヘッダファイルのインクルードと、ライブラリファイルのリンクを行うように指定します。  
デバッグ構成のときは、`XXXXXXXd.lib`を使うようにします。  

### 6.3. エフェクトのマネージャーの作成

```cpp
// Create a manager of effects
// エフェクトのマネージャーの作成
::Effekseer::ManagerRef efkManager = ::Effekseer::Manager::Create(8000);
```

`Effekseer::Manager`を作成、初期化します。  

> [!TIP]  
> `Effekseer::ManagerRef`や`Effekseer::EffectRef`など、Effekseerの型で`Ref`が付くものは、スマートポインタで管理されています。  
> これらは、オブジェクトを使用する変数が無くなると自動で解放されます。  

### 6.4. 描画モジュールの作成

**<DirectX11の場合>**

```cpp
// Create a  graphics device
// 描画デバイスの作成
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(device.GetID3D11Device(), device.GetID3D11DeviceContext());

// Create a renderer of effects
// エフェクトのレンダラーの作成
::EffekseerRenderer::RendererRef efkRenderer;
efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);
```

**<DirectX12の場合>**
```cpp
// Create a  graphics device
// 描画デバイスの作成
::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(device.GetID3D12Device(), device.GetCommandQueue(), 3);

// Create a renderer of effects
// エフェクトのレンダラーの作成
::EffekseerRenderer::RendererRef efkRenderer;
DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

// Create a memory pool
// メモリプールの作成
::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

// Create a command list
// コマンドリストの作成
::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;
efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
```

DirectX11では、`::EffekseerRendererDX11::CreateGraphicsDevice()`で、DirextX11のDevice(`ID3D11Device*`)とDeviceContext(`ID3D11DeviceContext*`)を引数として渡します。  

DirectX12では、`::EffekseerRendererDX12::CreateGraphicsDevice()`で、DirectX12のDevice(`ID3D12Device*`)とCommandQueue(`ID3D12CommandQueue*`)を引数として渡します。  
また、`::EffekseerRendererDX12::Create`の第2,4引数は、レンダーターゲットと深度バッファのフォーマット`DXGI_FORMAT`を渡します。

**サンプルソース上では、サンプル用のデバイス管理クラス(`device`, `DeviceDX11`/`DeviceDX12`)からDevice等を取得し、セットしています。**  
**実際に組み込む際は、アプリケーション/フレームワークに合わせた実装に置き換えてください。**

### 6.5. 作成した描画モジュールの設定

```cpp
// Specify rendering modules
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


### 6.6. 座標系の設定

```cpp
// Setup the coordinate system. This must be matched with your application.
// 座標系を設定する。アプリケーションと一致させる必要がある。
efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
```

座標系を設定します。  

実際には、導入先のアプリケーションの座標系と一致させる必要がありますが、まずは動作確認のため、右手座標系(`RH`)で実装します。  
(座標系については後述します。(関連: [7. カメラと座標系をアプリケーションと一致させる](#7-カメラと座標系をアプリケーションと一致させる)))

### 6.7. 投影行列とカメラ行列の作成

```cpp
// Specify a position of view
// 視点位置を確定
::Effekseer::Vector3D viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

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

こちらも実際には、アプリケーションのものと一致(同期)させる必要がありますが、まずは動作確認のため、右手座標系(RH)の固定の投影行列/カメラ行列を作成して使用します。  

### 6.8. エフェクトの読み込み

```cpp
// Load an effect
// エフェクトの読込
Effekseer::EffectRef effect;
effect = Effekseer::Effect::Create(efkManager, u"Laser01.efkefc");
```

[6.1. 再生するエフェクトファイルを用意する](#61-再生するエフェクトファイルを用意する)でコピーしてきたエフェクトファイル(`.efkefc`)から、エフェクトを読み込みます。  
戻り値として、読み込まれたエフェクトデータ(`EffectRef`)が返ってきます。  

`EffectRef`(`effect`)は、再生中のエフェクトではなく、「エフェクトを再生するために必要なデータ」です。  
同じエフェクトを複数再生するときは、使いまわすことができます。  

また、読み込み処理なので、比較的処理時間が長くなります。  

そのため、基本的には`Effekseer::Effect::Create`は、１種類のエフェクトファイルに対して１度のみ実行するだけで良いです。  

> [!WARNING]  
> サンプルソース中の`EFK_EXAMPLE_ASSETS_DIR_U16`は、サンプル専用のマクロです。  
> 導入先のアプリケーションで使うことができません。  

> [!NOTE]  
> `Effekseer::Effect::Create()`関数の第２引数の`path`(.efkefcファイルのパス)は、`char16_t*`型です。  
> 通常の文字列リテラル(`"ABCDE"`)や、通常の文字列(`char*`,`std::string`)をそのまま使うことはできません。  
> 上記のソースでは、文字列リテラルの前に`u`を付けることによって、`char16_t*`型の文字列リテラルにしています。  

### 6.9. エフェクトの再生と制御


```cpp
// Elapsed frames
// 経過したフレーム数のカウンタ
int32_t time = 0;

// Handle for the effect instance. Used to control the played effect.
// エフェクトのインスタンスのハンドル。再生したエフェクトの制御に使う。
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

> [!NOTE]  
> `while (device.NewFrame()) {}`がメインループにあたります。  
> この中が１フレーム間に実行する処理になるので、毎フレーム実行したい処理はここに実装されています。  
> (メインループの実装もサンプル用です。メインループは、導入先のアプリケーションやフレームワークの実装と置き換えて捉えて実装してください)  

毎フレーム`time`をインクリメントさせることで、経過フレーム数をカウントしています。  

`Effekseer::Handle`(`efkHandle`)を使うことで、再生中のエフェクトの、移動や停止などの制御を行っています。  

### 6.10. [DirectX12のみ、毎フレーム実行]フレーム開始時の処理


```cpp
while (device.NewFrame())
{
  // <<DirectX 12の時のみ、以下を追記>>

  // Call on starting of a frame
  // フレームの開始時に呼ぶ
  efkMemoryPool->NewFrame();

  // Begin a command list
  // コマンドリストを開始する。
  EffekseerRendererDX12::BeginCommandList(efkCommandList, device.GetCommandList());
  efkRenderer->SetCommandList(efkCommandList);

  ...(略)
```

**DirectX 12の時のみ、毎フレーム行う必要があります。**  
メインループのはじめに、描画に使用するコマンドリストの記録を開始します。  
さきほどの、[6.9. エフェクトの再生と制御](#69-エフェクトの再生と制御) でのメインループのはじめに追記してください。  


ここでも、サンプル用に用意されているデバイス管理クラス(`device`)からGetCommandListを取得してセットしていますが、こちらも導入先のアプリケーションに合わせて置き換えて実装します。  

### 6.11. [毎フレーム実行]Managerの更新処理

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
  
**この処理は毎フレーム実行する(メインループの中に書く)必要があります。**  

LayerParameterの更新(`efkManager->SetLayerParameter`)と、Managerの更新(`efkManager->Update`)を行います。  

Managerの更新の際には、引数に`Effekseer::Manager::UpdateParameter`を渡すことが出来ます。  
これを使うことで、経過フレームや更新回数、非同期処理などに関する制御が可能です。

### 6.12. [毎フレーム実行]Rendererの更新と描画処理

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

**この処理は毎フレーム実行する(メインループの中に書く)必要があります。**  

**DirectXでの描画処理の開始から、描画処理の終了までの間に行うようにしてください。**  
(サンプルでは`device.ClearScreen();`と`device.PresentDevice();`の間に行っています)  

経過時間の更新や、投影/カメラ行列のセットをしてから、描画処理を行います。  

`efkRenderer->SetTime(time / 60.0f);`では、[6.9. エフェクトの再生と制御](#69-エフェクトの再生と制御)で作成し、毎フレームインクリメントさせた`time`を使って、経過時間をセットしています。  
このサンプルでは、フレームレートが60fpsであるとして、`time`を60で割っています。  

### 6.13. [DirectX12のみ、毎フレーム実行]コマンドリスト終了処理

```cpp
// <<DirectX 12の時のみ、以下を追記>>
// ※efkRenderer->EndRendering();の直後に実装する

// Finish a command list
// コマンドリストを終了する。
efkRenderer->SetCommandList(nullptr);
EffekseerRendererDX12::EndCommandList(efkCommandList);
```

**DirectX 12の時のみ、毎フレーム行う必要があります。**  
さきほどの、[6.12. [毎フレーム実行]Rendererの更新と描画処理](#612-毎フレーム実行rendererの更新と描画処理) でのメインループの終盤、`efkRenderer->EndRendering();`の直後に実装してください。  

### 6.14. [DirectX12のみ]解放処理

```cpp
// 		time++;
// }
// DirectX12のみ、メインループが終わった直後に記述

efkCommandList.Reset();
efkMemoryPool.Reset();
efkRenderer.Reset();
```

DirectX12のみ、解放処理を行います。  
メインループが終わった直後に実装します。  

### 6.15. 動作を確認する

ここまでが、最低限のエフェクトの再生をする実装です。  

一度、ビルド・実行を試してみてください。  

正しく導入できていれば、下図のようにエフェクトが再生されているはずです。  

もし、正しく再生されなかったり、コンパイル/リンクエラーなどが出た場合には、どこかに誤りがある可能性があります。確認・修正してください。  
[9. トラブルシューティング（うまく動かないとき）](#9-トラブルシューティングうまく動かないとき)もご活用ください。  

![LaserEffect](images/LaserEffect.png)  

## 7. カメラと座標系をアプリケーションと一致させる

今の状態では、アプリケーション側のカメラを動かしたり視野角を変更しても、エフェクトの位置や見え方が変わりません。  

Effekseerとアプリケーションのカメラと座標系を一致させることで、正しい位置にエフェクトが描画されるように、追加/変更します。  

### 7.1. カメラを同期させる

投影(射影)行列とカメラ行列は、毎フレームカメラの情報をEffekseer側にセットする必要があります。  

以下は、投影(射影)行列とカメラ行列を同期させる実装例です。  

**以下を、[6.12. [毎フレーム実行]Rendererの更新と描画処理](#612-毎フレーム実行rendererの更新と描画処理) の、`efkRenderer->SetProjectionMatrix`や`efkRenderer->SetCameraMatrix`を呼び出している箇所の前に追加します。**  

```cpp
// efkRenderer->SetProjectionMatrix efkRenderer->SetCameraMatrixを呼び出す前に追加

// EffekseerのCameraMatrixは、カメラの姿勢行列の逆行列を指す。カメラ行列がカメラの姿勢行列である場合は、逆行列化しておく。
auto invAppCameraMatrix = appCameraMatrix.Invert();
// アプリケーションとEffekseerの、投影(射影)行列とカメラ行列を同期
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
> `appProjectionMatrix`や`invAppCameraMatrix`は、アプリケーション側の投影(射影)行列とカメラ行列にあたります。  
> また、DirectXTKの`DirectX::SimpleMath::Matrix`型であるときの実装例です。  
> 導入先のアプリケーションに合わせて置き換えて実装してください。  

> [!IMPORTANT]  
> EffekseerのCameraMatrixは、**カメラの姿勢行列の逆行列**を指します。  
> 一方で、カメラの姿勢行列のことをCameraMatrix、その逆行列のことをViewMatrixとするフレームワークも多いです。  
> そのため、上記の実装例では、CameraMatrixの逆行列をセットしています。  

### 7.2. 座標系をアプリケーションと一致させる

3D空間を表現するための座標系には、右手系（RH）と左手系（LH）の２種類があります。  

この座標系を、アプリケーションと一致させる必要があります。  
(Effekseerの座標系はデフォルトでは右手系（RH）です)  

**もし、導入先のアプリケーションが左手系であれば、Effekseerの初期化時に左手系座標系に設定する必要があります。**  
**[6.6. 座標系の設定](#66-座標系の設定)で`m_efkManager->SetCoordinateSystem`を呼び出している箇所を、以下のように書き換えることで、左手(LH)座標系に変更することができます。**  
(右手座標系である場合は、サンプルそのまま(RH)で問題ありません)

```cpp
...
// 導入先アプリケーションが左手座標系を使っている場合は、以下のようにに変更する
m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
...
```

> [!NOTE]  
> 左手/右手座標系の違いは、**XYZ軸の正(プラス)の向きの組み合わせ**です。  
> 下図のように、左右の手の親指/人差し指/中指の方向が、XYZ軸の正(プラス)の向きに対応しています。  
> これにより、導入先のアプリケーションの座標系を調べることができます。  
> ![CoordinateSystem](images/HelpCpp_CoordinateSystem_Ja.png)

### 7.3. レイヤーパラメータの設定

サンプル通りの実装では、`layerParameter.ViewerPosition`に固定の座標(`viewerPosition`)がセットされてしまっています。  

これも、実際のカメラの座標に合わせる必要があります。  

以下は、実際のカメラ座標に合わせて`layerParameter.ViewerPosition`を設定する実装例です。  
**[6.11. [毎フレーム実行]Managerの更新処理](#611-毎フレーム実行managerの更新処理)での該当箇所を、以下のように追記、変更します。**  

```cpp
// レイヤーパラメータの設定
Effekseer::Manager::LayerParameter layerParameter;
::Effekseer::Matrix44 invEfkCameraMatrix;
::Effekseer::Matrix44::Inverse(invEfkCameraMatrix, cameraMatrix);
layerParameter.ViewerPosition = ::Effekseer::Vector3D(invEfkCameraMatrix.Values[3][0], invEfkCameraMatrix.Values[3][1], invEfkCameraMatrix.Values[3][2]);
```

さきほど、[7.1. カメラを同期させる](#71-カメラを同期させる)でアプリケーションと同期させた、Effekseer側のカメラ行列 `cameraMatrix`から、カメラの座標を取り出しています。  


## 8. [付録] エフェクトの基本的な制御

エフェクトの制御について、基礎的なものを解説します。  

> [!NOTE]   
> - `Effekseer::EffectRef`は、`Effekseer::Effect::Create`で読み込んだエフェクトのデータへの参照です。エフェクトを再生開始するために使います。  
> - `Effekseer::EffectHandle`は、再生したエフェクトのインスタンス1つ1つの識別番号です。再生したエフェクトについての操作をするために使います。`efkManager->Play`で再生したときの戻り値などで、取得することができます。  
>
> 下図はイメージ図です。  
> ![EffectRefHandle_Explain](images/EffectRefHandle_Explain_Ja.png)

### 8.1. エフェクトを再生する

`efkManager->Play`で、エフェクトを再生します。  
引数で、エフェクトの位置を指定することもできます。  

**戻り値の`Effekseer::Handle`を用いて、再生したエフェクトのインスタンスに対して制御を行うことができます**。  

```cpp
Effekseer::Handle efkHandle = 0;
// エフェクトを表示したい座標を設定する
efkHandle = efkManager->Play(effect, 0.0f, 0.0f, 0.0f);
```

### 8.2. エフェクトを移動させる

`efkManager->SetLocation`で、エフェクトのインスタンスを任意の座標に移動させます。

```cpp
efkManager->SetLocation(efkHandle, 1.0f, 1.0f, 1.0f);
```

`efkManager->AddLocation`で、指定した座標だけ動かすこともできます。
```cpp
efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));
```

### 8.3. エフェクトを回転させる

`efkManager->SetRotation`で、エフェクトのインスタンスの姿勢を回転させます。  

角度はラジアンで指定します。

```cpp
// XYZ座標軸についての回転
efkManager->SetRotation(efkHandle, 3.141592653589f, 0.0f, 0.0f);
```

### 8.4. エフェクトを拡大縮小させる

`efkManager->SetScale`で、エフェクトのインスタンスの拡大縮小を設定します。

```cpp
// 2倍のスケールに設定
efkManager->SetScale(efkHandle, 2.0f, 2.0f, 2.0f);
```

### 8.5. エフェクトを停止させる

`efkManager->StopEffect`で、エフェクトのインスタンスを停止させます。  

```cpp
efkManager->StopEffect(efkHandle);
```

`efkManager->SetPaused`で、一時停止や再開をすることもできます。

```cpp
// 一時停止
efkManager->SetPaused(efkHandle, true);

// 再開
efkManager->SetPaused(efkHandle, false);
```

### 8.6. エフェクトをループ再生する

エフェクトが再生終了していたら、新たに再度再生しなおすことで、ループ再生をすることができます。  

```cpp
// 毎フレーム、エフェクトが再生終了しているか確認する
if (efkManager->Exists(efkHandle) == false)
{
  // 新たにエフェクトを再生し直す。座標はエフェクトを表示したい場所を設定する
  // (位置や回転、拡大縮小も設定しなおす必要がある)
  efkHandle = efkManager->Play(efkManager, 0.0f, 0.0f, 0.0f);
}
```

## 9. トラブルシューティング（うまく動かないとき）

### 9.1. 「WindowsによってPCが保護されました」と表示される

Widnows Defenderによる表示です。  

画面中央付近の `詳細情報` をクリックしてから、表示される`実行する` ボタンをクリックすることで、実行することができます。  

![WindowsDefender_protection](images/WindowsDefender_protection_Ja.png)  

### 9.2. CMakeビルド関連でうまく行かない時

以下を確認またはお試しください。  

- GitHubのmasterブランチからcloneしている場合、Releasesページ([https://github.com/effekseer/Effekseer/releases](https://github.com/effekseer/Effekseer/releases))からfor C++をダウンロードしてお試しください
- CMakeの最新バージョンがインストールされているか
- CMakeのパスが通っているか  
(出来ていなければ、自力でパスを通すか、CMakeを再インストールしてください。再インストールの際には、必ず「Add CMake to the system PATH for all users」か「Add CMake to the system PATH for the current user」のどちらかにチェックを入れて、PATHを通してください)
- 誤ってvulkanのプリセットを使ってビルドしていないか

### 9.3. 導入先アプリケーションでのビルドエラー

> [!NOTE]  
> 以下で示すエラー文は一例です。  
> `Effekseer.h` `Effekseer.lib`のようなファイル名などは、適宜読み替えてください。  

#### 9.3.1. ライブラリ/インクルードファイルが開けない

エラーメッセージの例：  

- `LNK1104 ファイル 'Effekseer.lib' を開くことができません。`
- `E1696 ソース ファイルを開けません "Effekseer.h"`
- `C1083 include ファイルを開けません。'Effekseer.h':No such file or directory`

いずれかが発生した場合は、該当するファイルが見つからない等の理由で、開くことができない状態です。  
以下を確認してください。

- 該当のファイルが正しいディレクトリに置かれているか
- プロジェクトのプロパティの、`ライブラリディレクトリ`や`インクルードディレクトリ`が正しく設定されているか
- プロジェクトのプロパティの、`ライブラリディレクトリ`や`インクルードディレクトリ`が、適切なプロジェクトや構成、プラットフォームに対して設定されているか
- `#include <Effekseer.h>` `#pragma comment(lib, "Effekseer.lib")`などのプリプロセッサ文が正しいか

> [!NOTE]  
> プロジェクトのプロパティは、プロパティページ上部の構成・プラットフォームに対してのみ適用されます。  
> 
> 正しく設定したつもりであっても、選択した構成やプラットフォームに誤りがあることにより、`ライブラリディレクトリ`や`インクルードディレクトリ`が正しく設定されていないこともあります。  
> 
> ライブラリディレクトリやインクルードディレクトリを設定する際は、[5.1. インクルードディレクトリの設定](#51-インクルードディレクトリの設定)や[5.2. ライブラリディレクトリの設定](#52-ライブラリディレクトリの設定)にあるように、必ず適切な`ライブラリディレクトリ`や`インクルードディレクトリ`を選択してください。  
> ![VisualStudio_ConfigurationAndPlatform](images/VisualStudio_ConfigurationAndPlatform_Ja.png)

#### 9.3.2. ライブラリのリンクに失敗する

エラーメッセージの例：  

- `LNK2001 外部シンボル "public: __thiscall Effekseer::Matrix44::Matrix44(void)" (??0Matrix44@Effekseer@@QAE@XZ) は未解決です`
- `LNK2019 未解決の外部シンボル "__imp__rand" が関数 "private: static int __cdecl Effekseer::ManagerImplemented::Rand(void)" (?Rand@ManagerImplemented@Effekseer@@CAHXZ)' で参照されました。`
- `LNK4272 ライブラリのコンピューターの種類 'x64' がターゲットのコンピューターの種類' x86' と競合しています`
- `LNK1112 モジュールのコンピューターの種類 'x64' がターゲットのコンピューターの種類' x86' と競合しています` 
- `LNK2038 'RuntimeLibrary' の不一致が検出されました。値 'MDd_DynamicDebug' が MTd_StaticDebug の値 'GraphEditor.obj' と一致しません。`


いずれかが発生した場合は、[3. Effekseer For C++のビルド](#3-effekseer-for-cのビルド)でビルドした際に選択したプリセットや設定に誤りがあったと考えられます。  

以下の手順で、正しいプリセットや設定で、再度Effekseer For C++のビルドを行ってください。

1. Effekseer For C++の、ビルドフォルダ(`build_msvcXXXX_XXXX`)とインストールフォルダ(`install_msvcXXXX_XXXX`)を削除する
2. 導入先のプロジェクトに合ったプリセットや設定で、Effekseer For C++をビルドする([3. Effekseer For C++のビルド](#3-effekseer-for-cのビルド))
3. インストールフォルダに新たに生成されたライブラリファイルを、導入先のプロジェクトへコピーしなおす([4. ビルドしたライブラリファイルの配置](#4-ビルドしたライブラリファイルの配置))  
