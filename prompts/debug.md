# Debug — Plugin Diagnosis

Walk a user from symptom to fix when something's wrong with a plugin. Six common categories: it won't build, won't load, makes the wrong sound, loses state, crashes, or doesn't respond to automation.

## Teach as you go

Before each diagnostic step, say in 1–2 sentences what you're checking and why. Debugging is the place users learn the most about how plugins actually work — they're staring at concrete failure modes. Don't waste the moment.

Specific moments to teach:

- **When checking the audio thread:** "Audio runs on a separate, real-time thread. Anything that can block — `new`, mutex locks, file I/O — causes audible clicks or dropouts. That's the first thing we check on click/pop reports."
- **When checking AU code signing:** "AU plugins are sandboxed by macOS and have to be code-signed. CLT alone can't sign them; full Xcode is required. This is the #1 cause of 'AU silently fails to load in Logic'."
- **When checking parameter IDs:** "APVTS keys state by parameter ID. If an ID was renamed between sessions, the old saved state can't find a home — values look 'lost' but the data is still in the project file."

## Trigger

Common phrasings: "/debug", "/debug <PluginName>", "plugin won't load", "I'm getting clicks", "state isn't saving", "crashes when I open the editor", "Logic doesn't see it".

## Process

### Step 1: Identify the symptom category

Ask (or infer):

1. **What do you see?** (compile error, "failed to load" in DAW, silence, distortion, clicks/pops, wrong sound, parameter values reset, crash)
2. **When does it happen?** (always, only on first load, only under CPU load, only on close, only after change X)
3. **Which DAW and format?** (Logic+AU, Ableton+VST3, Bitwig+CLAP, Reaper+VST3, Standalone)
4. **What changed recently?** (new code, JUCE submodule update, macOS update, new parameter)

Classify into one of these categories — pick the dominant one if it's mixed:

| Category | Example symptoms |
|----------|------------------|
| **BUILD** | CMake error, compiler error, linker error, missing header |
| **LOAD** | Build succeeds, DAW says "failed to load" or silently doesn't show plugin |
| **AUDIO** | Plugin loads, but: silence, distortion, clicks/pops, wrong frequency response, latency |
| **STATE** | Parameter values don't persist after closing/reopening project |
| **CRASH** | Plugin loads then crashes — on insert, on edit, on close, under load |
| **AUTOMATION** | Parameters don't move when automation plays back, or move but don't affect sound |

### Step 2: Consult `rules/known-issues.yaml`

Read `rules/known-issues.yaml`. For each entry, check whether any of its `detect` substrings appear in the user's error output, build log, or symptom description.

If a match is found:
- Tell the user which entry matched and why.
- Apply the listed `fix`.
- Increment the entry's `frequency` counter and update `last_seen` to today's date.
- Verify the fix worked (rebuild, reload, retest).

If no entry matches, proceed to Step 3.

### Step 3: Walk structured diagnostics by category

Pick the category from Step 1. Walk these checks in order. Stop when the symptom is gone.

#### BUILD

1. Read the actual error output, not just "it failed". Ask for the last 30–50 lines.
2. Verify JUCE submodule: `ls JUCE/CMakeLists.txt` should exist. If not, `git submodule update --init --recursive`.
3. Verify CMake version: `cmake --version` should be ≥ 3.22.
4. Look up unfamiliar JUCE class names via the JUCE Docs MCP server before assuming the API is correct.
5. Try a clean rebuild: `rm -rf build && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release`.

#### LOAD

1. Confirm the artifact exists at the install path:
   - VST3: `ls ~/Library/Audio/Plug-Ins/VST3/[DisplayName].vst3`
   - AU: `ls ~/Library/Audio/Plug-Ins/Components/[DisplayName].component`
   - CLAP: `ls ~/Library/Audio/Plug-Ins/CLAP/[DisplayName].clap`
