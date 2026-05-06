# Reverb-a-tron Architecture

## Signal Flow

```
Input → [FFT Pitch Detector] → detected frequency (analysis only, no audio modification)
  |
  v
Input → Pre-Delay → Reverb Delay Network → Harmonic Resonance Filters
  |                                              |
  |                                              v
  |                                   Frequency Sculpt (HPF + Mid EQ + LPF)
  |                                              |
  |                                              v
  |                                      Flutter Modulation (LFO-driven chorus)
  |                                              |
  |                                              v
  |                                      Stereo Width (M/S processing)
  |                                              |
  |                                              v
  |                                        Output Level
  |                                              |
  v                                              v
Dry signal ──── Mix blend ──── Wet signal → Output
```

## DSP Components

### 1. FFT Pitch Detector
- 4096-point FFT using `juce::dsp::FFT`
- Hann window applied before analysis
- Sums stereo input to mono for analysis
- Stores samples in a ring buffer and analyzes every 1024 samples
- Finds the dominant pitch via spectral peak detection
- Confidence: ratio of peak magnitude to spectral average
- Smoothed tracking controlled by `res_tracking` parameter

### 2. Pre-Delay
- Simple circular delay buffer
- Range: 0–250ms
- Uses smoothed delay-time changes to avoid clicks during automation

### 3. Reverb Core
- Four parallel feedback delay lines per channel
- Delay lengths scale with `room_size`
- Feedback gain is derived from `decay_time`, so the tail reaches roughly -60 dB at the selected time
- A light damping stage in each feedback loop keeps the tail from ringing harshly
- External wet/dry mix handles the final blend

### 4. Harmonic Resonance Filter Bank
- Up to 8 `juce::dsp::StateVariableTPTFilter` in bandpass mode
- Each tuned to: fundamental × harmonic_number (1, 2, 3, ... 8)
- Q controlled by `res_q` parameter
- Blend between unfiltered and filtered based on `res_amount` and confidence
- When confidence < threshold, smoothly crossfades to unfiltered

### 5. Frequency Sculpt
- High-pass: `StateVariableTPTFilter` (highpass mode)
- Low-pass: `StateVariableTPTFilter` (lowpass mode)
- Parametric mid: `juce::dsp::IIR::Filter` with peak coefficients

### 6. Flutter
- Modulated delay line (chorus-like)
- LFO shapes: Sine, Triangle, Random, Drunk Walk
- Rate and depth controllable
- Can sync to host tempo via beat sync

### 7. Beat Sync
- Reads `juce::AudioPlayHead` for BPM
- Converts the selected musical division into the effective decay time
- Optionally syncs flutter rate to a subdivision of that same value

### 8. Stereo Width
- Mid/Side: M = (L+R)/2, S = (L-R)/2
- Width scales S channel: 0% = mono, 100% = unchanged, 200% = widened
- Output: L = M + S*width, R = M - S*width

### 9. Output Level
- Final gain stage with smoothed level changes
