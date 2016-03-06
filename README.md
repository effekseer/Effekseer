Effekseer
=========
本ソフトウェアはヴィジュアルエフェクト編集ツールです。
ランタイムやプラグインを組み込むことで、アプリケーション中で作成されたエフェクトを再生できるようになります。
主にゲームで使用することを想定しており、ヴィジュアルエフェクトの編集をプログラマでなくデザイナーが担当できるようにします。

* [公式ウェブサイト](http://effekseer.github.io/jp/)

This software is a visual effects editing tool.
By incorporating a runtime and plug-ins, you will be able to play the effect that was created in the application.
It is assumed to be used mainly in the game, and the editing of visual effects rather than the programmer so designers can be in charge.

* [Official website](http://effekseer.github.io/en/)

[![](http://img.youtube.com/vi/WwNX9TO7guA/0.jpg)](https://www.youtube.com/watch?v=WwNX9TO7guA)

**不具合情報**

現在、UnityMac版ではLegacyDefferedとForwardのみ描画対応しています。不具合の原因は現在調査中です。

# RoadMap

**Version 1.20**
* ?

# Todo
プルリクエストお待ちしております。

**小規模な追加・変更**
* ドキュメントの改良
* 文言の改良
* 各種アイコンの追加
* サンプルの追加
* Windows、Linux以外のテストの構築

**中規模な追加・変更**
* Effectのパラメーターの動的な変更
* ショートカットの見直し
* モデル出力プラグイン(Blender等)


**大規模な追加・変更**
* ツールのGUIライブラリの変更
* ツールのマルチプラットフォーム化

**他**
* DirectX12対応
* UnrealEngine4, Cocos2D対応
* iPhone,Android対応

**候補**
* HDRレンダリング
* タービュランスノイズ ( https://ics.media/entry/10921 )

# コミッター向け

## ランタイムテスト方法

* Windows

```Dev/Cpp/Test.sln``` を実行する。

* Linux

```Dev/Cpp/build.sh``` を実行する。
```Dev/Cpp/Test/CMakeLists.txt``` をcmakeを使用してmakeを生成しコンパイルする。

* Mac

不明

