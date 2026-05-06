# Plan — Architecture Design

Take a plugin concept (from `prompts/dream.md`) and design the technical implementation: DSP signal flow, JUCE module selection, and a concrete build plan that produces a compiling skeleton.

## Teach as you go

Before each substantive step, explain in 1–3 sentences what you're about to do and why. When you choose a DSP architecture, say *why* in plain language and what the alternatives would have cost. When you pick a JUCE module, name what it provides and what it saves you from writing. The user should finish this phase with a clearer mental model of *how plugins are structured*, not just a CMakeLists.txt.

## Trigger

The user wants to plan an existing concept. Common phrasings: "/plan", "/plan <PluginName>", "let's plan it out", "design the architecture".

## Prerequisites

The plugin should have `.ideas/` specs from `prompts/dream.md`. If they don't exist, ask the user to run `/dream` first or provide the concept inline.

## Process

### Step 1: Read the specs

Read the plugin's `.ideas/` directory:
- `plugins/[PluginName]/.ideas/creative-brief.md`
- `plugins/[PluginName]/.ideas/parameter-spec.md`
- `plugins/[PluginName]/.ideas/ui-mockup-v*.md` — latest version (highest N). Plugins from before versioning may have `ui-mockup.md`; read that if no versioned files exist.

If `plugins/[PluginName]/status.json` exists, read it too — it tells you what `/dream` has already decided (UI framework, design iteration).

### Step 2: DSP architecture

Design the signal processing chain. Tell the user what you're doing and why before showing the diagram.

1. **Signal flow diagram** (text-based)
   ```
   Input -> [Stage 1] -> [Stage 2] -> ... -> Output
   ```

2. **DSP components needed** (filters, delays, oscillators, envelopes, etc.). For each, briefly explain its role:

   > "We'll need an envelope follower to track the input signal's amplitude over time. That feeds the compressor's gain reduction."

3. **JUCE classes to use** — look these up via the JUCE Docs MCP server before naming them. When you propose a class, say what it does and what the alternative would have been:

   > "We'll use `juce::dsp::IIR::Filter` for the EQ stages because it's a stable, low-CPU implementation. The alternative would be FFT-based filtering, which is more flexible but adds latency the user would feel as input lag."

4. **Processing considerations**
   - Sample rate handling (does any DSP block need to be re-prepared if sample rate changes?)
   - Latency requirements (zero-latency? small fixed delay OK?)
   - CPU budget estimate (rough order of magnitude)

### Step 3: JUCE module selection

Determine which JUCE modules are needed beyond the basics. Explain to the user what each module gives them.

| Module | What it provides | When to include |
|--------|------------------|-----------------|
| `juce_audio_utils` | Plugin host glue, audio device handling | Always |
| `juce_dsp` | Built-in DSP blocks (filters, oscillators, FFT) | Most plugins |
| `juce_gui_basics` | Sliders, buttons, layout primitives | Always (for UI) |
| `juce_graphics` | Drawing, fonts, colours | Always (for UI) |
| `juce_opengl` | Hardware-accelerated rendering | Only if doing custom 3D/animation |

### Step 4: Generate `CMakeLists.txt`

Read `templates/CMakeLists.txt.template` and fill in:

- `{{PLUGIN_NAME}}` — PascalCase name
- `{{PLUGIN_CODE}}` — unique 4-char code
- `{{DISPLAY_NAME}}` — human-readable name shown in DAWs
- `{{BUNDLE_SUFFIX}}` — lowercase for bundle ID
- `{{IS_SYNTH}}` — TRUE/FALSE
- `{{NEEDS_MIDI_INPUT}}` — TRUE/FALSE
- `{{NEEDS_MIDI_OUTPUT}}` — TRUE/FALSE
- `{{FORMATS}}` — space-separated list. Default: `AU VST3 Standalone CLAP`. Ask the user if they want to opt out of CLAP (e.g. on older JUCE versions). See `rules/juce-patterns.md` "Plugin Formats" for what each one is for.

Add any additional JUCE modules to `target_link_libraries`. Write the result to `plugins/[PluginName]/CMakeLists.txt`.

If the user has not provided a company name yet, ask now — `COMPANY_NAME`, manufacturer code (`PLUGIN_MANUFACTURER_CODE`, 4 chars), and bundle ID prefix. There are no defaults.

### Step 5: Save the architecture doc

Write `plugins/[PluginName]/.ideas/architecture.md`:

```markdown
# [Plugin Name] Architecture

## Signal Flow
[Diagram]

## DSP Components
[List of processing blocks with JUCE classes and their roles]

## Parameters → DSP Mapping
[Which parameters control which DSP blocks]

## Implementation Stages

### Stage 1: Foundation
- Project builds and loads in DAW
- Passes audio through (bypass)

### Stage 2: DSP
- Core audio processing working
- All parameters connected
- Sounds correct

### Stage 3: UI
- Custom editor with the agreed-on visual style
- All controls functional
- Meters/visualizations working

## Notes
[Any technical concerns, edge cases, or decisions]
```

### Step 6: Create source stubs

Create minimal `Source/PluginProcessor.h`, `Source/PluginProcessor.cpp`, `Source/PluginEditor.h`, `Source/PluginEditor.cpp` with:

- Correct class names
- APVTS setup with all parameters from the spec
- Empty `processBlock` (pass-through)
- Basic editor with correct window size

This ensures the project compiles from Stage 1, so the next phase has a known-good starting point.

### Step 7: (Optional) Scaffold tests

Ask the user whether they want unit tests scaffolded for this plugin. Default: yes — DSP-heavy plugins benefit most, but even simple plugins gain regression protection.

If yes:

1. Create `plugins/[PluginName]/tests/` directory.
2. Copy `templates/tests/CMakeLists.txt.template` to `plugins/[PluginName]/tests/CMakeLists.txt` and replace `[PluginName]` with the actual name.
3. Copy `templates/tests/sample-test.cpp.template` to `plugins/[PluginName]/tests/sample-test.cpp` and replace `[PluginName]`.
4. Tell the user that `/implement` Stage 2 will guide DSP into testable classes (see `rules/juce-patterns.md` "Testing").

The plugin's main `CMakeLists.txt` will pick up `tests/CMakeLists.txt` automatically — no further wiring needed.

### Step 8: Verify the build

Run:
```bash
cd plugins/[PluginName]
cmake -B build
cmake --build build
```

If the build fails, fix the issues before finishing. The plan is only complete when Stage 1 (empty plugin that loads) compiles successfully.

### Step 9: Update `status.json`

Update `plugins/[PluginName]/status.json` (create it if missing — see `rules/status-schema.md`):

- Set `phase` to `"plan_complete"`.
- Set `formats` to the array form of `{{FORMATS}}` (e.g. `["AU", "VST3", "Standalone", "CLAP"]`).
- Update `last_build` with the result of Step 8.

Failure to update `status.json` is non-blocking — log and continue.

## Notes

- Use the JUCE Docs MCP server to verify class names and method signatures. Do not guess.
- Reference `plugins/underwater/` as an example of a working plugin structure.
- Keep the architecture doc concise. It's a reference, not a textbook.
