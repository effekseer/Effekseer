#!/bin/bash
set -e
set +x

: "${RDIR:=$PWD/EffekseerTool}"
BIN_DIR="$RDIR/Tool"
SAMPLE_DIR="$RDIR/Sample"

if ! command -v robocopy >/dev/null; then
    function robocopy {
        from=$1
        to=$2
        shift 2
        include=()
        for p in "$@"; do
            [ "$p" = "/S" ] && continue
            include+=("--include=$p")
            echo "Include $p"
        done
        echo "Copy from $from to $to"
        # Ensure directories are included so files can be copied recursively
        rsync -av --include='*/' "${include[@]}" --exclude='*' "$from/" "$to"
    }
fi

rm -rf "$RDIR"
mkdir -p "$BIN_DIR"

echo "Compile Editor"

echo "Copy application"
cp -Rv Dev/release "$BIN_DIR"
rm -rf "$BIN_DIR/linux-x64" "$BIN_DIR/publish"

echo "Sample"
mkdir -p "$SAMPLE_DIR"
patterns=(*.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx)
robocopy Release/Sample "$SAMPLE_DIR" "${patterns[@]}" /S
robocopy ResourceData/samples "$SAMPLE_DIR" "${patterns[@]}" /S

echo "Readme"
cp -v readme_tool_win.txt "$RDIR/readme.txt"
cp -v docs/readme_sample.txt "$SAMPLE_DIR/readme.txt"
cp -v docs/Help_Ja.html "$RDIR/Help_Ja.html"
cp -v docs/Help_En.html "$RDIR/Help_En.html"
cp -v LICENSE_TOOL "$RDIR/LICENSE_TOOL"
