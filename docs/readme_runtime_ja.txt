# Effekseerランタイム

## 概要

Effekseerをダウンロードしていただきありがとうございます。
本パッケージはDirectXやOpenGLを使用しているアプリケーションで、エフェクトを再生できるようにするランタイムが含まれています。

デザイナー向けのエフェクトを作成するためのツールやエフェクトのサンプルは含まれていませんので、
それらが必要な方はツールのパッケージをダウンロードするようお願いします。

## 使用方法

### Cmakeのインストール

CMakeをインストールします。CmakeはVisualStudioやXXcode等のプロジェクトを生成するツールです。
Cmakeの実行ファイルがあるディレクトリへのパスを環境変数に追加してください。

https://cmake.org/

### Cmakeを使用しているプロジェクトに組み込む場合

subdirectoryとして、Effekseerを追加することで使用できます。

### Cmakeを使用していないプロジェクトに組み込む場合

環境に応じてbatやshファイルを実行します。

install フォルダが作成され、その中にインクルードファイルとライブラリファイルが生成されます。

例えば Visual Studio 2019 の場合、バッチファイルは build_msvc2019.bat、install フォルダは install_msvc2019 です。

これらをリンクすることで、Effekseer を使用できます。

## サンプル

サンプルはlibファイル等と一緒にコンパイルされます。
Examplesのディレクトリにサンプルが格納されています。

### CustomLoader

ファイルの読み込み方法をカスタマイズするサンプルです。

### DirectX9

DirectX9を使用しているアプリケーションでEffekseerを使用するサンプルです。

### DirectX11

DirectX11を使用しているアプリケーションでEffekseerを使用するサンプルです。

### DirectX12

DirectX12を使用しているアプリケーションでEffekseerを使用するサンプルです。

### OpenGL

OpenGLを使用しているアプリケーションでEffekseerを使用するサンプルです。

### OpenAL

OpenALを使用しているアプリケーションでEffekseerを使用するサンプルです。

### その他

そのほかにも様々なサンプルがあります。