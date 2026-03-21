#!/bin/sh
set -eu

profile="${2:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"

echo "RequestID $1 Profile $profile"
xcrun notarytool info "$1" --keychain-profile "$profile"
