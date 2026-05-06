#!/bin/bash
set -euo pipefail

# Build a signed and notarized macOS .pkg installer for a plugin.
# Usage: ./scripts/build-mac-installer.sh <PluginName>
# Example: ./scripts/build-mac-installer.sh ReverbATron

if [ -z "${1:-}" ]; then
    echo "Usage: $0 <PluginName>"
    echo "Example: $0 ReverbATron"
    exit 1
fi

PLUGIN="$1"
PLUGIN_DIR="plugins/$PLUGIN"
CMAKE_FILE="$PLUGIN_DIR/CMakeLists.txt"

# Verify plugin exists
if [ ! -f "$CMAKE_FILE" ]; then
    echo "Error: $CMAKE_FILE not found"
    exit 1
fi

# Extract metadata from CMakeLists.txt
VERSION=$(grep -o 'VERSION [0-9]\+\.[0-9]\+\.[0-9]\+' "$CMAKE_FILE" | head -1 | awk '{print $2}')
DISPLAY_NAME=$(grep 'PRODUCT_NAME' "$CMAKE_FILE" | sed 's/.*PRODUCT_NAME[[:space:]]*"\([^"]*\)".*/\1/')
COMPANY_NAME=$(grep 'COMPANY_NAME' "$CMAKE_FILE" | sed 's/.*COMPANY_NAME[[:space:]]*"\([^"]*\)".*/\1/')
BUNDLE_ID=$(grep 'BUNDLE_ID' "$CMAKE_FILE" | sed 's/.*BUNDLE_ID[[:space:]]*"\([^"]*\)".*/\1/')

VERSION="${VERSION:-1.0.0}"
DISPLAY_NAME="${DISPLAY_NAME:-$PLUGIN}"
COMPANY_NAME="${COMPANY_NAME:-MyCompany}"
BUNDLE_ID="${BUNDLE_ID:-com.mycompany.$(echo "$PLUGIN" | tr '[:upper:]' '[:lower:]')}"

if [ "$COMPANY_NAME" = "MyCompany" ]; then
    echo "Warning: COMPANY_NAME not set in $CMAKE_FILE. Falling back to placeholder 'MyCompany'." >&2
fi

# Apple Developer signing identities. Set via env or override here.
# Find yours with: security find-identity -v -p codesigning
APP_SIGN_ID="${APP_SIGN_ID:-}"
PKG_SIGN_ID="${PKG_SIGN_ID:-}"

if [ -z "$APP_SIGN_ID" ] || [ -z "$PKG_SIGN_ID" ]; then
    echo "Error: APP_SIGN_ID and PKG_SIGN_ID must be set." >&2
    echo "Example:" >&2
    echo '  export APP_SIGN_ID="Developer ID Application: Your Name (TEAMID)"' >&2
    echo '  export PKG_SIGN_ID="Developer ID Installer: Your Name (TEAMID)"' >&2
    exit 1
fi

ARTIFACTS="$PLUGIN_DIR/build/${PLUGIN}_artefacts/Release"
INSTALLER_DIR="$PLUGIN_DIR/installer"
OUTPUT_PKG="$INSTALLER_DIR/${PLUGIN}-Installer-Mac.pkg"

echo "=== Building installer for $DISPLAY_NAME v$VERSION ==="
echo "    Plugin: $PLUGIN"
echo "    Bundle: $BUNDLE_ID"
echo ""

# Check build artifacts exist
if [ ! -d "$ARTIFACTS/VST3/${DISPLAY_NAME}.vst3" ]; then
    echo "Error: VST3 artifact not found at $ARTIFACTS/VST3/${DISPLAY_NAME}.vst3"
    echo "Error: Build artifacts not found. Building now..."
    cd "$PLUGIN_DIR"
    rm -rf build
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
    cmake --build build --config Release
    cd - > /dev/null
fi
if [ ! -d "$ARTIFACTS/AU/${DISPLAY_NAME}.component" ]; then
    echo "Error: AU artifact not found at $ARTIFACTS/AU/${DISPLAY_NAME}.component"
    exit 1
