# JUCE 8 Patterns and Best Practices

Technical reference for building audio plugins with JUCE in this workspace.

---

## Build System: CMake Only

This workspace uses CMake, not Projucer. Every plugin has its own `CMakeLists.txt`.

### Standard Build Commands

```bash
cd plugins/[PluginName]
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### CMakeLists.txt Structure

Every plugin CMakeLists.txt follows this pattern:

```cmake
cmake_minimum_required(VERSION 3.22)
project(PluginName VERSION 1.0.0)

# Reference shared JUCE submodule
add_subdirectory(../../JUCE JUCE)

# Define the plugin
juce_add_plugin(PluginName
    COMPANY_NAME             "YourCompany"
    PLUGIN_MANUFACTURER_CODE Ymfg    # 4-char unique manufacturer code
    PLUGIN_CODE              Xxxx    # 4-char unique plugin code
    FORMATS                  AU VST3 Standalone CLAP
    PRODUCT_NAME             "Plugin Name"
    BUNDLE_ID                "com.yourcompany.pluginname"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT           FALSE
    COPY_PLUGIN_AFTER_BUILD  TRUE
)

juce_generate_juce_header(PluginName)

target_sources(PluginName PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(PluginName PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PluginName
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
```

---

## Core Architecture

### PluginProcessor (Audio Engine)

The processor handles all DSP. Key methods:

```cpp
// Called before playback starts
void prepareToPlay(double sampleRate, int samplesPerBlock);

// Process audio - this is the hot path
void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

// Save/restore plugin state
void getStateInformation(juce::MemoryBlock& destData);
void setStateInformation(const void* data, int sizeInBytes);
```

### PluginEditor (UI)

The editor handles the GUI. Key methods:

```cpp
// Constructor - set up components
PluginEditor(PluginProcessor&);

// Draw custom graphics
void paint(juce::Graphics&);

// Handle window resize
void resized();
```

### Parameter System

Use `juce::AudioProcessorValueTreeState` (APVTS) for parameters:

```cpp
// In processor header
juce::AudioProcessorValueTreeState apvts;

// Create parameters
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain", 0.0f, 1.0f, 0.5f));
    return { params.begin(), params.end() };
}
```

---

## Common JUCE Modules

| Module | Purpose | When to Use |
|--------|---------|-------------|
| `juce_audio_utils` | Audio plugin basics | Always |
| `juce_dsp` | DSP building blocks (filters, oscillators, etc.) | Most plugins |
| `juce_gui_basics` | UI components | Always (for UI plugins) |
| `juce_graphics` | Drawing, fonts, colors | Always (for UI plugins) |
| `juce_audio_processors` | Plugin format support | Included by juce_audio_utils |

### Adding Modules

Add to `target_link_libraries` in CMakeLists.txt:

```cmake
target_link_libraries(PluginName
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl          # Add for OpenGL rendering
)
```

---

## Plugin Formats

The default `FORMATS` line includes `AU VST3 Standalone CLAP`:

| Format | Hosts | Notes |
|--------|-------|-------|
| `AU` | Logic Pro, GarageBand | Apple-only. Requires full Xcode for code signing. |
| `VST3` | Ableton, FL Studio, Cubase, Reaper, Bitwig, … | Cross-DAW. Works with CLT alone. |
| `Standalone` | (no host — runs as an app) | Useful for testing and live use. |
| `CLAP` | Bitwig, Reaper, FL Studio, MultitrackStudio | Modern open standard. Native in JUCE 8. |

If a target JUCE version doesn't recognize `CLAP` in `FORMATS`, drop it from the list — VST3 and AU cover most users. CLAP is the right choice when targeting Bitwig users or wanting per-voice modulation.

---

## Common Gotchas

### 1. JUCE_WEB_BROWSER and JUCE_USE_CURL

Always disable unless needed. They add unnecessary dependencies:

```cmake
target_compile_definitions(PluginName PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)
```

### 2. Plugin Code Must Be Unique

The 4-character `PLUGIN_CODE` must be unique across all your plugins. Use meaningful codes:
- Underwater: `Udwt`
- Reverb plugin: `Rvrb`
- Delay plugin: `Dlay`

### 3. COPY_PLUGIN_AFTER_BUILD

Set to `TRUE` to auto-install plugins to the system plugin directory after building. This means your DAW will see them immediately.

### 4. Build Directory

Always use out-of-source builds:
```bash
cmake -B build    # Creates build/ directory
cmake --build build
```

Never build in the source directory.

### 5. Xcode vs Command Line Tools

- **CLT only:** Can build VST3 and Standalone
- **Full Xcode:** Required for AU plugins (code signing)
- If AU build fails, check: `xcode-select -p` should point to `/Applications/Xcode.app/Contents/Developer`

### 6. DSP in processBlock

- Never allocate memory in `processBlock` (no `new`, no `std::vector` resize)
- Never lock mutexes
- Never do I/O (file, network, logging)
- Pre-allocate everything in `prepareToPlay`

---

## JUCE Docs MCP Server

This workspace includes a JUCE Docs MCP server that provides access to JUCE C++ class documentation. Use it to look up:
- Class APIs and method signatures
- Module contents
- Constructor parameters
- Inheritance hierarchies

The server auto-loads when Claude Code opens this repo (configured in `.mcp.json`).

---

## Plugin Validation

If pluginval is installed (`brew install pluginval`), validate after building:

```bash
pluginval --validate plugins/[PluginName]/build/[PluginName]_artefacts/VST3/[PluginName].vst3
```

This catches common issues:
- Parameter range violations
- Thread safety problems
- State save/restore failures
- Audio buffer handling errors

CI runs pluginval at strictness 5 against every build automatically — see `.github/workflows/build-installer.yml`.

---

## Testing (DSP unit tests)

Plugins can opt into Catch2-based unit tests by adding a `tests/` directory next to `Source/`. The plugin's main `CMakeLists.txt` includes `tests/CMakeLists.txt` automatically when the file exists, and CI runs `ctest` on plugins that have it.

### Making DSP testable

To unit-test DSP, extract algorithms into standalone classes that don't depend on `juce::AudioProcessor`:

```cpp
// Source/dsp/MyFilter.h
class MyFilter
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(float* buffer, int numSamples);
private:
    // ...
};
```

`PluginProcessor::processBlock` instantiates and uses these; tests instantiate them directly with synthetic input. Anything that takes a `juce::AudioBuffer` is harder to test cleanly — keep the algorithmic core in plain functions/classes.

### Setting up tests for a plugin

Copy the scaffold:

```bash
mkdir -p plugins/[PluginName]/tests
cp templates/tests/CMakeLists.txt.template plugins/[PluginName]/tests/CMakeLists.txt
cp templates/tests/sample-test.cpp.template plugins/[PluginName]/tests/sample-test.cpp
```

Replace `[PluginName]` in both files. Then build and run:

```bash
cd plugins/[PluginName]
cmake -B build
cmake --build build
cd build && ctest --output-on-failure
```

The first build pulls Catch2 v3.5.0 via FetchContent — adds ~30 seconds. Subsequent builds are fast.

### What to test

Good DSP unit tests:
- **Identity**: silence in → silence out (margin for denormals)
- **Stability**: unit impulse in → bounded output (no NaN, no explosion)
- **Frequency response**: known sine at known frequency → expected gain
- **Parameter sweep**: smoothly varying parameter → no clicks in output (sample-by-sample diff stays bounded)
- **State**: same input twice → same output twice (after `prepare`)

What pluginval already covers (so tests don't need to): parameter ranges, state save/restore round-trip, audio buffer handling, threading behavior.
