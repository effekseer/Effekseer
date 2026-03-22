#!/bin/sh
set -eu

profile="${2:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"
app_path="Effekseer/Effekseer.app"
dmg_path="Effekseer.dmg"

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

echo "Notarizing app bundle: $app_path"
xcrun notarytool submit "$app_path" --keychain-profile "$profile" --wait

echo "Stapling app bundle: $app_path"
xcrun stapler staple "$app_path"

echo "Creating dmg from stapled app bundle: $dmg_path"
hdiutil create "$dmg_path" -volname "Effekseer" -srcfolder "Effekseer"

codesign --force --verify --verbose --sign "$1" "$dmg_path" --deep --options runtime --timestamp
echo "Notarizing dmg: $dmg_path"
xcrun notarytool submit "$dmg_path" --keychain-profile "$profile" --wait

echo "Stapling dmg: $dmg_path"
xcrun stapler staple "$dmg_path"
