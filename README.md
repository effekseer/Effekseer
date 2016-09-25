Effekseer
=========
This software is a tool editing visual effects.
By incorporating a runtime and plug-ins, you will be able to show the effects that was created with Effekseer.
Effekseer is useful in developing game software especially. Not only programmers but also designers take charge of visual effects.

* [Official website](http://effekseer.github.io/en/)

本ソフトウェアはヴィジュアルエフェクト編集ツールです。
ランタイムやプラグインを組み込むことで、Effekseerで作成されたエフェクトをアプリケーション中で再生できるようになります。
主にゲームで使用することを想定しており、ヴィジュアルエフェクトの編集をプログラマでなくデザイナーが担当できるようにします。

* [公式ウェブサイト](http://effekseer.github.io/jp/)

[![Slack Status](https://effekseer-slackin.herokuapp.com/badge.svg)](https://effekseer-slackin.herokuapp.com/)

[![](http://img.youtube.com/vi/WwNX9TO7guA/0.jpg)](https://www.youtube.com/watch?v=WwNX9TO7guA)

* [ForUnity](https://github.com/effekseer/EffekseerForUnity)

* [ForUE4](https://github.com/effekseer/EffekseerForUE4)

* [ForCocos2d-x](https://github.com/effekseer/EffekseerForCocos2d-x)

* [ForDXLib](https://github.com/effekseer/EffekseerForDXLib)

Todo
=========

Please read issues.

[Issues](https://github.com/effekseer/Effekseer/issues)

# RoadMap

## 1.3

* For UnrealEngine4
* For Cocos2d-x

# Todo
プルリクエストお待ちしております。

**小規模な追加・変更**
* ドキュメントの改良
* 文言の改良
* 各種アイコンの追加
* Windows、Linux以外のテストの構築

**中規模な追加・変更**
* Effectのパラメーターの動的な変更
* モデル出力プラグイン(Blender等)


**大規模な追加・変更**
* ツールのGUIライブラリの変更
* ツールのマルチプラットフォーム化

**他**
* DirectX12対応

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
