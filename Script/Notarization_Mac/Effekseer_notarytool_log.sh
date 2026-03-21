#!/bin/sh
set -eu

request_id="${1:-}"
profile="${2:-${NOTARYTOOL_PROFILE:-effekseer-notarytool}}"
output_file="${3:-notarytool_log.json}"

if [ -z "$request_id" ]; then
    echo "Usage: sh Effekseer_notarytool_log.sh <Request ID> [profile] [output file]" >&2
    exit 1
fi

echo "RequestID $request_id Profile $profile Output $output_file"
xcrun notarytool log "$request_id" --keychain-profile "$profile" "$output_file"
