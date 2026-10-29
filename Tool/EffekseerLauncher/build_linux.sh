#!/bin/sh
set -eu

cd "$(dirname "$0")"
SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH:-$(git log -1 --format=%ct)}"
export SOURCE_DATE_EPOCH
export TZ=UTC
export LC_ALL=C
cmake -B build_linux -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build_linux --config Release
