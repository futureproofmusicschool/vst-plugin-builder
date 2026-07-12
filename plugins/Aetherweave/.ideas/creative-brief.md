# Aetherweave

## Concept
Aetherweave is a four-stage granular processor that turns any incoming audio into a sculptable cloud of luminous grains. Each stage — **Capture**, **Shatter**, **Drift**, **Bloom** — adds a distinct transformation, and the user shapes the result not through traditional knobs alone but by painting **gravity wells** onto a 2D Grain Constellation. Wells attract or repel grains across a time-vs-pitch field, so the user is literally choreographing a swarm of sound particles instead of dialing parameters.

The audible result is something between a frozen orchestral nebula, a granular pad, and an alien resonator — controllable enough to be musical, alive enough to be surprising.

## Target User
- Sound designers and ambient/experimental producers who already know granular synthesis exists but find conventional grain plugins (PaulStretch, Granulator, Quanta) either too academic or too rigid.
- Film, game, and trailer composers who need otherworldly textures fast.
- Producers in genres adjacent to ambient, IDM, hyperpop, post-rock, and modern classical — anywhere "what is that sound?" is an asset.
- Skill level: intermediate to advanced, but the gestural Constellation UI is approachable enough that beginners can stumble into great results.

## Key Differentiator
Three things no shipping granular plugin combines today:

1. **Gravity-well grain control.** Instead of a "pitch scatter" knob, the user paints attractors and repulsors in a 2D phase space (time × pitch). Grains physically swarm toward and away from these wells. The motion is visible and audible at once.
2. **Explicit multistage pipeline.** Capture → Shatter → Drift → Bloom are visible as four lit panels along the top of the UI. The user can freeze the buffer at Capture, change grain birth behavior at Shatter, animate the swarm at Drift, and finish with spectral shimmer at Bloom. Each stage can be bypassed independently.
3. **Living visualization.** The Constellation is not a meter — it is the instrument. Grains render as glowing particles with motion trails, their brightness mapped to amplitude and their colour mapped to pitch deviation from the source. The same view is both the control surface and the analyzer.

## Audio Characteristics
- **Capture** holds a rolling 1–8 second buffer of the input. A freeze toggle locks the buffer so the source can stop playing while the plugin keeps weaving. Otherwise the buffer slides forward in time with input.
- **Shatter** spawns grains from the buffer with controllable grain size (5–500 ms), density (1–200 grains/sec), and stochastic position/pitch jitter. Windowing is a Hann/Tukey blend with a tilt control.
- **Drift** is where the gravity-well physics live. Each grain has a position in (time, pitch) space; wells exert force on grains, so the audible distribution shifts based on where the user paints. Includes a global "swarm temperature" — at low temp grains hold tight to wells, at high temp they break free and scatter.
- **Bloom** is a parallel spectral shimmer + diffusion reverb tail. Pitch shifts the wet path by intervals (octave up by default), feeds it back through a long reverb, and blends with dry. Gives the cloud a halo and a sense of vast space.

Sonically: think a string section recorded on Mars, frozen mid-phrase, then exhaled into a cathedral. Or a vocal turned into rain. Or a hi-hat turned into a constellation of insect wings.
