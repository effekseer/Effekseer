#!/bin/sh
set -eu

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
profile="${2:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"
app_path="Effekseer/Effekseer.app"
dmg_path="Effekseer.dmg"
app_zip="Effekseer-app.zip"
package_dir="$(mktemp -d "${TMPDIR:-/tmp}/effekseer-dmg-package.XXXXXX")"
app_submit_out="$(mktemp "${TMPDIR:-/tmp}/effekseer-app-notarytool.XXXXXX")"
dmg_submit_out="$(mktemp "${TMPDIR:-/tmp}/effekseer-dmg-notarytool.XXXXXX")"

trap 'rm -rf "$package_dir"; rm -f "$app_zip" "$app_submit_out" "$dmg_submit_out"' EXIT

submit_and_check_log() {
    artifact="$1"
    label="$2"
    profile="$3"
    output_log="$4"
    submit_out="$5"

    submitted=0
    if xcrun notarytool submit "$artifact" --keychain-profile "$profile" --wait --output-format json >"$submit_out"; then
        submitted=1
    fi
    cat "$submit_out"
    request_id=$(python3 -c 'import json, sys; print(json.load(open(sys.argv[1]))["id"])' "$submit_out")
    echo "Downloading notarization log for $label: $output_log"
    xcrun notarytool log "$request_id" --keychain-profile "$profile" "$output_log"
    test "$submitted" = 1
    # An accepted submission can still contain warnings. Keep and check the
    # log on successful submissions as well as failures.
    python3 - "$output_log" <<'PY'
import json
import sys
with open(sys.argv[1]) as stream:
    log = json.load(stream)
if log.get("status") != "Accepted" or log.get("issues"):
    sys.exit("Notarization was rejected or reported issues: " + sys.argv[1])
PY
}

echo "DevID $1 Profile $profile"

python3 "$script_dir/sign_app.py" "$app_path" "$1"

echo "Creating zip for app notarization: $app_zip"
ditto -c -k --keepParent "$app_path" "$app_zip"

echo "Notarizing app bundle archive: $app_zip"
submit_and_check_log "$app_zip" "app bundle" "$profile" "Effekseer-app-notarytool-log.json" "$app_submit_out"

echo "Stapling app bundle: $app_path"
xcrun stapler staple "$app_path"
xcrun stapler validate "$app_path"
spctl --assess --type execute --verbose=4 "$app_path"

echo "Preparing dmg staging directory: $package_dir"
cp -R "$app_path" "$package_dir/"
ln -s /Applications "$package_dir/Applications"

echo "Creating dmg from stapled app bundle: $dmg_path"
hdiutil create "$dmg_path" -volname "Effekseer" -srcfolder "$package_dir"

echo "Signing dmg: $dmg_path"
codesign --force --sign "$1" --timestamp "$dmg_path"

echo "Verifying dmg signature"
codesign --verify --verbose=4 "$dmg_path"
echo "Notarizing dmg: $dmg_path"
submit_and_check_log "$dmg_path" "dmg" "$profile" "Effekseer-dmg-notarytool-log.json" "$dmg_submit_out"

echo "Stapling dmg: $dmg_path"
xcrun stapler staple "$dmg_path"
xcrun stapler validate "$dmg_path"
