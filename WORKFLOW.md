# VST Plugin Builder Workflow

How to go from idea to installed plugin, step by step.

---

## Quick Start

```
cd ~/Documents/GitHub/vst-plugin-builder
claude
```

Then follow the four phases below.

---

## Phase 1: Dream

```
/dream
```

or

```
/dream "shimmer reverb with pitch-shifted tails"
```

**What happens:** You describe what you want the plugin to do in plain language. Claude asks follow-up questions about the sound, parameters, and UI. No technical knowledge needed.

**What you get:**
- `plugins/YourPlugin/.ideas/creative-brief.md` — What, who, why
- `plugins/YourPlugin/.ideas/parameter-spec.md` — Every knob and slider with ranges
- `plugins/YourPlugin/.ideas/ui-mockup.md` — Layout description

**Tips:**
- Describe sounds, not code. "I want it to sound like playing guitar underwater" is perfect.
- Reference plugins you like. "Similar to Valhalla Shimmer but with a darker tone."
- Don't overthink it. You can always come back and revise.

**When you're done:** Review the specs. If something's off, say so. Once you're happy, move to Phase 2.

---

## Phase 2: Plan

```
/plan YourPlugin
```

**What happens:** Claude reads your specs and designs the technical architecture: signal flow, DSP blocks, JUCE modules needed, and a CMakeLists.txt.

**What you get:**
- `plugins/YourPlugin/.ideas/architecture.md` — Signal flow and DSP design
- `plugins/YourPlugin/CMakeLists.txt` — Build configuration
- `plugins/YourPlugin/Source/` — Starter source files that compile

**Important:** At the end of this phase, the plugin should compile as an empty shell (passes audio through, no processing yet). Claude will verify the build before finishing.

**Tips:**
- If you have opinions about specific DSP approaches, share them.
- Claude uses the JUCE Docs MCP server to look up real APIs, so the architecture will reference actual JUCE classes.

**When you're done:** You have a compiling project. Move to Phase 3.

---

## Phase 3: Implement

```
/implement YourPlugin
```

or resume a specific stage:

```
/implement YourPlugin stage2
```

**What happens:** Claude builds the plugin in three stages, checking the build after each one.

### Stage 1: Foundation
- Project compiles and loads as a plugin
- Audio passes through unchanged
- Build verified

### Stage 2: DSP
- Audio processing implemented
- All parameters connected to DSP
- Plugin state saves and restores
- Build verified

### Stage 3: UI
- Custom editor (stock JUCE LookAndFeel, or your own brand if supplied during /dream)
- All controls wired to parameters
- Build verified

**Claude will ask for confirmation between stages.** This is your chance to test in a DAW and give feedback before moving on.

**Tips:**
- After Stage 2, load the plugin in your DAW and test the sound. If something's wrong, say so before moving to UI work.
- If a build fails 3 times on the same error, Claude will stop and reassess rather than keep patching.

**When you're done:** You have a fully functional plugin with a branded UI.

---

## Phase 4: Deploy

```
/deploy YourPlugin
```

**What happens:** Clean Release build, installs to your system plugin directories.

**What you get:**
- VST3 installed to `~/Library/Audio/Plug-Ins/VST3/`
- AU installed to `~/Library/Audio/Plug-Ins/Components/` (requires full Xcode)
- Standalone app in the build directory

**After deploying:**
1. Open your DAW
2. Rescan plugins if needed
3. Find your plugin under your company name (set in `CMakeLists.txt`)
4. Test on a track

---

## Validation (Optional but Recommended)

If you have pluginval installed (`brew install pluginval`), you can validate at any point after a successful build:

```bash
pluginval --validate ~/Library/Audio/Plug-Ins/VST3/YourPlugin.vst3
```

This catches issues that might not show up in normal use:
- Parameter edge cases
- State save/restore bugs
- Thread safety violations
- Unusual buffer sizes

**When to validate:**
- After Stage 1 (basic load test)
- After Stage 2 (DSP stress test)
- After Stage 3 (full validation before release)
- After any significant changes

---

## Iterating on an Existing Plugin

You don't have to start from scratch each time. To modify an existing plugin:

1. Describe the change you want in plain language
2. Claude reads the existing source and specs
3. Makes the change, rebuilds, and verifies

For example:
- "Add a low-pass filter to the Underwater plugin"
- "Change the reverb tail to be longer by default"
- "Make the UI wider and add a waveform display"

---

## Rebuilding After Manual Edits

If you edit source files directly (outside of Claude), rebuild with:

```bash
cd plugins/YourPlugin
cmake --build build --config Release
```

Or for a clean rebuild:

```bash
cd plugins/YourPlugin
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## Typical Session

A full plugin build usually goes like this:

```
You:    /dream "tape delay with wow and flutter"
Claude: [asks about character, parameters, UI]
You:    [describe what you want]
Claude: [saves specs to .ideas/]

You:    /plan TapeDelay
Claude: [designs DSP, creates CMakeLists, verifies build]

You:    /implement TapeDelay
Claude: [Stage 1: foundation — builds ✓]
Claude: [Stage 2: DSP — builds ✓] "Try it in your DAW"
You:    "The flutter is too subtle"
Claude: [adjusts, rebuilds ✓]
Claude: [Stage 3: UI — builds ✓]

You:    /deploy TapeDelay
Claude: [clean build, installs to plugin directories]
```

Total time depends on complexity, but a simple effect can go from idea to installed in one session.

---

## Troubleshooting

### Plugin doesn't show up in DAW
- Rescan plugins in your DAW's preferences
- Check that the `.vst3` file exists: `ls ~/Library/Audio/Plug-Ins/VST3/`

### AU build fails
- You need Xcode (full app from App Store), not just Command Line Tools
- Check: `xcode-select -p` should show `/Applications/Xcode.app/Contents/Developer`

### Build fails with CMake errors
- Make sure JUCE submodule is initialized: `git submodule update --init --recursive`
- Check CMake version: `cmake --version` (need 3.22+)

### Plugin crashes in DAW
- Run pluginval to identify the issue
- Check that `processBlock` doesn't allocate memory or do I/O
- Try the Standalone build first (easier to debug)
