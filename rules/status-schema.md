# status.json Schema

Each plugin has a `status.json` at its root recording where it is in the lifecycle. Skills read it to skip preflight questions and resume cleanly. It is **not** a gate — skills may still proceed when fields are missing or stale; the file is an aid, not a contract.

## Location

```
plugins/[PluginName]/status.json
```

## Shape

```json
{
  "plugin_name": "ReverbATron",
  "phase": "implement.stage_2_dsp",
  "ui_framework": "native",
  "formats": ["AU", "VST3", "Standalone", "CLAP"],
  "design_iterations": ["v1", "v2"],
  "last_build": {
    "result": "ok",
    "timestamp": "2026-05-05T14:22:00Z",
    "config": "Release"
  },
  "last_validation": {
    "tool": "pluginval",
    "strictness": 5,
    "result": "ok",
    "timestamp": "2026-05-05T14:23:10Z"
  }
}
```

## Fields

| Field | Type | Written by | Meaning |
|-------|------|------------|---------|
| `plugin_name` | string | `/dream` | PascalCase name; matches the directory under `plugins/`. |
| `phase` | string | every skill | Current lifecycle phase. See values below. |
| `ui_framework` | string | `/dream` | `"native"` for stock JUCE LookAndFeel. (Reserved for future WebView option.) |
| `formats` | string[] | `/plan` | Plugin formats compiled — `"AU"`, `"VST3"`, `"Standalone"`, `"CLAP"`. |
| `design_iterations` | string[] | `/dream` | Versions of `ui-mockup-vN.md` that exist. Latest is the active one. |
| `last_build` | object | `/implement`, `/deploy` | Most recent build attempt. `result` is `"ok"` or `"failed"`. |
| `last_validation` | object | `/deploy` | Most recent pluginval run. `result` is `"ok"`, `"failed"`, or `"skipped"`. |

## Phase values

| Value | Meaning |
|-------|---------|
| `dream_complete` | Specs in `.ideas/` are written. |
| `plan_complete` | `architecture.md`, `CMakeLists.txt`, source stubs exist and Stage 1 builds. |
| `implement.stage_1_foundation` | Stage 1 verified — empty plugin loads. |
| `implement.stage_2_dsp` | Stage 2 verified — DSP works. |
| `implement.stage_3_ui` | Stage 3 verified — UI works. |
| `implement_complete` | All three stages done. |
| `deployed` | `/deploy` checklist passed. |
| `installer_built` | `.pkg` produced by `/installer`. |

## Behavior

- **Missing file:** treat as a fresh plugin. Skills may proceed and create the file when they have something to write.
- **Stale or wrong file:** skills should verify before trusting (e.g. `last_build.result == "ok"` doesn't mean the binary still exists).
- **Updates are non-blocking:** failure to update `status.json` is not a build failure. Log and continue.
- **No backwards-compat shims:** when fields evolve, update or remove the old key.

## Example progression for a new plugin

1. After `/dream`: `phase = "dream_complete"`, `design_iterations = ["v1"]`.
2. After `/plan`: `phase = "plan_complete"`, `formats = [...]` populated.
3. After `/implement` Stage 2: `phase = "implement.stage_2_dsp"`, `last_build` updated.
4. After `/deploy`: `phase = "deployed"`, `last_validation` populated.
