#!/bin/sh
set -eu

profile="${2:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"
app_path="Effekseer/Effekseer.app"
dmg_path="Effekseer.dmg"
app_zip="Effekseer-app.zip"
app_submit_out="$(mktemp "${TMPDIR:-/tmp}/effekseer-app-notarytool.XXXXXX")"
dmg_submit_out="$(mktemp "${TMPDIR:-/tmp}/effekseer-dmg-notarytool.XXXXXX")"

trap 'rm -f "$app_zip" "$app_submit_out" "$dmg_submit_out"' EXIT

submit_and_log_failure() {
    artifact="$1"
    label="$2"
    profile="$3"
    output_log="$4"
    submit_out="$5"

    if xcrun notarytool submit "$artifact" --keychain-profile "$profile" --wait >"$submit_out" 2>&1; then
        cat "$submit_out"
        return 0
    fi

    echo "Notarization failed for $label" >&2
    cat "$submit_out" >&2

    request_id=$(sed -n 's/.*id:[[:space:]]*//p' "$submit_out" | head -n 1)
    if [ -n "$request_id" ]; then
        echo "Downloading notarization log for $label: $output_log" >&2
        if xcrun notarytool log "$request_id" --keychain-profile "$profile" "$output_log" >/dev/null 2>&1; then
            echo "Saved notarization log: $output_log" >&2
        else
            echo "Failed to download notarization log for request $request_id" >&2
        fi
    else
        echo "Could not find a request ID in the notarytool output for $label" >&2
    fi

    return 1
}

echo "DevID $1 Profile $profile"

for file in "$app_path"/Contents/Resources/*.dylib ; do
    [ -f "$file" ] || continue
    echo codesign "$file"
    codesign --force --verify --verbose --sign "$1" "$file" --deep --options runtime --entitlements entitlements.plist --timestamp
done

codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/EffekseerMaterialEditor" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/Effekseer" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/libfbxsdk.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/libEffekseerMaterialCompilerGL.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/mqoToEffekseerModelConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/libEffekseerMaterialCompilerMetal.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/fbxToEffekseerModelConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/Resources/tools/fbxToEffekseerCurveConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path/Contents/MacOS/EffekseerLauncher" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "$app_path" --deep --options runtime --entitlements entitlements.plist --timestamp

echo "Creating zip for app notarization: $app_zip"
ditto -c -k --keepParent "$app_path" "$app_zip"

echo "Notarizing app bundle archive: $app_zip"
submit_and_log_failure "$app_zip" "app bundle" "$profile" "Effekseer-app-notarytool-log.json" "$app_submit_out"

echo "Stapling app bundle: $app_path"
xcrun stapler staple "$app_path"

echo "Creating dmg from stapled app bundle: $dmg_path"
hdiutil create "$dmg_path" -volname "Effekseer" -srcfolder "Effekseer"

codesign --force --verify --verbose --sign "$1" "$dmg_path" --deep --options runtime --timestamp
echo "Notarizing dmg: $dmg_path"
submit_and_log_failure "$dmg_path" "dmg" "$profile" "Effekseer-dmg-notarytool-log.json" "$dmg_submit_out"

echo "Stapling dmg: $dmg_path"
xcrun stapler staple "$dmg_path"
