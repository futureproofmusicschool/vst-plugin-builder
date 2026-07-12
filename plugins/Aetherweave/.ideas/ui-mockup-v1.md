# Aetherweave UI Layout — v1

## Window Size
1000 x 680 (resizable, min 800 x 560, max 1400 x 950, preserves aspect)

The window is wider than tall because the Grain Constellation reads more naturally as a horizontal phase-space (time flows left → right, pitch deviates up/down from a horizontal centerline).

## Layout

```
┌────────────────────────────────────────────────────────────────────────────────┐
│  AETHERWEAVE                                  preset ▾   A/B   ?   ⚙        │
├────────────────────────────────────────────────────────────────────────────────┤
│ ┌─CAPTURE─────┐ ┌─SHATTER─────┐ ┌─DRIFT───────┐ ┌─BLOOM───────┐   stage tabs    │
│ │ ◉ enable    │ │ ◉ enable    │ │ ◉ enable    │ │ ◉ enable    │                 │
│ │ Length 2.0s │ │ Size  80ms  │ │ Temp   30%  │ │ Shim +12st  │                 │
│ │ ❄ freeze    │ │ Dens  40/s  │ │ Strgth 60%  │ │ Diff   70%  │                 │
│ │             │ │ Jit   25%   │ │ Speed  1.0× │ │ Mix    35%  │                 │
│ └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘                 │
├────────────────────────────────────────────────────────────────────────────────┤
│ ╔══════════════════ source ribbon (waveform of capture buffer) ══════════════╗ │
│ ║  ▁▂▃▅▇▆▅▃▂▁▁▂▄▆▇▇▆▄▂▁  [▶ playhead]   ❄ freeze ─────────────────────────  ║ │
│ ╚════════════════════════════════════════════════════════════════════════════╝ │
│                                                                                │
│ ╔══════════════════════ GRAIN CONSTELLATION ════════════════════════════════╗ │
│ ║ +24st┊                                                                     ║ │
│ ║      ┊        · ·   ✦                  ⊕(attractor)     ·                  ║ │
│ ║      ┊      ·  ✦ ✦ ·         · ·   ✦ ✦ ✦                                   ║ │
│ ║   0  ┊─────·──✦ ✦ ✦ ✦──·─·──✦──·──✦ ✦ ✦ ✦ ✦──·──────  ─ source pitch      ║ │
│ ║      ┊      ·  ✦ ✦ ✦       ·   ✦ ✦ ✦       ⊖(repulsor)                    ║ │
│ ║      ┊        ·   ·   ·              ·                                     ║ │
│ ║ -24st┊                                                                     ║ │
│ ║       └────────────────────── buffer time ──────────────────────────────┘  ║ │
│ ╚════════════════════════════════════════════════════════════════════════════╝ │
│                                                                                │
│  [ Voices 64 ] [ Spread 70% ] [ Bright 0 ] [ Sync ○ ] [ Trail 60% ]            │
├────────────────────────────────────────────────────────────────────────────────┤
│              Dry/Wet  ●━━━━━━━━━━━━━━━━━━ 100%    Out  ━━━━●━━━━ 0dB  ▮ meter  │
└────────────────────────────────────────────────────────────────────────────────┘
```

### Regions, top to bottom

1. **Title bar** — name, preset menu, A/B compare, help, settings gear.
2. **Stage strip** — four side-by-side stage panels (Capture, Shatter, Drift, Bloom), each a small card with that stage's parameters and an enable LED. Clicking the stage name expands it temporarily into a larger view; double-click bypasses. Active stage lights up.
3. **Source ribbon** — a thin horizontal waveform of the current capture buffer with a moving playhead and a prominent freeze toggle (snowflake icon) overlaid. Clicking on the ribbon repositions the grain read head.
4. **Grain Constellation** — the centerpiece. The biggest region on screen.
5. **Secondary controls** — a thin row of toggles and small sliders for voice limit, stereo spread, brightness bias, tempo sync, visual trail length.
6. **Output strip** — dry/wet, output gain, output level meter.

### Grain Constellation interaction

- **Axes:** X is buffer time (left = start of buffer, right = end). Y is pitch deviation from source (centerline = 0 semitones, top = +24, bottom = -24).
- **Grains** appear as glowing dots with motion trails. Brightness ∝ grain amplitude. Hue ∝ pitch deviation (cool blue below source, warm orange above).
- **Click + drag** in empty space → paints an **attractor** well at click point. Drag radius = well radius. Releases as a soft circular glow with a `⊕` glyph.
- **Right-click + drag** → paints a **repulsor** well. Same gesture, opposite polarity, `⊖` glyph.
- **Click on existing well** → selects it. Drag handles around it for radius. Press Delete or click the small × to remove. Shift-drag the centre to move.
- **Double-click empty space** → clears all wells.
- **Cmd/Ctrl-click** on a well → toggles attractor/repulsor.
- Up to 16 wells are allowed; past that the oldest is removed and a small message fades in.
- A subtle horizontal grid at semitone intervals (every 12) and a vertical grid at buffer subdivisions (quarters of buffer length). Grids dim while the user is dragging.
- **Empty constellation state** (no wells) shows ghost copy: "Click to attract grains. Right-click to repel." It fades on first interaction.

## Visual Notes

### Aesthetic
- **Mood:** deep-space observatory. Dark navy-to-black radial gradient background. Grains and wells are the only bright elements.
- **Palette (placeholder until user supplies brand):**
  - Background: `#070a14` (deep) → `#0e1426` (vignette)
  - Grain warm: `#ffb47a` (above pitch)
  - Grain cool: `#7ac9ff` (below pitch)
  - Attractor well: `#7afff0` translucent (cyan-mint glow)
  - Repulsor well: `#ff6f9c` translucent (magenta-pink glow)
  - Text primary: `#e8ecf5`, secondary: `#8a95ad`
  - Stage card border (active): `#7afff0`, inactive: `#2a3550`
- **Typography:** No bundled brand. Default to JUCE's bundled `getDefaultSansSerifFont()` until user picks a brand. If user opts in to bundled fonts, use **Panchang** for the title and stage names, **Satoshi** for everything else (see `resources/fonts/`).

### Motion
- Grains render at 60 fps via a `juce::OpenGLContext` overlay so the swarm stays smooth even with 200+ grains.
- Trails are alpha-decaying sprites (cheaper than line strips for many grains).
- Wells gently pulse (1.5 s breathing cycle) to read as "alive" even when audio is silent.
- Stage panel LEDs throb when active and dim when bypassed.

### Accessibility
- Every gestural action on the Constellation has a keyboard/mouse fallback in a `⚙ settings → wells` list view (add, edit numerically, delete). Important for users with mobility constraints and for screenshot/preset reviewing.
- Parameter readouts shown numerically on each card; not just visual.
- High-contrast mode toggle in settings (boosts background to near-black and grain colours to pure RGB primaries).

### Brand placeholder
This v1 mockup uses placeholder colours and the JUCE default font. When the user is ready to brand it, capture choices in a new `ui-mockup-v2.md` per `rules/brand.md`.
