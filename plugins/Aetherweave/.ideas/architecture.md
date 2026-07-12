# Aetherweave Architecture

## Signal Flow

```
                                         (parallel)
                                              │
Input ──► [Capture] ──► [Shatter] ──► [Drift] ┼─► [Bloom]
            buffer       grain         swarm  │    shimmer + diffusion
            + freeze     engine        physics│
                                              ▼
                                         (post-Drift) ─► sum ─► [Dry/Wet] ─► [Out gain] ─► Output
                                                              ▲
                                                              │
                                              Input ──────────┘ (dry path)
```

Each stage is bypassable via its `*_enable` parameter, in which case it passes audio unchanged. Bloom is a parallel send/return on the post-Drift signal, not a serial insert — that lets users dial in a tail without changing the body of the sound.

## DSP Components

### 1. CaptureBuffer (`Source/dsp/CaptureBuffer.{h,cpp}`)

A circular buffer wrapping `juce::AudioBuffer<float>`. Sized to `8 sec × sampleRate × channels` at `prepare()` time (the maximum capture length). The write head advances each block; on freeze, it stops, but reads continue. Provides `float read(double position, int channel)` with Lagrange-3rd interpolation so grains can read at fractional positions and at non-unity rates.

**Why a custom class instead of `juce::dsp::DelayLine`?** DelayLine is single-tap with one read head. We need many concurrent grain reads at arbitrary positions and rates — easier to write directly.

### 2. GrainEngine (`Source/dsp/GrainEngine.{h,cpp}`)

A pool of `Grain` voices (size = `voice_limit`, default 64, ceiling 256) and a Poisson scheduler that births new grains at rate `grain_density`. Each `Grain` holds:

```cpp
struct Grain {
    bool   active;
    double readPos;      // current read position in CaptureBuffer (samples)
    double readSpeed;    // 1.0 + pitchOffsetSemis converted to ratio
    int    lifeRemaining;// samples until grain dies
    int    grainLengthSamples;
    float  amplitude;
    float  panL, panR;
    // (time, pitch) coordinates for the Drift swarm
    float  swarmTime;    // normalized 0–1 across buffer
    float  swarmPitch;   // semitones from source, -24..+24
};
```

Each `processBlock`, the engine:
1. Schedules new grains (Poisson process with rate `density`).
2. For each new grain, sets `swarmTime`/`swarmPitch` from a center + `pos_jitter`/`pitch_jitter` Gaussian, then asks `DriftField` for a corrected position (gravity wells pull it).
3. Maps `(swarmTime, swarmPitch)` to `(readPos, readSpeed)`.
4. Sums grain output into the working buffer with Hann/Tukey windowing (tilt-controlled).

Grain *birth* parameters (size, density, jitter) live here; grain *motion* lives in DriftField.

**Why a custom voice pool instead of `juce::Synthesiser`?** Synthesiser is MIDI-note-driven. Grains are not notes — they're continuously birthed by a Poisson process and have no key/pitch semantics in the MIDI sense. Hand-rolled pool is the right fit.

### 3. DriftField (`Source/dsp/DriftField.{h,cpp}`)

The gravity-well physics. Holds:

```cpp
struct Well {
    float x, y;          // (time 0–1, pitch normalized -1..+1)
    float radius;        // 0..1
    float strength;      // 0..1 magnitude
    int   polarity;      // +1 attractor, -1 repulsor
};
```

Up to 16 wells. Stored in plugin state via a `juce::ValueTree` child, NOT in APVTS (see "State" below).

Each block, for each *active* grain, `DriftField`:
1. Computes net force from all wells: `F = Σ polarity_i · strength_i · (well_i - grain) / (|well_i - grain|² + ε)`, clamped at well radius.
2. Adds Brownian noise scaled by `swarm_temp` (0% = none, 100% = forces fully overridden by random walk).
3. Integrates velocity → position using simple Euler with `motion_speed` as a time-scaling factor.
4. Hands back the new `(swarmTime, swarmPitch)` to the grain.

Coupling between engine and field is one-way: GrainEngine asks DriftField for updated positions; DriftField has no audio buffer access.

