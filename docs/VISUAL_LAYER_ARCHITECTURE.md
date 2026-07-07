# Visual Layer Architecture

Visual polish is intentionally postponed until core apps are stable.

Goal: ABVx should support both a minimal utility mode and a more expressive art mode without hurting battery life or app reliability.

## Modes

### Minimal mode

Default productive mode.

- Black background.
- High-contrast text.
- Low redraw rate.
- Few or no animations.
- Best battery behavior.

### Art mode

Optional expressive mode.

- Boot/sleep/idle screens.
- Cyberpunk-style monochrome/limited-color animations.
- Animated launch transitions.
- Music/Record visualizers.
- More visible personality and atmosphere.

Art mode must be optional and disableable in Settings.

## Design constraints

- Never block audio playback.
- Never destabilize SD operations.
- Avoid constant full-screen redraws during Music/Record playback.
- Keep animations low-cost and frame-limited.
- Respect screen timeout and power-save settings.
- All effects should degrade gracefully to static UI.

## Candidate features

- Additional boot splash variants.
- Idle/sleep clock visual.
- Launcher transition animation.
- App-open/app-close scanline effect.
- Music waveform themes.
- Recorder waveform themes.
- Charging/battery animation.
- Low-power animated screensaver.

## Settings integration

Future Settings options:

```text
VISUAL: MIN / ART
ANIM: OFF / LOW / FULL
SAVER: OFF / CLOCK / ART
```

## Implementation direction

Use a small visual layer rather than per-app ad-hoc effects:

- `drawTransition(from, to)`
- `drawIdleVisual()`
- `drawBootVariant()`
- `drawSignalWaveform()`
- `visualAccentColor()`

Apps should call shared visual helpers, not own custom animation loops.

## Priority

Final polish phase only.

Do not prioritize visual layer over:

- Music stability;
- Recorder reliability;
- Reader/Notes correctness;
- SD safety;
- Mac Companion basics;
- Connections transfer stability.
