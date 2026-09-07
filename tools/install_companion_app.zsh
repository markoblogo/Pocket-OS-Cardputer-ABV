#!/bin/zsh
set -euo pipefail
ROOT="${0:A:h:h}"
DEST="${1:-/Applications/ABVx Companion.app}"
if [[ -e "$DEST" ]]; then
  print -u2 "Already exists: $DEST. Choose a different destination or move the old app first."
  exit 1
fi
PYTHON="${ABVX_COMPANION_PYTHON:-$(command -v python3)}"
PYTHON="$($PYTHON -c 'import sys;print(sys.executable)')"
STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
APP="$STAGE/ABVx Companion.app"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources/companion_ui"
cp "$ROOT"/tools/*.py "$APP/Contents/Resources/"
cp "$ROOT/tools/companion_ui/index.html" "$APP/Contents/Resources/companion_ui/"
cp "$ROOT/tools/mac/ABVx.icns" "$APP/Contents/Resources/ABVx.icns"
xcrun swiftc -O "$ROOT/tools/mac/Companion.swift" -o "$APP/Contents/MacOS/ABVx Companion" -framework Cocoa -framework WebKit
cat > "$APP/Contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>ABVx Companion</string>
<key>CFBundleIdentifier</key><string>org.abvx.cardputer.companion</string>
<key>CFBundleName</key><string>ABVx Companion</string>
<key>CFBundleDisplayName</key><string>Cardputer Companion</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleIconFile</key><string>ABVx.icns</string>
<key>CFBundleShortVersionString</key><string>0.3.0</string>
<key>CFBundleVersion</key><string>3</string>
<key>NSHighResolutionCapable</key><true/>
<key>NSAppTransportSecurity</key><dict><key>NSAllowsLocalNetworking</key><true/></dict>
</dict></plist>
PLIST
/usr/libexec/PlistBuddy -c "Add :ABVxPython string $PYTHON" "$APP/Contents/Info.plist"
/usr/libexec/PlistBuddy -c "Add :ABVxProjectRoot string $ROOT" "$APP/Contents/Info.plist"
codesign --force --deep --sign - "$APP"
ditto "$APP" "$DEST"
print "Installed: $DEST"
