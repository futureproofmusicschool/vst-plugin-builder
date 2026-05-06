# Reverb-a-tron Parameters

## Core Parameters
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Dry/Wet | mix | 0–100 | 30 | % | Blend between dry and wet signal |
| Decay | decay_time | 0.1–10.0 | 2.0 | sec | Base reverb tail length |
| Size | room_size | 0–100 | 50 | % | Virtual room dimensions |
| Pre-Delay | pre_delay | 0–250 | 20 | ms | Gap before reverb onset |

## Harmonic Resonance
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Resonance Amount | res_amount | 0–100 | 50 | % | How strongly the reverb tunes to detected pitch |
| Harmonics | res_harmonics | 1–8 | 4 | count | How many harmonics above the fundamental resonate |
| Resonance Q | res_q | 0.1–10.0 | 2.0 | Q | Width of each resonant band (tight vs. broad) |
| Tracking Speed | res_tracking | 0–100 | 70 | % | How quickly resonance follows pitch changes |
| Confidence Threshold | res_confidence | 0–100 | 50 | % | Minimum detection confidence before resonance disengages |

Pitch detection uses FFT-based spectral analysis to identify the dominant pitch in the input. It works best on tonal material with a clear fundamental. When detection confidence falls below the threshold, resonance smoothly blends back to neutral (un-tuned) reverb.

## Frequency Sculpt
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Low Cut | sculpt_low_cut | 20–2000 | 80 | Hz | High-pass filter on reverb tail |
| High Cut | sculpt_high_cut | 1000–20000 | 12000 | Hz | Low-pass filter on reverb tail |
| Mid Freq | sculpt_mid_freq | 200–8000 | 1000 | Hz | Center frequency for mid band |
| Mid Gain | sculpt_mid_gain | -18–+18 | 0 | dB | Boost/cut the mid band in the tail |
| Mid Q | sculpt_mid_q | 0.1–10.0 | 1.0 | Q | Width of mid band |

## Flutter
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Flutter Rate | flutter_rate | 0.1–10.0 | 2.5 | Hz | Speed of pitch/time modulation |
| Flutter Depth | flutter_depth | 0–100 | 30 | % | Intensity of the warble |
| Flutter Shape | flutter_shape | 0–3 | 0 | — | Waveform: Sine (0), Triangle (1), Random (2), Drunk Walk (3) |

## Beat Sync
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Sync Enable | sync_enable | Off/On | Off | — | Lock decay to host tempo |
| Sync Division | sync_division | 1/16–4 bars | 1/4 | note | Musical note value for decay time |
| Sync Flutter | sync_flutter_rate | Off/On | Off | — | Lock flutter rate to tempo subdivision |

## Output
| Parameter | ID | Range | Default | Unit | Description |
|---|---|---|---|---|---|
| Width | stereo_width | 0–200 | 100 | % | Stereo spread of reverb (0=mono, 200=widened) |
| Output Level | output_level | -24–+6 | 0 | dB | Final output gain |

**Total: 23 parameters across 6 sections.**