**Why custom physics instead of `juce::dsp::LFO` per grain?** LFOs produce periodic motion. We want continuous-in-state, gesture-driven swarm dynamics — that's a particle system, not an oscillator.

### 4. BloomTail (`Source/dsp/BloomTail.{h,cpp}`)

A parallel shimmer + diffusion. Internally:

1. **Pitch shifter** — granular pitch shifter built on `juce::dsp::DelayLine<float, Lagrange3rd>` with two crossfaded read heads (classic ping-pong granular technique). Cheap, mono-friendly, no FFT latency. Interval = `shimmer_interval`.
2. **Reverb** — `juce::dsp::Reverb` with `roomSize` and `damping` driven by `bloom_diffusion`.
3. **Feedback** — shimmer output feeds into reverb input; reverb output feeds back into shimmer at ~30% (creates the rising shimmer halo).
4. **Wet level** — final blend against the post-Drift signal at `bloom_mix`.

**Why granular shifter over FFT phase-vocoder?** Phase-vocoder gives cleaner shifts on harmonic material but adds ~50 ms latency and CPU. For shimmer (a wet effect heard far behind the dry), granular is more than good enough and adds no latency.

### 5. WellManager (UI side, `Source/ui/WellManager.{h,cpp}`)

Lives only in the editor. Owns the `std::vector<Well>` and mediates between mouse gestures and the processor's serialized well state. Pushes well changes to the processor via a thread-safe handoff (a `juce::AbstractFifo` or `std::atomic`-protected double-buffered well array — the processor's audio thread reads a stable snapshot each block).

## Parameters → DSP Mapping

