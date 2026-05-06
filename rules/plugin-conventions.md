# Plugin Conventions

Standards for audio plugins built in this workspace.

---

## Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Plugin name | PascalCase (code), Display Name (UI) | `Underwater`, "Underwater" |
| Plugin code | 4-char, meaningful abbreviation | `Udwt`, `Shrm` |
| Bundle ID | `com.[yourcompany].[lowercase]` | `com.mycompany.underwater` |
| Source files | `PluginProcessor.cpp/h`, `PluginEditor.cpp/h` | Standard JUCE names |

## Directory Structure

Each plugin follows this layout:

```
plugins/[PluginName]/
├── .ideas/                    <- Plugin concept and specs
│   ├── creative-brief.md      <- What the plugin does, who it's for
│   ├── parameter-spec.md      <- All parameters with ranges and defaults
│   ├── architecture.md        <- DSP signal flow and technical design
│   └── ui-mockup.md           <- UI layout description
├── Source/
│   ├── PluginProcessor.h
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h
│   └── PluginEditor.cpp
├── CMakeLists.txt
└── build/                     <- Build output (gitignored)
```

## Company Info

Company info is set per plugin — there are no defaults. The `/dream` skill will ask the user for:

| Field | Example |
|-------|---------|
| Company Name | `MyCompany` |
| Manufacturer Code | `Mcmp` (4 chars, unique per developer) |
| Bundle ID Pattern | `com.mycompany.[pluginname]` |

See `brand.md` for visual styling notes.

## Plugin Formats

Build all three formats by default:
- **AU** (Audio Unit) - macOS/Logic Pro
- **VST3** - Cross-platform (Ableton, FL Studio, etc.)
- **Standalone** - For testing without a DAW

## CMake Settings

Set these per plugin based on the user's company/project:

```cmake
COMPANY_NAME             "YourCompany"
PLUGIN_MANUFACTURER_CODE Xxxx    # 4-char unique code
FORMATS                  AU VST3 Standalone
COPY_PLUGIN_AFTER_BUILD  TRUE
```

## Parameter Guidelines

- Use `juce::AudioProcessorValueTreeState` for all parameters
- Parameter IDs: lowercase with underscores (`filter_cutoff`, `mix_amount`)
- Display names: Title Case ("Filter Cutoff", "Mix Amount")
- Always specify sensible defaults
- Group related parameters with `juce::AudioProcessorParameterGroup`

## UI Guidelines

- Default plugin window size: 600x400 (adjustable per plugin)
- Use stock JUCE `LookAndFeel_V4` unless the user supplies their own brand
- Readable at standard DPI and Retina
- See `rules/brand.md` for notes on adding a custom brand (colours, fonts, LookAndFeel)
