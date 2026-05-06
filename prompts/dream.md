# Dream — Plugin Ideation

Interactive concept development for a new audio plugin. Walk a user from initial idea to a concrete spec ready for `prompts/plan.md` to act on.

## Teach as you go

Before each substantive step, explain in 1–3 sentences what you're about to do and why. Treat the user as a curious learner — they want a plugin, but they also want to understand how plugins work. Keep explanations concrete and tied to what the user is deciding right now, not abstract theory. If they're in a hurry they can scroll past; if they're learning they have what they need.

## Trigger

The user wants to start a new plugin. Common phrasings: "I want to make a plugin", "/dream", "/dream <one-line concept>". If a concept is provided inline, start from it; otherwise begin with concept exploration.

## Process

### Step 1: Concept exploration

If the user did not give you a concept, ask:

1. **What type of plugin?** (Effect, Instrument/Synth, Utility, Analyzer)
2. **What's the core idea?** (One sentence: what does it DO to the sound?)
3. **Who is it for?** (Genre, skill level, workflow)
4. **What makes it different?** (Why build this instead of using an existing plugin?)

If they gave you a concept, restate it back in your own words so they can correct misunderstandings, then dig into specifics.

### Step 2: Parameter design

Work with the user to define the plugin's parameters.

For each parameter you propose, briefly explain *why* it's there and what musical effect it produces. Examples:

- "A wet/dry mix lets you blend the processed signal with the original — almost every effect needs one."
- "We'll need an attack time so the user can choose between snappy transient response and a smoother, more pillowy sound."

Capture for each parameter:

- **Name** (display name, Title Case)
- **ID** (lowercase with underscores, e.g. `filter_cutoff`)
- **Range and units** (e.g. 20 Hz – 20 kHz, or 0–100%)
- **Default value** (and why this is the right default)
- **Group** if it belongs with related controls

Distinguish core parameters (the main controls that define the effect) from secondary parameters (fine-tuning, options).

Encourage the user to describe sounds and behaviors, not implementation details. Translate their language into specific DSP parameters.

### Step 3: UI concept

Describe the plugin's visual layout:

- **Window size** (default 600x400 — adjust based on control count)
- **Layout** (which controls go where; ASCII sketch is fine)
- **Visual grouping** (sections, panels)
- **Special visual elements** (meters, waveform displays, XY pads, etc.)

No default brand is applied. Ask the user about visual style: do they want stock JUCE `LookAndFeel`, or do they want to define their own colours, fonts, and aesthetic? If they have brand guidelines, capture them in the ui-mockup spec. See `rules/brand.md` for what to capture and the optional bundled fonts in `resources/fonts/`.

### Step 4: Save specs

Create the plugin directory and save three spec files. The UI mockup is versioned from the start (`ui-mockup-v1.md`) so future iterations can sit alongside it as `ui-mockup-v2.md`, `ui-mockup-v3.md` for comparison or rollback.

```
plugins/[PluginName]/
├── .ideas/
│   ├── creative-brief.md      <- What it is, who it's for, why it's different
│   ├── parameter-spec.md      <- All parameters with ranges and defaults
│   └── ui-mockup-v1.md        <- Layout description and visual style (versioned)
└── status.json                <- Lifecycle state — initialized at end of this step
```

#### `creative-brief.md`

```markdown
# [Plugin Name]

## Concept
[One paragraph: what it does and why]

## Target User
[Who this is for]

## Key Differentiator
[What makes it unique]

## Audio Characteristics
[How it sounds / what it does to audio]
```

#### `parameter-spec.md`

```markdown
# [Plugin Name] Parameters

## Core Parameters
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| ... | ... | ... | ... | ... | ... |

## Secondary Parameters
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| ... | ... | ... | ... | ... | ... |
```

#### `ui-mockup-v1.md`

```markdown
# [Plugin Name] UI Layout — v1

## Window Size
[width] x [height]

## Layout
[ASCII sketch or prose description of control placement]

## Visual Notes
[Special elements, brand application notes]
```

### Step 5: Initialize `status.json`

Write `plugins/[PluginName]/status.json` with the initial lifecycle state. See `rules/status-schema.md` for the full schema.

```json
{
  "plugin_name": "[PluginName]",
  "phase": "dream_complete",
  "ui_framework": "native",
  "design_iterations": ["v1"]
}
```

Skip the `formats`, `last_build`, and `last_validation` keys — `/plan` and `/deploy` will fill those in later.

### Step 6: Summary

Print a concise summary of the plugin concept and confirm the user is happy before they move to `prompts/plan.md`.

## Notes

- This is a creative, conversational phase. Don't rush.
- Encourage the user to describe sounds and behaviors, not implementation.
- Use the JUCE Docs MCP server to verify feasibility of proposed features as the conversation goes.
- Reference `rules/plugin-conventions.md` for naming standards.
