# Reverb-a-tron UI Layout

## Window Size
750 x 500

## Visual Style
Retro / analog — warm tones, skeuomorphic knobs, vintage hardware panel feel. Warm color palette with browns, ambers, and cream tones.

## Layout

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                          R E V E R B - A - T R O N                          │
│                              Example Co                                     │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─── MAIN ──────────┐  ┌─── HARMONIC RESONANCE ────────────────────────┐  │
│  │                    │  │                                               │  │
│  │ [Mix] [Decay] [Size] [PreDly] │  │ [Amount] [Harmonics] [Q] [Track] [Confid] │  │
│  │                                │  │                                             │  │
│  │                    │  │     ┌──────────────────┐                      │  │
│  │                    │  │     │  A4   440 Hz     │  ◀ pitch display     │  │
│  │                    │  │     │  ─┼──|──────┼─   │  ◀ tuning meter     │  │
│  │                    │  │     └──────────────────┘                      │  │
│  └────────────────────┘  └──────────────────────────────────────────────┘  │
│                                                                              │
│  ┌─── FREQ SCULPT ──────────────────┐  ┌─── FLUTTER ────────────────────┐  │
│  │                                   │  │                                │  │
│  │  [LoCut]  [Mid Freq]   [HiCut]   │  │  [Rate]   [Depth]   [Shape]   │  │
│  │           [Mid Gain]   [Mid Q]    │  │                                │  │
│  │                                   │  └────────────────────────────────┘  │
│  └───────────────────────────────────┘                                       │
│                                                                              │
│  ┌─── BEAT SYNC ─────────────────────┐  ┌─── OUTPUT ────────────────────┐  │
│  │                                    │  │                               │  │
│  │  [Enable]  [Division]  [SyncFlut] │  │  [Width]      [Output Level]  │  │
│  │                                    │  │                               │  │
│  └────────────────────────────────────┘  └───────────────────────────────┘  │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

## Controls
- **Rotary knobs** for all continuous parameters
- **Toggle switches** for Sync Enable and Sync Flutter (retro slide-toggle style)
- **Drop-down selector** for Flutter Shape (4 options: Sine, Triangle, Random, Drunk)

## Pitch Display
Located in the Harmonic Resonance section as a visual centerpiece:
- Note name + octave (e.g., "A4")
- Frequency in Hz (e.g., "440 Hz")
- Tuning meter showing cents deviation from nearest semitone
- Dims or shows "---" when confidence is below threshold
- Retro LCD / nixie tube aesthetic to match the analog theme

## Visual Notes
- Warm color palette: browns, ambers, cream, with subtle orange/amber accent lighting on active controls
- Panel texture resembling brushed metal or aged wood
- Embossed or engraved section labels
- Vintage knob style with pointer indicators
- Subtle drop shadows on raised elements
