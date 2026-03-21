#!/bin/sh
set -eu

apple_id="${1:-}"
team_id="${2:-}"
password="${3:-}"
profile="${4:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"

if [ -z "$apple_id" ] || [ -z "$team_id" ] || [ -z "$password" ]; then
    echo "Usage: sh Effekseer_notarytool_setup.sh <Apple ID> <Team ID> <App-specific password> [profile]" >&2
    exit 1
fi

echo "Storing credentials for profile $profile"
xcrun notarytool store-credentials "$profile" --apple-id "$apple_id" --team-id "$team_id" --password "$password"
