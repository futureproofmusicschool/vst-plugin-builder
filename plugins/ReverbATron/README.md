# Reverb-a-tron

A harmonically intelligent reverb plugin by Futureproof. It listens to what you play and tunes its resonances to match, turning the reverb space into a musical instrument rather than a passive room simulation.

Built with the JUCE framework. Available as VST3, AU, and Standalone.

## What It Does

Reverb-a-tron analyzes the dominant pitch of incoming audio using FFT and uses that information to shape the reverb tail in real time. The reverb rings in tune with your input, pulses with the beat, and warbles with organic modulation. Six control sections let you dial in exactly the sound you want.

## Sections

### Main

Basic reverb controls. **Mix** blends between dry and wet signal. **Decay** sets the tail length (0.1 to 10 seconds). **Size** controls the virtual room dimensions. **Pre-Delay** adds a gap before the reverb onset (up to 250ms), which helps separate the dry sound from the tail.

### Harmonic Resonance

This is the core feature. A bank of up to 8 bandpass filters tunes itself to the harmonics of whatever pitch is detected in the input.

- **Amount** controls how strongly the reverb tail is colored by the resonant filters (0% = no effect, 100% = fully resonant).
- **Harmonics** sets how many overtones above the fundamental are reinforced (1 = fundamental only, 8 = rich harmonic series).
- **Q** controls the width of each resonant band. Higher Q means tighter, more ringing resonances. Lower Q gives a broader, subtler coloring.
- **Track** controls how quickly the resonance follows pitch changes. Low values give a slow, laggy response (good for pads). High values snap to new pitches quickly (good for melodic lines).
- **Confid** (Confidence Threshold) sets the minimum pitch detection confidence required before resonance engages. When the detector isn't sure what pitch it's hearing (noise, complex textures, silence), resonance fades out smoothly. Lower the threshold to keep resonance active on less tonal material; raise it to only engage on clear pitches.

The pitch display below the knobs shows the detected note, frequency in Hz, and a tuning meter.

### Freq Sculpt

EQ applied to the reverb tail only (the dry signal is not affected). Use this to shape how the reverb sits in a mix without changing the source sound.

- **Lo Cut** removes low-frequency mud from the tail (high-pass filter, 20 Hz to 2 kHz).
- **Mid Freq / Mid Gain / Mid Q** form a parametric mid-band EQ. Boost or cut a specific frequency range in the tail by up to 18 dB.
- **Hi Cut** tames harsh high frequencies in the tail (low-pass filter, 1 kHz to 20 kHz).

### Flutter

Adds organic pitch and time modulation to the reverb tail, similar to the imperfections in tape machines or old spring reverbs. Implemented as an LFO-modulated delay line.

- **Rate** controls modulation speed (0.1 to 10 Hz).
- **Depth** controls the intensity of the warble (0% = off, 100% = heavy modulation).
- **Shape** selects the LFO waveform:
  - **Sine** -- smooth, predictable modulation
  - **Triangle** -- slightly more linear sweep
  - **Random** -- sample-and-hold randomness, glitchy and unpredictable
  - **Drunk** -- random walk that wanders gradually, organic and drifting

### Beat Sync

Locks the reverb to your DAW's tempo.

- **Sync** toggle enables tempo sync. When on, the Decay knob is overridden and the tail length matches the selected note division.
- **Division** selects the musical length: 1/16, 1/8, 1/4, 1/2, 1 Bar, 2 Bars, or 4 Bars.
- **Sync Flut** toggle also locks the flutter rate to a subdivision of the selected beat division.

Beat sync reads BPM from the host DAW. It has no effect in Standalone mode or if the host isn't providing tempo information.

### Output

- **Width** adjusts the stereo spread of the wet signal using mid/side processing. 0% collapses to mono, 100% is unchanged, 200% exaggerates the stereo image.
- **Output** is a final gain stage (-24 dB to +6 dB) applied to the wet signal before the dry/wet mix.

## Signal Flow

```
Input ---> FFT Pitch Detector (analysis only, no audio change)
  |
  v
Input ---> Pre-Delay ---> Reverb ---> Harmonic Resonance Filters
  |                                          |
  |                                    Frequency Sculpt
  |                                          |
  |                                       Flutter
  |                                          |
  |                                     Stereo Width
  |                                          |
  |                                     Output Level
  |                                          |
  v                                          v
Dry ------------------- Mix ------------------- Wet ---> Output
```

The dry signal is saved before any processing and blended back in at the end. All processing sections (resonance, sculpt, flutter, width, output level) affect only the wet path.

## Building

```bash
cd plugins/ReverbATron
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

With `COPY_PLUGIN_AFTER_BUILD` enabled, the built plugin is automatically installed to your system plugin directories. Rescan plugins in your DAW after building.

## Requirements

- CMake 3.22+
- Xcode Command Line Tools (Xcode full app required for AU signing)
- JUCE framework (included as a git submodule at the repo root)
