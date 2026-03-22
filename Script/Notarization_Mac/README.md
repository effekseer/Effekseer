# Notarization_Mac

## English

This folder contains helper scripts for codesigning, notarizing, and stapling the macOS build of Effekseer.

### Files

- `Effekseer_cert.sh`
  - Codesigns the app bundle and bundled executables, creates `Effekseer.dmg`, and submits it for notarization with `xcrun notarytool` using a Keychain profile.
- `Effekseer_cert_check.sh`
  - Checks the notarization status using the request ID with `xcrun notarytool` and the same Keychain profile.
- `Effekseer_notarytool_setup.sh`
  - Stores App Store Connect credentials in the Keychain for later use by `notarytool`.
- `Effekseer_notarytool_log.sh`
  - Downloads the notarization log for a submission ID.
- `entitlements.plist`
  - Entitlements used when signing the app and its bundled binaries.

### Usage

Run these scripts from this directory after placing the macOS build in the expected layout:

```bash
Effekseer/
  Effekseer.app
```

First, store your notarization credentials in the Keychain:

```bash
sh Effekseer_notarytool_setup.sh "Apple ID" "TEAMID" "App-specific password" "effekseer-notarytool"
```

Example:

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "effekseer-notarytool"
```

If you want to use a different profile name, pass it as the second argument or set `NOTARYTOOL_PROFILE`:

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "my-notary-profile"
```

`Effekseer_cert.sh` now performs the full flow, including stapling both the app bundle and the DMG in the correct order.

If you want to check the request status:

```bash
sh Effekseer_cert_check.sh "Request ID" "effekseer-notarytool"
```

If notarization fails and you want the detailed log:

```bash
sh Effekseer_notarytool_log.sh "Request ID" "effekseer-notarytool" "notarytool_log.json"
```

Notes:

- The scripts use `xcrun notarytool` and `xcrun stapler`.
- `Effekseer_cert.sh` expects the app bundle and related tools to already be built and placed under `Effekseer/`.
- `Effekseer_notarytool_setup.sh` is the only script that handles the App-specific password, and it stores that secret in the Keychain for reuse.
- `Effekseer_notarytool_log.sh` is useful when `notarytool submit` returns a rejection and you need the JSON issue report.
- `Effekseer_cert.sh` now performs this full order: notarize app, staple app, create DMG from the stapled app, notarize DMG, staple DMG.
- If the app still shows the Gatekeeper warning, check whether the bundle was modified after signing or whether the quarantine attribute is still present.

### Troubleshooting

If the app cannot be opened on macOS, check these in order:

```bash
codesign --verify --deep --strict --verbose=4 "Effekseer/Effekseer.app"
spctl -a -t exec -vv "Effekseer/Effekseer.app"
xcrun stapler validate "Effekseer/Effekseer.app"
xcrun stapler validate "Effekseer.dmg"
xattr -lr "Effekseer/Effekseer.app"
```

What to look for:

- `codesign` errors usually mean something changed after signing, or a bundled file was not signed correctly.
- `spctl` failures usually mean Gatekeeper does not trust the bundle as distributed.
- `stapler validate` failures usually mean the notarization ticket was not attached to the app or DMG.
- `com.apple.quarantine` in `xattr` output means the file still has a downloaded/quarantined state.

If the issue persists, check the Gatekeeper log:

```bash
log show --last 1h --predicate 'process == "syspolicyd"'
```

## 日本語

このフォルダには、Effekseer の macOS 版に対してコード署名、Notarization、Staple を行うための補助スクリプトがあります。

### ファイル

- `Effekseer_cert.sh`
  - アプリ本体と同梱バイナリにコード署名を行い、`Effekseer.dmg` を作成して `xcrun notarytool` で Notarization を申請します。
- `Effekseer_cert_check.sh`
  - Request ID を使って `xcrun notarytool` で Notarization の状況を確認します。
- `entitlements.plist`
  - 署名時に使用する entitlements です。

### 使い方

このディレクトリで、macOS ビルドを次の配置にした状態で実行します。

```bash
Effekseer/
  Effekseer.app
```

最初に、Notarization 用の認証情報をキーチェーンへ保存します。

```bash
sh Effekseer_notarytool_setup.sh "Apple ID" "TEAMID" "アプリ用パスワード" "effekseer-notarytool"
```

例:

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "effekseer-notarytool"
```

別の profile 名を使いたい場合は、2 番目の引数で指定するか、`NOTARYTOOL_PROFILE` を設定します。

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "my-notary-profile"
```

`Effekseer_cert.sh` が、app の stapling から DMG の作成・stapling までをまとめて実行します。

申請状況を確認したい場合は、次を使います。

```bash
sh Effekseer_cert_check.sh "Request ID" "effekseer-notarytool"
```

Notarization に失敗した場合は、詳細ログを取得できます。

```bash
sh Effekseer_notarytool_log.sh "Request ID" "effekseer-notarytool" "notarytool_log.json"
```

補足:

- スクリプトは `xcrun notarytool` と `xcrun stapler` を使用します。
- `Effekseer_cert.sh` は、`Effekseer/` 配下にアプリ本体と関連ツールがすでに配置されていることを前提としています。
- `Effekseer_notarytool_setup.sh` だけが App-specific password を扱い、その後はキーチェーン内の profile を再利用します。
- `Effekseer_notarytool_log.sh` は、`notarytool submit` が拒否されたときの JSON 形式の issue report を確認するのに便利です。
- `Effekseer_cert.sh` は、`app` を Notarization → staple → DMG 作成 → DMG Notarization → DMG staple の順で処理します。
- 起動できない場合は、署名の破損、Gatekeeper の拒否、quarantine 属性の残存を順に確認してください。

### 問題調査

アプリが起動できないときは、次の順で調べると原因を絞りやすいです。

```bash
codesign --verify --deep --strict --verbose=4 "Effekseer/Effekseer.app"
spctl -a -t exec -vv "Effekseer/Effekseer.app"
xcrun stapler validate "Effekseer/Effekseer.app"
xcrun stapler validate "Effekseer.dmg"
xattr -lr "Effekseer/Effekseer.app"
```

見方:

- `codesign` で失敗する場合は、署名後に中身が変わっているか、同梱ファイルの署名が不完全です。
- `spctl` で失敗する場合は、Gatekeeper が配布物を許可していません。
- `stapler validate` で失敗する場合は、app または dmg に notarization ticket が付いていません。
- `xattr` に `com.apple.quarantine` が残っている場合は、ダウンロード由来の quarantine が効いています。

さらに詳しく見る場合は、Gatekeeper のログを確認します。

```bash
log show --last 1h --predicate 'process == "syspolicyd"'
```