| Parameter | Drives |
|-----------|--------|
| `capture_enable` | Bypasses CaptureBuffer write (read still happens; just stale) |
| `capture_length` | CaptureBuffer's active window size |
| `capture_freeze` | Halts CaptureBuffer write head |
| `shatter_enable` | Bypasses GrainEngine (passes input through to Drift, which then just passes through too) |
| `grain_size` | GrainEngine `grainLengthSamples` for new grains |
| `grain_density` | GrainEngine Poisson rate λ |
| `pos_jitter` | GrainEngine stddev for `swarmTime` at birth |
| `pitch_jitter` | GrainEngine stddev for `swarmPitch` at birth |
| `window_tilt` | GrainEngine Hann↔Tukey window blend |
| `drift_enable` | DriftField bypass (grain positions stay at birth coords) |
| `swarm_temp` | DriftField Brownian noise scale |
| `well_strength` | DriftField master force gain |
| `motion_speed` | DriftField integration time-scale |
| `bloom_enable` | BloomTail bypass (zero wet) |
| `shimmer_interval` | BloomTail pitch shift in semitones |
| `bloom_diffusion` | BloomTail reverb roomSize + damping |
| `bloom_mix` | BloomTail wet level |
| `mix` | Top-level dry/wet |
| `output_gain` | Final gain stage |
| `voice_limit` | GrainEngine pool ceiling |
| `stereo_spread` | GrainEngine pan distribution |
| `brightness_bias` | GrainEngine bias for spectral-centroid-weighted grain birth (Stage 2 deferral — implement as no-op for now and revisit if there's time) |
| `tempo_sync` | Quantizes `capture_length` and `grain_size` to host tempo divisions |
| `vis_trail` | UI-only, unused on audio thread |

## State

APVTS handles all standard parameters via XML in `getStateInformation`/`setStateInformation`. **Wells** are appended as a child `ValueTree` named `"wells"` containing one child per well with attributes `x`, `y`, `radius`, `strength`, `polarity`. Standard XML round-trip.

This is the conventional JUCE pattern for "structured plugin state that isn't a single parameter" and is honored correctly by DAW preset save/load.

## Threading

- **Audio thread:** reads APVTS via cached `std::atomic<float>*` pointers; reads a frozen snapshot of the well array from a lock-free double-buffer.
- **UI thread (60 fps timer):** writes to wells (via the lock-free buffer), reads grain position snapshots from the processor for visualization (also via a double-buffered snapshot).
- **Message thread:** state save/load.
- No locks in `processBlock`. No allocations in `processBlock`.

## Sample-rate handling

All DSP modules accept a `prepare(double sampleRate, int blockSize, int numChannels)` call. The CaptureBuffer is sized to `8.0 * sampleRate * channels` once at prepare. The reverb and delay lines re-prepare on every sample rate change. Tempo sync queries `juce::AudioPlayHead` each block.

## Latency

Zero reported latency. No FFT, no lookahead. The Bloom shimmer is wet-only so its small grain-pitch-shift latency (~10 ms) does not delay the dry path.

## CPU budget (estimated)

| Component | Budget @ 64 voices, 48 kHz, stereo |
|-----------|------------------------------------|
| CaptureBuffer | <1% (memcpy + write pointer) |
| GrainEngine (64 grains) | ~3–5% (Lagrange read + windowing) |
| DriftField (64 grains × 16 wells) | <1% (vectorizable) |
| BloomTail | ~2% (reverb dominates) |
| **Total** | **<10% on M1, <15% on older Intel** |

256 voices roughly triples the GrainEngine cost — still well under 20% on modern CPUs. The `voice_limit` parameter gives users a clear performance dial.

## Implementation Stages

### Stage 1: Foundation
- CMakeLists.txt builds.
- Plugin loads in standalone and a DAW.
- APVTS exposes all parameters from `parameter-spec.md`.
- `processBlock` is pass-through (no audio change).
- `GenericAudioProcessorEditor` shows all parameters in a default layout.
- **Verification:** Plugin appears in DAW, all knobs visible, audio plays through unchanged.

### Stage 2: DSP
Build modules in order, gating on tests at each step.

1. **CaptureBuffer** — write input, read at unity. Unit test: silence-in → silence-out; impulse-in → impulse-out at correct read offset.
2. **GrainEngine** — schedule and render grains at fixed (time, pitch) = (now, 0). Unit test: stable RMS output given a stable sine input; no NaN on impulse.
3. **DriftField** — wells modify grain positions. Unit test: with one attractor at center, grain positions converge over time; with `swarm_temp = 0`, motion is deterministic.
4. **BloomTail** — shimmer + reverb. Unit test: impulse-in → bounded decay; silence-in → silence-out (margin).
5. Wire all four into `processBlock` with bypass switches and dry/wet mix.
- **Verification:** All four stages audibly do what they should. pluginval strictness 5 passes on the VST3.

### Stage 3: UI
1. Replace `GenericAudioProcessorEditor` with custom `AetherweaveEditor`.
2. Build the stage strip (4 cards), source ribbon, secondary controls row, output strip.
3. Build the Grain Constellation OpenGL view: grain particles, trails, wells, axes.
4. Wire gesture handlers (left-drag = attractor, right-drag = repulsor, etc.).
5. Connect the well manager to the processor (lock-free handoff).
6. Connect the visualization snapshot reader (processor publishes a grain snapshot at audio rate; UI samples it at 60 fps).
- **Verification:** Painting wells changes the swarm in real time. Visual matches audio. No glitches on resize. Resize preserves aspect.

## Notes

- The brightness-bias parameter (Stage 2) is implemented as a no-op until grain birth has a working spectral centroid bucketer for the buffer; revisit only if there's time after Stage 3.
- Tempo sync (Stage 2) requires `AudioPlayHead::getCurrentPosition()` — supported in JUCE 8 across formats; AU/VST3 both report.
- The OpenGL grain renderer should fall back gracefully if the host disables GL (some hosts do for security). The fallback is to render via `Component::paint` with reduced trail length — slower but never crashes.
- Wells beyond 16 silently drop the oldest; this is documented in the UI.
- No MIDI input or output. `IS_SYNTH = FALSE`, `NEEDS_MIDI_INPUT = FALSE`.
- **CLAP** is dropped from `FORMATS` because the bundled JUCE 8.0.12 submodule does not include native CLAP CMake support. Re-add via `clap-juce-extensions` (https://github.com/free-audio/clap-juce-extensions) if CLAP becomes a requirement before release.
