# Notarization_Mac

## English

This folder contains helper scripts for codesigning, notarizing, and stapling the macOS build of Effekseer.

### Files

- `Effekseer_cert.sh`
  - Codesigns the app bundle and bundled executables, creates `Effekseer.dmg`, and submits it for notarization.
- `Effekseer_cert_check.sh`
  - Checks the notarization status using the request ID.
- `Effekseer_cert_post.sh`
  - Staples the notarization ticket to `Effekseer.dmg`.
- `entitlements.plist`
  - Entitlements used when signing the app and its bundled binaries.

### Usage

Run these scripts from this directory after placing the macOS build in the expected layout:

```bash
Effekseer/
  Effekseer.app
```

Example:

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "Apple ID" "App-specific password"
```

After notarization is approved, run:

```bash
sh Effekseer_cert_post.sh
```

If you want to check the request status:

```bash
sh Effekseer_cert_check.sh "Request ID" "Apple ID" "App-specific password"
```

Notes:

- The scripts use `xcrun altool` and `xcrun stapler`.
- `Effekseer_cert.sh` expects the app bundle and related tools to already be built and placed under `Effekseer/`.

## 日本語

このフォルダには、Effekseer の macOS 版に対してコード署名、Notarization、Staple を行うための補助スクリプトがあります。

### ファイル

- `Effekseer_cert.sh`
  - アプリ本体と同梱バイナリにコード署名を行い、`Effekseer.dmg` を作成して Notarization を申請します。
- `Effekseer_cert_check.sh`
  - Request ID を使って Notarization の状況を確認します。
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

例:

```bash
sh Effekseer_cert.sh "Developer ID Application: Your Name (TEAMID)" "Apple ID" "アプリ用パスワード"
```

Notarization が承認されたら、次を実行します。

```bash
sh Effekseer_cert_post.sh
```

申請状況を確認したい場合は、次を使います。

```bash
sh Effekseer_cert_check.sh "Request ID" "Apple ID" "アプリ用パスワード"
```

補足:

- スクリプトは `xcrun altool` と `xcrun stapler` を使用します。
- `Effekseer_cert.sh` は、`Effekseer/` 配下にアプリ本体と関連ツールがすでに配置されていることを前提としています。
