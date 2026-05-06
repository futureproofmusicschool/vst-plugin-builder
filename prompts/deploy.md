# Deploy — Build and Install

Build the plugin in Release mode, install it to the system plugin directories, and provide the user with verification steps.

## Teach as you go

Briefly explain what's happening at each step. The user should finish this phase understanding what the three plugin formats are, where they live on disk, and why a DAW might need to rescan.

Specific moments to teach:

- **Plugin formats:** "AU is Apple's format — Logic Pro and GarageBand only load AU. VST3 is cross-DAW (Ableton, FL Studio, Cubase, Reaper, etc). Standalone is a regular app you can run without a DAW — useful for testing in isolation."
- **Install locations:** "macOS DAWs scan specific folders for plugins. We're installing to `~/Library/Audio/Plug-Ins/` — that's the per-user location, so you don't need admin rights. The system-wide location is `/Library/Audio/Plug-Ins/` and requires sudo."
- **Rescanning:** "Some DAWs cache the plugin list and won't notice new plugins until you trigger a rescan. Logic does this automatically; Ableton sometimes needs you to toggle 'Use VST3 Plug-In System Folders' off and on."
- **Code signing for AU:** "AU plugins must be code-signed to load in Logic. CMake handles this automatically when you have full Xcode installed (not just CLT). If AU silently fails to load, that's the usual culprit."

Keep each note short.

## Trigger

Common phrasings: "/deploy", "/deploy <PluginName>", "install it", "let's get it into my DAW".

## Process

### Step 1: Clean build

```bash
cd plugins/[PluginName]
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Tell the user why we do a clean Release build (vs incremental Debug): we want optimization on, and we want to make sure no stale artifacts are around.

**On build failure:** Consult `rules/known-issues.yaml` *before* trying fresh diagnosis. Grep the build output against each entry's `detect` patterns. If you find a match, apply the listed fix and increment that entry's `frequency` counter. If no match, diagnose normally — and once resolved, append a new entry to `known-issues.yaml` with a `detect` pattern future runs can match on.

### Step 2: Verify artifacts

Check that the plugin binaries were created:

```bash
ls -la build/[PluginName]_artefacts/Release/VST3/
ls -la build/[PluginName]_artefacts/Release/AU/
ls -la build/[PluginName]_artefacts/Release/Standalone/
```

### Step 3: Verify installation

With `COPY_PLUGIN_AFTER_BUILD=TRUE` in the CMakeLists, the plugins should auto-install to:

- **VST3:** `~/Library/Audio/Plug-Ins/VST3/[DisplayName].vst3`
- **AU:** `~/Library/Audio/Plug-Ins/Components/[DisplayName].component`

Verify:
```bash
ls -la ~/Library/Audio/Plug-Ins/VST3/ | grep [DisplayName]
ls -la ~/Library/Audio/Plug-Ins/Components/ | grep [DisplayName]
```

### Step 4: Pre-shipping checklist

Walk every item below in order. Each is a hard gate — do not skip any. If an item can't be verified now (e.g. user can't open their DAW right now), say so explicitly and mark it pending; don't silently skip.

1. **Release build succeeded** with no errors. (From Step 1.)
2. **Every requested format produced an artifact.** Check the `formats` list in `status.json`. Verify each: `.vst3`, `.component`, `.app`, `.clap` (if requested).
3. **Plugins installed to system directories.** (From Step 3.)
4. **pluginval at strictness 5 passes** — or has an explicit reason for skipping. Run:
   ```bash
   pluginval --strictness-level 5 --validate ~/Library/Audio/Plug-Ins/VST3/[DisplayName].vst3
   ```
   On failure, see `rules/known-issues.yaml` (entry `pluginval-strictness-5-fail`). Tell the user what `pluginval` checks: parameter range violations, threading bugs, state save/restore failures, audio buffer handling.
5. **Plugin loads in the target DAW** (user confirms). For AU plugins: Logic Pro. For VST3: Ableton, Reaper, or whatever the user has. For CLAP: Bitwig or Reaper.
6. **All parameters automate.** User moves each control, records automation, scrubs the timeline, and confirms the parameter follows.
7. **Plugin state persists across reload.** User closes and reopens the DAW project; parameter values are intact.
8. **No crash on plugin window close/reopen.** User opens the plugin's editor, closes it, reopens it, repeats 3x.

If any item fails, stop and fix before reporting success. Surface what was checked vs. what's pending in the report.

### Step 5: Report

Print:
```
[PluginName] deployed successfully.

Installed to:
  VST3: ~/Library/Audio/Plug-Ins/VST3/[DisplayName].vst3
  AU:   ~/Library/Audio/Plug-Ins/Components/[DisplayName].component

To test:
  1. Open your DAW (Ableton, Logic, etc.)
  2. Rescan plugins if needed
  3. Find "[DisplayName]" under the manufacturer name set in CMakeLists.txt
  4. Insert on a track and test

To rebuild after changes:
  cd plugins/[PluginName] && cmake --build build --config Release
```

### Step 6: Update `status.json`

If all items in Step 4 passed, update `plugins/[PluginName]/status.json`:

- Set `phase` to `"deployed"`.
- Update `last_build` (result, timestamp, config: "Release").
- Update `last_validation` with the pluginval result and strictness.

If any checklist item is pending or failed, leave `phase` at its previous value and record the failure in `last_build` or `last_validation`.

## Notes

- AU plugins require Xcode (full app) for code signing. If only Command Line Tools is installed, AU may silently fail to load in Logic Pro.
- After deploying, the DAW may need a plugin rescan.
- The Standalone version can be run directly for quick testing without a DAW.
