This readme is available in English and Japanese.

English
==========================
* Outline

Thank you for downloading Effekseer.
This package contains the Effekseer effect editing tool for 64-bit Linux.
To play effects in a game, download the runtime or plug-in for your engine separately.

* Operating environment

- 64-bit Linux
- An OpenGL 3.2 compatible graphics environment

* Directory structure

- Tool
  The Effekseer editor and supporting tools.
- Sample
  Sample effects provided by various users.
- Help_Ja.html / Help_En.html
  Effekseer help.

* Starting Effekseer

Run `Tool/Effekseer`. If execution permissions were lost while extracting the archive,
run `chmod +x Tool/Effekseer Tool/bin/Effekseer` first.

* Verifying the download

Download SHA256SUMS and provenance.sigstore.json alongside the release ZIP.
Check the archive with `sha256sum --check --ignore-missing SHA256SUMS`.
Authenticate its origin with GitHub CLI (replace VERSION with the release version):

gh attestation verify EffekseerVERSIONLinux.zip --repo effekseer/Effekseer --signer-workflow effekseer/Effekseer/.github/workflows/build.yml --bundle provenance.sigstore.json

The build-info directory records the source commit and build tool versions.


日本語
==========================
■ 概要

Effekseerをダウンロードしていただきありがとうございます。
このパッケージには64bit Linux向けのエフェクト編集ツールが含まれています。
ゲーム内でエフェクトを再生する場合は、使用するエンジン向けのランタイムまたはプラグインを別途ダウンロードしてください。

■ 動作環境

- 64bit Linux
- OpenGL 3.2互換のグラフィックス環境

■ ディレクトリ構成

- Tool
  Effekseer本体と補助ツールが保存されています。
- Sample
  様々なユーザーから提供されたサンプルエフェクトが保存されています。
- Help_Ja.html / Help_En.html
  Effekseerのヘルプです。

■ Effekseerの起動

`Tool/Effekseer`を実行してください。アーカイブ展開時に実行権限が失われた場合は、
先に`chmod +x Tool/Effekseer Tool/bin/Effekseer`を実行してください。

■ ダウンロードの検証

リリース ZIP と同時に SHA256SUMS と provenance.sigstore.json をダウンロードしてください。
`sha256sum --check --ignore-missing SHA256SUMS`でハッシュを検証できます。
配布元の確認には GitHub CLI で上記の`gh attestation verify`コマンドを実行してください。
VERSION はダウンロードしたバージョンに置き換えてください。
build-info ディレクトリにはソースのコミットとビルドツールのバージョンが記録されています。
