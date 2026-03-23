# Notarization_Mac

## English

This folder contains helper scripts for codesigning, notarizing, and stapling the macOS build of Effekseer.

### Files

- `Effekseer_cert.sh`
  - Signs bundled executables that have any executable bit set with the app entitlements, signs `.dylib` files without entitlements, then signs the app bundle, creates a temporary zip for app notarization, recreates `Effekseer.dmg` with the app bundle and an `Applications` symlink, and submits both for notarization with `xcrun notarytool` using a Keychain profile.
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
- `notarytool` does not accept a raw `.app` bundle, so the script creates a temporary zip archive for app notarization and removes it afterward.
- `Effekseer_notarytool_setup.sh` is the only script that handles the App-specific password, and it stores that secret in the Keychain for reuse.
- `Effekseer_notarytool_log.sh` is useful when `notarytool submit` returns a rejection and you need the JSON issue report.
- When `Effekseer_cert.sh` fails, it writes notarization logs to `Effekseer-app-notarytool-log.json` or `Effekseer-dmg-notarytool-log.json` if a request ID can be recovered.
- `Effekseer_cert.sh` now performs this full order: notarize app, staple app, recreate the DMG from the stapled app and an `Applications` symlink, notarize DMG, staple DMG.
- `Effekseer_cert.sh` signs bundled executables and `.dylib` files first, then signs the app bundle and the DMG.
- If the app still shows the Gatekeeper warning, check whether the bundle was modified after signing or whether the quarantine attribute is still present.

### GitHub Actions

This repository's CI can notarize the macOS tool on `push` and `release` builds by reusing these scripts.
Configure the following GitHub Actions secrets before enabling the workflow on your repository:

- `APPLE_CERT_P12_BASE64`
  - Base64-encoded `.p12` file that contains the `Developer ID Application` certificate.
- `APPLE_CERT_P12_PASSWORD`
  - Password for the `.p12` file.
- `APPLE_SIGNING_IDENTITY`
  - Full certificate name, for example `Developer ID Application: Your Name (TEAMID)`.
- `APPLE_NOTARY_APPLE_ID`
  - Apple ID used for notarization.
- `APPLE_NOTARY_TEAM_ID`
  - Apple Developer Team ID.
- `APPLE_NOTARY_APP_PASSWORD`
  - App-specific password for the Apple ID above.

The workflow generates a temporary keychain password on the runner, so no extra secret is required for that.

The workflow skips notarization on `pull_request` because repository secrets are not available there.

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
- If notarization complains about a specific bundled executable such as `createdump`, the leaf binary likely kept an ad hoc or incomplete signature. Re-sign the leaf binary before the app bundle.
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
  - 実行ビットが付いた同梱ファイルには app の entitlements を付けて署名し、`.dylib` は entitlements なしで署名します。その後でアプリ本体に署名し、アプリ本体と `Applications` シンボリックリンクを含む `Effekseer.dmg` を再作成して `xcrun notarytool` で Notarization を申請します。
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
- `notarytool` は `.app` 直送を受け付けないため、スクリプトは一時的に zip 化してから app を notarize し、終了後に削除します。
- `Effekseer_notarytool_setup.sh` だけが App-specific password を扱い、その後はキーチェーン内の profile を再利用します。
- `Effekseer_notarytool_log.sh` は、`notarytool submit` が拒否されたときの JSON 形式の issue report を確認するのに便利です。
- `Effekseer_cert.sh` が失敗した場合、Request ID を取得できれば `Effekseer-app-notarytool-log.json` または `Effekseer-dmg-notarytool-log.json` にログを保存します。
- `Effekseer_cert.sh` は、`app` を Notarization → staple → `Applications` リンク付き DMG 再作成 → DMG Notarization → DMG staple の順で処理します。
- `Effekseer_cert.sh` は、まず leaf バイナリを `--options runtime` と `--timestamp` 付きで署名し、その後に app bundle を署名します。
- `Effekseer_cert.sh` は、`-perm -u+x -o -perm -g+x -o -perm -o+x` で拾える実行ビット付きファイルを署名対象にします。
- `Effekseer_cert.sh` は、`Effekseer` のような Rosetta 下で動く leaf 実行ファイルに `com.apple.security.cs.allow-jit` などの entitlements を付けます。
- 起動できない場合は、署名の破損、Gatekeeper の拒否、quarantine 属性の残存を順に確認してください。

### GitHub Actions

このリポジトリの CI では、これらのスクリプトを使って `push` と `release` の macOS ビルドを公証できます。
有効化する前に、リポジトリの GitHub Actions secrets に次を設定してください。

- `APPLE_CERT_P12_BASE64`
  - `Developer ID Application` 証明書を含む `.p12` を base64 化した文字列。
- `APPLE_CERT_P12_PASSWORD`
  - `.p12` のパスワード。
- `APPLE_SIGNING_IDENTITY`
  - 証明書のフルネーム。例: `Developer ID Application: Your Name (TEAMID)`。
- `APPLE_NOTARY_APPLE_ID`
  - notarization に使う Apple ID。
- `APPLE_NOTARY_TEAM_ID`
  - Apple Developer の Team ID。
- `APPLE_NOTARY_APP_PASSWORD`
  - 上記 Apple ID の app-specific password。

一時キーチェーン用のパスワードは workflow 側で都度生成するため、追加の secret は不要です。

`pull_request` ではリポジトリ secrets が使えないため、公証ステップは自動的にスキップされます。

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
- `.NET` ベースの実行ファイルが起動直後に `EXC_BAD_ACCESS` で落ちる場合は、その実行ファイルに app entitlements が付いているか確認してください。`Effekseer` のような Rosetta 変換された leaf プロセスは、`allow-jit` などがないと起動時にクラッシュすることがあります。
- `createdump` のような同梱バイナリで notarization が止まる場合は、leaf バイナリの署名が ad hoc のままか、Developer ID 署名・timestamp・hardened runtime のいずれかが不足しています。app bundle の前に leaf バイナリを再署名してください。
- `spctl` で失敗する場合は、Gatekeeper が配布物を許可していません。
- `stapler validate` で失敗する場合は、app または dmg に notarization ticket が付いていません。
- `xattr` に `com.apple.quarantine` が残っている場合は、ダウンロード由来の quarantine が効いています。

さらに詳しく見る場合は、Gatekeeper のログを確認します。

```bash
log show --last 1h --predicate 'process == "syspolicyd"'
```
