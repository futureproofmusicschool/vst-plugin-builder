# Installer — Package Plugin for Distribution

Create a macOS `.pkg` installer for a built plugin. The installer places VST3 and AU files in the system-wide plugin directories so any user/DAW on the machine can find them.

## Teach as you go

Briefly explain code signing and notarization as you do it. The user should leave understanding *why* macOS rejects unsigned plugins by default — this is the question every plugin developer eventually has to answer.

Specific moments to teach:

- **Why sign at all:** "macOS Gatekeeper blocks unsigned binaries from running, especially when downloaded from the internet. Without a valid signature, your users see a 'damaged' or 'unidentified developer' warning and can't install."
- **Two certificates, two purposes:** "Developer ID Application signs the plugin binary itself. Developer ID Installer signs the `.pkg` wrapper. Apple requires both — the signature on the wrapper is what Gatekeeper checks first."
- **Notarization:** "After signing, you submit the package to Apple. They scan it for malware and issue a 'ticket' confirming it's clean. The ticket is then 'stapled' to the package so offline machines can verify it without an internet check."
- **App-specific passwords:** "Notarization needs an app-specific password (different from your Apple ID password). You create one at appleid.apple.com and store it once in the keychain via `xcrun notarytool store-credentials`."

## Trigger

Common phrasings: "/installer", "/installer <PluginName>", "make a .pkg", "package it for distribution".

## Prerequisites

The plugin must already be built in Release mode. If artifacts are missing, tell the user to run `/deploy [PluginName]` first.

The user must have Apple Developer signing certificates installed. Find theirs with:
```bash
security find-identity -v -p codesigning
```

The signing identities should be exported as environment variables before running the build script:
```bash
export APP_SIGN_ID="Developer ID Application: Their Name (TEAMID)"
export PKG_SIGN_ID="Developer ID Installer: Their Name (TEAMID)"
```

## Process

The simplest approach is to delegate to the prebuilt script, which handles all the steps below:

```bash
./scripts/build-mac-installer.sh [PluginName]
```

The sections below describe what the script does, so the user understands the pipeline.

### Step 1: Identify plugin and verify artifacts

Read the plugin's `CMakeLists.txt` to extract:
- `PRODUCT_NAME` → display name
- `BUNDLE_ID` → bundle identifier base
- `COMPANY_NAME` → company name
- `VERSION` → version number from `project(... VERSION x.y.z)`

Check that Release build artifacts exist:

```bash
ls plugins/[PluginName]/build/[PluginName]_artefacts/Release/VST3/[DisplayName].vst3
ls plugins/[PluginName]/build/[PluginName]_artefacts/Release/AU/[DisplayName].component
```

If either is missing, stop and tell the user:
```
Build artifacts not found. Run /deploy [PluginName] first to create a Release build.
```

### Step 2: Create installer staging area

```bash
mkdir -p plugins/[PluginName]/installer/vst3-payload
mkdir -p plugins/[PluginName]/installer/au-payload
```

Copy the plugin bundles into the payload directories. Each payload directory represents what gets installed to the target location:

```bash
cp -R plugins/[PluginName]/build/[PluginName]_artefacts/Release/VST3/[DisplayName].vst3 \
      plugins/[PluginName]/installer/vst3-payload/

cp -R plugins/[PluginName]/build/[PluginName]_artefacts/Release/AU/[DisplayName].component \
      plugins/[PluginName]/installer/au-payload/
```

### Step 3: Build component packages

Create individual `.pkg` files for each plugin format using `pkgbuild`:

```bash
# VST3 package
pkgbuild --root plugins/[PluginName]/installer/vst3-payload \
         --install-location "/Library/Audio/Plug-Ins/VST3" \
         --identifier "[BundleID].vst3" \
         --version "[VERSION]" \
         plugins/[PluginName]/installer/vst3.pkg

# AU package
pkgbuild --root plugins/[PluginName]/installer/au-payload \
         --install-location "/Library/Audio/Plug-Ins/Components" \
         --identifier "[BundleID].au" \
         --version "[VERSION]" \
         plugins/[PluginName]/installer/au.pkg
```

### Step 4: Create distribution XML

Read the template from `templates/distribution.xml.template` and replace placeholders:

