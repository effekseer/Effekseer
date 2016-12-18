Effekseer
=========
This software is a tool editing particle effects.
By incorporating a runtime and plug-ins, you will be able to show the effects that was created with Effekseer.
Effekseer is useful in developing game software especially. Not only programmers but also designers take charge of particle effects.

* [Official website](http://effekseer.github.io/en/)

本ソフトウェアはパーティクルエフェクト編集ツールです。
ランタイムやプラグインを組み込むことで、Effekseerで作成されたエフェクトをアプリケーション中で再生できるようになります。
主にゲームで使用することを想定しており、パーティクルエフェクトの編集をプログラマでなくデザイナーが担当できるようにします。

* [公式ウェブサイト](http://effekseer.github.io/jp/)

本軟件為粒子效果的編輯工具。
透過組合 runtime 和插件,由Effekseer制作而成的效果便可以在應用程式內播放。
Effekseer主要用作遊戲開發,不僅是程序員而且設計師也可進行粒子效果的編輯。

* [Official website](http://effekseer.github.io/en/)

[![Slack Status](https://effekseer-slackin.herokuapp.com/badge.svg)](https://effekseer-slackin.herokuapp.com/)

[![](http://img.youtube.com/vi/WwNX9TO7guA/0.jpg)](https://www.youtube.com/watch?v=WwNX9TO7guA)

* [ForUnity](https://github.com/effekseer/EffekseerForUnity)

* [ForUE4](https://github.com/effekseer/EffekseerForUE4)

* [ForCocos2d-x](https://github.com/effekseer/EffekseerForCocos2d-x)

* [ForDXLib](https://github.com/effekseer/EffekseerForDXLib)

Todo
=========

Please read issues and wishlist. We always welcome pull requests for improvements, bug fixes, and so on.

issuesとwishlistを読んでください。常時、機能追加、バグ修正といったプルリクエストを募集しております。

[Issues](https://github.com/effekseer/Effekseer/issues)

[Wishlist](https://trello.com/b/APLk8e6V/effekseer-wishlist)

# RoadMap

## 1.3

* For UnrealEngine4
* For Cocos2d-x

# Todo

** Easy **
* Add Samples
* Add Documents
* Add Tests
* Add some icons to Editor GUI

** Medium
* Add model exporters for 3D modeling softwares
* HDR Color
* Bloom Postprocess in Editor
* Effectのパラメーターの動的な変更

** Hard
* Support Metal
* Support DirectX12
* FBX Importer
* ツールのGUIライブラリの変更
* ツールのマルチプラットフォーム化

** VeryHard
* Support Vulkan
* Support AfterEffects

** Others
* タービュランスノイズ ( https://ics.media/entry/10921 )

# コミッター向け

## ランタイムテスト方法

* Windows

```Dev/Cpp/Test.sln``` を実行する。

* Linux

```Dev/Cpp/build.sh``` を実行する。
```Dev/Cpp/Test/CMakeLists.txt``` をcmakeを使用してmakeを生成しコンパイルする。