fi

# Stage payloads
echo "[1/7] Staging payloads..."
rm -rf "$INSTALLER_DIR"
mkdir -p "$INSTALLER_DIR/vst3-payload" "$INSTALLER_DIR/au-payload"
cp -R "$ARTIFACTS/VST3/${DISPLAY_NAME}.vst3" "$INSTALLER_DIR/vst3-payload/"
cp -R "$ARTIFACTS/AU/${DISPLAY_NAME}.component" "$INSTALLER_DIR/au-payload/"

# Sign plugin binaries
echo "[2/7] Signing plugin binaries..."
codesign --force --deep --sign "$APP_SIGN_ID" "$INSTALLER_DIR/vst3-payload/${DISPLAY_NAME}.vst3"
codesign --force --deep --sign "$APP_SIGN_ID" "$INSTALLER_DIR/au-payload/${DISPLAY_NAME}.component"

# Build component packages
echo "[3/7] Building component packages..."
pkgbuild --root "$INSTALLER_DIR/vst3-payload" \
         --install-location "/Library/Audio/Plug-Ins/VST3" \
         --identifier "${BUNDLE_ID}.vst3" \
         --version "$VERSION" \
         "$INSTALLER_DIR/vst3.pkg" > /dev/null 2>&1

pkgbuild --root "$INSTALLER_DIR/au-payload" \
         --install-location "/Library/Audio/Plug-Ins/Components" \
         --identifier "${BUNDLE_ID}.au" \
         --version "$VERSION" \
         "$INSTALLER_DIR/au.pkg" > /dev/null 2>&1

# Fill distribution template
echo "[4/7] Creating distribution..."
sed -e "s/{{DISPLAY_NAME}}/$DISPLAY_NAME/g" \
    -e "s/{{VERSION}}/$VERSION/g" \
    -e "s/{{COMPANY_NAME}}/$COMPANY_NAME/g" \
    -e "s/{{BUNDLE_ID}}/$BUNDLE_ID/g" \
    templates/distribution.xml.template > "$INSTALLER_DIR/distribution.xml"

# Build and sign installer
echo "[5/7] Building and signing .pkg..."
productbuild --distribution "$INSTALLER_DIR/distribution.xml" \
             --package-path "$INSTALLER_DIR/" \
             --version "$VERSION" \
             "$INSTALLER_DIR/${PLUGIN}-Unsigned.pkg" > /dev/null 2>&1

productsign --sign "$PKG_SIGN_ID" \
            "$INSTALLER_DIR/${PLUGIN}-Unsigned.pkg" \
            "$OUTPUT_PKG" > /dev/null 2>&1

# Notarize
echo "[6/7] Notarizing with Apple (this may take a few minutes)..."
xcrun notarytool submit "$OUTPUT_PKG" \
      --keychain-profile "notarytool-profile" \
      --wait 2>&1 | grep -E "status:|id:"

# Staple
echo "[7/7] Stapling notarization ticket..."
xcrun stapler staple "$OUTPUT_PKG" > /dev/null 2>&1

# Clean up intermediate files
rm -rf "$INSTALLER_DIR/vst3-payload" "$INSTALLER_DIR/au-payload"
rm -f "$INSTALLER_DIR/vst3.pkg" "$INSTALLER_DIR/au.pkg"
rm -f "$INSTALLER_DIR/distribution.xml" "$INSTALLER_DIR/${PLUGIN}-Unsigned.pkg"

# Report
SIZE=$(du -h "$OUTPUT_PKG" | awk '{print $1}')
echo ""
echo "=== Done ==="
echo "Installer: $OUTPUT_PKG ($SIZE)"
echo ""
echo "Installs to:"
echo "  VST3: /Library/Audio/Plug-Ins/VST3/${DISPLAY_NAME}.vst3"
echo "  AU:   /Library/Audio/Plug-Ins/Components/${DISPLAY_NAME}.component"
