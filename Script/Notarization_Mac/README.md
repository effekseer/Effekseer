# Notarization_Mac

## English

This folder contains helper scripts for codesigning, notarizing, and stapling the macOS build of Effekseer.

### Files

- `Effekseer_cert.sh`
  - Codesigns the app bundle and bundled executables, creates `Effekseer.dmg`, and submits it for notarization with `xcrun notarytool` using a Keychain profile.
- `Effekseer_cert_check.sh`
  - Checks the notarization status using the request ID with `xcrun notarytool` and the same Keychain profile.
- `Effekseer_cert_post.sh`
  - Staples the notarization ticket to `Effekseer.dmg`.
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

After notarization is approved, run:

```bash
sh Effekseer_cert_post.sh
```

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

## 日本語

このフォルダには、Effekseer の macOS 版に対してコード署名、Notarization、Staple を行うための補助スクリプトがあります。

### ファイル

- `Effekseer_cert.sh`
  - アプリ本体と同梱バイナリにコード署名を行い、`Effekseer.dmg` を作成して `xcrun notarytool` で Notarization を申請します。
- `Effekseer_cert_check.sh`
  - Request ID を使って `xcrun notarytool` で Notarization の状況を確認します。
- `Effekseer_cert_post.sh`
  - `Effekseer.dmg` に notarization ticket を stapling します。
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

Notarization が承認されたら、次を実行します。

```bash
sh Effekseer_cert_post.sh
```

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