2. For AU: verify `xcode-select -p` points to `/Applications/Xcode.app/Contents/Developer`. CLT alone cannot sign AU.
3. Trigger a DAW rescan (Logic: Plug-In Manager > Reset & Rescan; Ableton: toggle "Use VST3 Plug-In System Folders").
4. Run `pluginval --strictness-level 5 --validate <path>` — many "won't load" issues surface here.
5. For AU specifically: `auval -v aufx <PLUGIN_CODE> <MANUFACTURER_CODE>` (replace with the 4-char codes from CMakeLists.txt).

#### AUDIO

1. **Silence:** confirm `processBlock` is being called (add a temporary `juce::Logger::writeToLog` outside the audio thread or set a counter member). Confirm the buffer isn't being cleared after processing.
2. **Clicks/pops:** look for allocations or locks in `processBlock`. Common offenders: `std::vector::push_back`, `juce::String` construction, `DBG()` calls, `std::lock_guard`. Move them all to `prepareToPlay`.
3. **Distortion at unity:** confirm wet/dry mix isn't double-applying gain. Check that the input buffer isn't being summed into itself.
4. **Wrong frequency response:** verify filter coefficients are recomputed when sample rate changes (in `prepareToPlay`, not just the constructor).
5. **Latency:** if DSP introduces delay, the processor must call `setLatencySamples()` so the DAW can compensate.

#### STATE

1. Compare the parameter IDs in `createParameterLayout()` against what's stored in the saved project. Renames break restore.
2. Verify `getStateInformation` and `setStateInformation` are using APVTS's serialization — ad-hoc serialization usually has the bug.
3. For non-parameter state: that's not handled by APVTS automatically. Add it to a child ValueTree node and serialize manually.
4. Check the saved project file (XML for Reaper; binary for Ableton — extract via `als2xml`) to see what's actually stored.

#### CRASH

1. Build a Debug version: `cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug && cmake --build build-debug`.
2. Run the Standalone in lldb: `lldb plugins/[PluginName]/build-debug/[Name]_artefacts/Standalone/[Name].app/Contents/MacOS/[Name]` then `run`. Crash gives a stack trace.
3. Common offenders: dangling pointer in editor (processor outlives editor — make sure listeners are removed in editor destructor), null dereference on first call, unguarded UI update from audio thread.
4. Run pluginval at strictness 10 — it stresses concurrent edit/process patterns.

#### AUTOMATION

1. Verify APVTS is wired correctly — every parameter has an entry in `createParameterLayout()` and the editor uses `SliderAttachment` (not manual listeners).
2. Confirm the parameter ID strings are identical between processor (where the parameter is created) and editor (where the attachment is made).
3. Check whether the DAW shows the parameter at all — if it doesn't, the issue is in `createParameterLayout()`. If it does but doesn't react, the issue is in the DSP code reading the parameter.
4. Trigger a DAW rescan — DAWs cache parameter lists.

### Step 4: Record new findings

If the resolution wasn't already in `rules/known-issues.yaml`, append a new entry. Use this shape (see existing entries for examples):

```yaml
- id: <short-kebab-case-id>
  symptom: <one line>
  detect:
    - <substring 1>
    - <substring 2>
  fix: |
    <multi-line fix>
  frequency: 1
  last_seen: <YYYY-MM-DD>
```

Pick a `detect` pattern that's specific enough to not false-match but general enough to catch the same error in different builds. The next person to hit this saves time.

### Step 5: Verify and report

After applying a fix:

1. Clean rebuild: `rm -rf build && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release`.
2. Re-test the original failure mode. Don't trust "it should be fixed now" — confirm.
3. Update `plugins/[PluginName]/status.json`: update `last_build` with the new result. (Schema in `rules/status-schema.md`.)
4. Tell the user what was wrong, what fixed it, and whether you added a new entry to `known-issues.yaml`.

## Notes

- If three diagnostic attempts in a category don't move the needle, stop and reassess. Ask the user whether the symptom changed, or whether you've been chasing the wrong category.
- Use the JUCE Docs MCP server when an API name is in doubt. Fabricated APIs cause silent runtime errors that look like new bugs.
- Reference `plugins/underwater/Source/` and `plugins/ReverbATron/Source/` for known-good patterns when comparing against broken code.
