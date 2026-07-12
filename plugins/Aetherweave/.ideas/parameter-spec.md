# Aetherweave Parameters

Parameters are grouped by stage. Each stage can be bypassed via its own `*_enable` parameter so users can A/B individual transformations.

## Core Parameters

### Stage 1 — Capture
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Capture Enable | `capture_enable` | off / on | on | bool | Bypass disables buffering and passes input through. |
| Buffer Length | `capture_length` | 0.25 – 8.0 | 2.0 | sec | How much input audio is held for granulation. Longer = more material to draw grains from. |
| Freeze | `capture_freeze` | off / on | off | bool | Locks the buffer. Source can stop and the plugin keeps weaving. The gesture every granular user reaches for first. |

### Stage 2 — Shatter
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Shatter Enable | `shatter_enable` | off / on | on | bool | |
| Grain Size | `grain_size` | 5 – 500 | 80 | ms | Short grains = pointillist / textural. Long grains = smooth pad. |
| Density | `grain_density` | 1 – 200 | 40 | grains/s | Sparse to thick. Above ~120 it starts sounding like continuous tone. |
| Position Jitter | `pos_jitter` | 0 – 100 | 25 | % | Randomizes which slice of the buffer each grain is drawn from. 0 = locked, 100 = chaotic scrub. |
| Pitch Jitter | `pitch_jitter` | 0 – 24 | 0 | semitones | Random pitch deviation per grain. Small values = chorus-like, large = chord cloud. |
| Window Tilt | `window_tilt` | 0 – 100 | 50 | % | 0 = soft Hann (smooth), 100 = harder Tukey edges (more transient). |

### Stage 3 — Drift (Gravity Constellation)
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Drift Enable | `drift_enable` | off / on | on | bool | |
| Swarm Temperature | `swarm_temp` | 0 – 100 | 30 | % | Low temp = grains stick to wells. High temp = grains break free and scatter. The single most expressive macro. |
| Well Strength | `well_strength` | 0 – 100 | 60 | % | Master gain for attractor/repulsor forces. Wells stored as data, not parameters — see `ui-mockup-v1.md`. |
| Motion Speed | `motion_speed` | 0.1 – 10 | 1.0 | × | How fast the swarm responds to forces. Slow = drifting fog, fast = darting flocks. |

### Stage 4 — Bloom
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Bloom Enable | `bloom_enable` | off / on | on | bool | |
| Shimmer Interval | `shimmer_interval` | -24 – +24 | +12 | semitones | Pitch of the shimmer feedback path. +12 is the classic Eno shimmer. |
| Diffusion | `bloom_diffusion` | 0 – 100 | 70 | % | Reverb tail length and density. |
| Bloom Mix | `bloom_mix` | 0 – 100 | 35 | % | Wet level of the shimmer/diffusion path against the post-Drift signal. |

### Output
| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Dry/Wet | `mix` | 0 – 100 | 100 | % | Blend of fully processed signal vs original input. Default 100 because the effect is dramatic and most users want to hear it solo first. |
| Output Gain | `output_gain` | -24 – +12 | 0 | dB | Trim after processing. Granular clouds can be loud. |

## Secondary Parameters

| Parameter | ID | Range | Default | Unit | Description |
|-----------|-----|-------|---------|------|-------------|
| Grain Voices | `voice_limit` | 8 – 256 | 64 | count | Maximum concurrent grains. Performance ceiling. |
| Stereo Spread | `stereo_spread` | 0 – 100 | 70 | % | Pan distribution of grains across the field. |
| Brightness Bias | `brightness_bias` | -100 – +100 | 0 | % | Tilts grain selection toward brighter or darker spectral regions of the buffer (cheap STFT centroid bucketing). |
| Tempo Sync | `tempo_sync` | off / on | off | bool | If on, Buffer Length and Grain Size snap to host tempo divisions. |
| Visual Trail Length | `vis_trail` | 0 – 100 | 60 | % | UI-only. How long grain trails persist. Doesn't affect audio. |

## Notes on `wells` (NOT a normal parameter)
The gravity wells the user paints onto the Constellation are stored in plugin state as an array of `{x, y, polarity, radius, strength}` records. They're saved/loaded with presets but aren't host-automatable individually — automating them as a group would break DAW automation lanes. Instead the four global parameters above (`swarm_temp`, `well_strength`, `motion_speed`, `drift_enable`) are the automatable handles on the well system. This is called out here so `/plan` builds the right state model.
