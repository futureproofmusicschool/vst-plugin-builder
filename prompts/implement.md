# Implement — Build Orchestration

Take a planned plugin (from `prompts/plan.md`) and build it in three stages, validating after each one. This is where the actual C++ code gets written.

## Teach as you go

This phase is where the most teaching happens. Before each substantive code block, narrate in 1–3 sentences what it does and what JUCE rule it follows. The goal: by the end of the build, the user should understand *how a plugin works*, not just have a binary.

Specific moments to teach (don't skip these):

- **Entering `prepareToPlay`:** "This runs once before audio starts. It's where we allocate buffers and set sample rate — never inside `processBlock`."
- **Entering `processBlock`:** "This is the real-time hot path — it runs every audio buffer (~5 ms at 44.1 kHz / 256 samples). Three rules: no allocations, no locks, no I/O. Breaking any of them causes audible glitches."
- **Wiring APVTS:** "We're using `AudioProcessorValueTreeState` because it gives us automation, undo, and state save/restore for free. Without it, we'd have to wire all of that up by hand."
- **State save/restore:** "DAWs ask plugins to serialize their state when projects are saved. APVTS handles this for parameters; you only need extra code for non-parameter state."
- **UI binding (`SliderAttachment`):** "This keeps the knob and the parameter in sync — when one changes, the other updates. No manual listener code."
- **`juce::dsp::ProcessSpec`:** "We pass this to each DSP block in `prepareToPlay` so they know the sample rate, max buffer size, and channel count up front. They allocate based on these values, then never allocate again."

Keep each explanation short — 1–3 sentences. If the user is in a hurry they can scroll past; if they're learning, the lesson accumulates over the course of the build.

## Trigger

Common phrasings: "/implement", "/implement <PluginName>", "/implement <PluginName> stage2", "let's start coding".

## Prerequisites

The plugin must have:
- `.ideas/architecture.md` (from `/plan`)
- `CMakeLists.txt` (from `/plan`)
- Source stubs that compile (from `/plan`)

## Resume from `status.json`

Before starting, read `plugins/[PluginName]/status.json` if it exists (schema in `rules/status-schema.md`). Use the `phase` value to skip ahead:

| `phase` | Action |
|---------|--------|
| `plan_complete` or earlier | Start at Stage 1. |
| `implement.stage_1_foundation` | Skip Stage 1 verification, jump to Stage 2. Confirm with the user first ("status says Stage 1 is done — proceed to Stage 2?") so they can override. |
| `implement.stage_2_dsp` | Skip to Stage 3. Confirm. |
| `implement.stage_3_ui` or `implement_complete` | Ask the user what they want to redo. |

Update `status.json` after each stage's validation passes (or fails) — see the end of each stage below.

## Process

### Stage 1: Foundation (Build System)

**Goal:** Plugin compiles, loads in a DAW, passes audio through unchanged.

1. Verify `CMakeLists.txt` is correct.
2. Verify source files compile:
   ```bash
   cd plugins/[PluginName]
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```
3. Check the plugin binary was created in `build/[PluginName]_artefacts/`.
4. Verify `COPY_PLUGIN_AFTER_BUILD` placed it in the system plugin directory.

**Validation:**
- Build succeeds with zero errors.
- Plugin binary exists.
- If `pluginval` is installed: `pluginval --validate [path to .vst3]`.

**Update `status.json`:** set `phase` to `"implement.stage_1_foundation"` and update `last_build`.

**Ask the user to confirm** before proceeding to Stage 2. This is their chance to load it in a DAW and confirm it doesn't crash.

### Stage 2: DSP (Audio Engine)

**Goal:** Core audio processing works correctly with all parameters connected.

Read `plugins/[PluginName]/.ideas/architecture.md` for the signal flow and DSP design.

1. **Implement the DSP chain** in `PluginProcessor.cpp`. Before writing each piece, tell the user what it does:
   - Set up processing in `prepareToPlay` — allocate any buffers, prepare each `juce::dsp` block with the `ProcessSpec`.
   - Implement `processBlock` with the designed signal flow. Walk the user through the buffer flow stage-by-stage as you write it.
   - Connect APVTS parameters to DSP blocks. Explain that we read parameter values at the top of `processBlock` (atomic, real-time-safe) and apply them per-block, not per-sample, unless smoothing matters.
   - **If the plugin has a `tests/` directory** (i.e. `/plan` scaffolded tests), extract each non-trivial DSP algorithm into a standalone class under `Source/dsp/` (e.g. `Source/dsp/MyFilter.h`). The class should not depend on `juce::AudioProcessor` — it takes a sample rate, a block size, and raw buffers. `processBlock` instantiates and uses these. Then add unit tests in `tests/` that exercise the algorithm directly. See `rules/juce-patterns.md` "Testing" for the pattern.

2. **Follow DSP rules** (and tell the user why each rule exists):
   - No memory allocation in `processBlock` — `new`/`malloc` can block on system locks.
   - No mutex locks in the audio thread — they can priority-invert and cause dropouts.
   - No I/O operations in the audio thread — file/network calls can take milliseconds.
   - Pre-allocate buffers in `prepareToPlay`.
   - Use `juce::dsp::ProcessSpec` for initialization.

3. **State persistence:**
   - Implement `getStateInformation` and `setStateInformation`.
   - APVTS gives you parameter state for free; explain to the user why this is the easy path.

4. **Build and validate:**
   ```bash
   cd plugins/[PluginName]
   cmake --build build --config Release
   ```

**Validation:**
- Build succeeds.
- If `pluginval` is available: run validation.
- Parameters save and restore correctly.
- If the plugin has tests: `cd build && ctest --output-on-failure` passes.

**Update `status.json`:** set `phase` to `"implement.stage_2_dsp"` and update `last_build`.

**Ask the user to confirm** before proceeding to Stage 3. Have them play audio through the plugin and confirm it sounds right.

### Stage 3: UI (Editor)

**Goal:** Custom plugin UI with all controls functional.

Read the latest `plugins/[PluginName]/.ideas/ui-mockup-v*.md` (highest version). Older plugins may have `ui-mockup.md` — fall back to that.

1. **Implement the editor** in `PluginEditor.cpp`. Narrate as you go:
   - Set window size from spec.
   - Create controls (sliders, knobs, buttons) for each parameter — explain `juce::Slider::RotaryVerticalDrag` vs linear, when to use which.
   - Attach controls to APVTS parameters using `juce::AudioProcessorValueTreeState::SliderAttachment`. Explain how this avoids manual listener code.
   - Use stock `juce::LookAndFeel_V4` unless the user has supplied a custom brand (see `rules/brand.md`).
   - If the user has provided brand guidelines, apply their custom LookAndFeel and any header/branding they specified.

2. **Layout controls** using `resized()`:
   - Use `juce::FlexBox` or `juce::Grid` for clean layout, or manual bounds for simple cases.
   - Group related controls visually.
   - Add labels above or below each control.

3. **Build and validate:**
   ```bash
   cd plugins/[PluginName]
   cmake --build build --config Release
   ```

**Validation:**
- Build succeeds.
- UI renders correctly (user confirms visually).
- All controls respond to interaction.
- Parameters save/restore with UI state.

**Update `status.json`:** set `phase` to `"implement.stage_3_ui"` and update `last_build`. Then, after the user confirms the UI looks right, set `phase` to `"implement_complete"`.

## After all stages

Print a summary:
```
Plugin [Name] implementation complete.

Build: plugins/[PluginName]/build/
VST3:  plugins/[PluginName]/build/[Name]_artefacts/VST3/[Name].vst3
AU:    plugins/[PluginName]/build/[Name]_artefacts/AU/[Name].component

Next: Run /deploy to install and test in your DAW.
```

## Notes

- Use the JUCE Docs MCP server to look up class APIs while coding. Do not guess class names or method signatures — fabricated APIs cause silent runtime errors.
- Reference `plugins/underwater/Source/` for working examples.
- If a build fails 3 times on the same error, stop and reassess the architecture rather than patching forever. Ask the user.
- Never skip validation between stages.
