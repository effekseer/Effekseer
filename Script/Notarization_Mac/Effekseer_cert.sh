echo DevID $1 User $2 Pass $3

for file in Effekseer/Effekseer.app/Contents/Resources/*.dylib ; do
    [ -f "$file" ] || continue
    echo codesign "$file"
    codesign --force --verify --verbose --sign "$1" "$file" --deep --options runtime --entitlements entitlements.plist --timestamp
done

codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/EffekseerMaterialEditor" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/Effekseer" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/libfbxsdk.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/libEffekseerMaterialCompilerGL.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/mqoToEffekseerModelConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/libEffekseerMaterialCompilerMetal.dylib" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/fbxToEffekseerModelConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/Resources/tools/fbxToEffekseerCurveConverter" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app/Contents/MacOS/EffekseerLauncher" --deep --options runtime --entitlements entitlements.plist --timestamp
codesign --force --verify --verbose --sign "$1" "Effekseer/Effekseer.app" --deep --options runtime --entitlements entitlements.plist --timestamp

hdiutil create Effekseer.dmg -volname "Effekseer" -srcfolder "Effekseer"

codesign --force --verify --verbose --sign "$1" "Effekseer.dmg" --deep --options runtime --entitlements entitlements_dmg.plist --timestamp
xcrun altool --notarize-app -t osx -f "Effekseer.dmg" --primary-bundle-id "jp.co.Effekseer.Effekseer" -u "$2" -p "$3"