| Placeholder | Value |
|-------------|-------|
| `{{DISPLAY_NAME}}` | From CMakeLists.txt `PRODUCT_NAME` |
| `{{VERSION}}` | From CMakeLists.txt `project(... VERSION ...)` |
| `{{COMPANY_NAME}}` | From CMakeLists.txt `COMPANY_NAME` |
| `{{BUNDLE_ID}}` | From CMakeLists.txt `BUNDLE_ID` |

Write the filled template to `plugins/[PluginName]/installer/distribution.xml`.

### Step 5: Sign plugin binaries

Sign the plugin bundles with the Developer ID Application certificate before packaging:

```bash
codesign --force --deep --sign "$APP_SIGN_ID" \
         plugins/[PluginName]/installer/vst3-payload/[DisplayName].vst3

codesign --force --deep --sign "$APP_SIGN_ID" \
         plugins/[PluginName]/installer/au-payload/[DisplayName].component
```

### Step 6: Build final installer package

Combine the component packages into a single distributable `.pkg`:

```bash
productbuild --distribution plugins/[PluginName]/installer/distribution.xml \
             --package-path plugins/[PluginName]/installer/ \
             --version "[VERSION]" \
             plugins/[PluginName]/installer/[PluginName]-Unsigned.pkg
```

### Step 7: Sign the installer package

Sign the `.pkg` with the Developer ID Installer certificate:

```bash
productsign --sign "$PKG_SIGN_ID" \
            plugins/[PluginName]/installer/[PluginName]-Unsigned.pkg \
            plugins/[PluginName]/installer/[PluginName]-Installer.pkg
```

### Step 8: Notarize the installer

Submit to Apple for notarization. This requires an app-specific password stored in the keychain.

If not already set up, tell the user to create one at https://appleid.apple.com/account/manage and store it with:
```bash
xcrun notarytool store-credentials "notarytool-profile" \
      --apple-id "your-apple-id@example.com" \
      --team-id "YOUR_TEAM_ID" \
      --password "[APP_SPECIFIC_PASSWORD]"
```

Submit for notarization:
```bash
xcrun notarytool submit plugins/[PluginName]/installer/[PluginName]-Installer.pkg \
      --keychain-profile "notarytool-profile" \
      --wait
```

Staple the notarization ticket to the package:
```bash
xcrun stapler staple plugins/[PluginName]/installer/[PluginName]-Installer.pkg
```

### Step 9: Clean up intermediate files

Remove staging files, keep only the final signed installer:

```bash
rm -rf plugins/[PluginName]/installer/vst3-payload
rm -rf plugins/[PluginName]/installer/au-payload
rm -f  plugins/[PluginName]/installer/vst3.pkg
rm -f  plugins/[PluginName]/installer/au.pkg
rm -f  plugins/[PluginName]/installer/distribution.xml
rm -f  plugins/[PluginName]/installer/[PluginName]-Unsigned.pkg
```

### Step 10: Report

```
[PluginName] installer created successfully (signed & notarized).

Installer: plugins/[PluginName]/installer/[PluginName]-Installer.pkg
Size: [file size]

When a user opens this .pkg:
  1. macOS Installer wizard launches (no Gatekeeper warning)
  2. They choose which formats to install (VST3, AU)
  3. Admin password required (installs to /Library/Audio/Plug-Ins/)
  4. After install, they rescan plugins in their DAW

Installed locations:
  VST3: /Library/Audio/Plug-Ins/VST3/[DisplayName].vst3
  AU:   /Library/Audio/Plug-Ins/Components/[DisplayName].component
```

## Install locations

The installer uses system-wide paths (not per-user `~/Library`) so all user accounts on the machine can access the plugins:

| Format | Install Path |
|--------|-------------|
| VST3 | `/Library/Audio/Plug-Ins/VST3/` |
| AU | `/Library/Audio/Plug-Ins/Components/` |

## Notes

- `pkgbuild`, `productbuild`, and `productsign` are included with Xcode Command Line Tools.
- Plugin binaries are signed with Developer ID Application; the `.pkg` is signed with Developer ID Installer.
- After notarization and stapling, users will not see Gatekeeper warnings.
- The `installer/` directory inside each plugin is gitignored — installers are build artifacts, not source.
- Only VST3 and AU are included. The Standalone app is not packaged because users don't typically need it installed system-wide.
- For **cross-platform builds** (Mac + Windows), use the GitHub Actions workflow at `.github/workflows/build-installer.yml`. Trigger it manually or push a tag like `v1.0.0-PluginName`.
